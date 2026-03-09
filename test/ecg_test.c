#include <stdio.h>
#include <stdlib.h>     // Required for exit()
#include <unistd.h>     // Required for close()
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include "../include/hw_address.h"

// Note: For production biomedical platforms, avoid mapping the entire 2MB span.
#define HW_REGS_SPAN   ( 0x00200000 )
#define HW_REGS_MASK   ( HW_REGS_SPAN - 1 )

// Cast pointers to integers to safely calculate byte offsets
#define JP1_BASE_OFFSET  ((uint32_t)LSC_BASE_GPIO_JP1 - (uint32_t)HPS_LWMASTER_BASE)
#define JP2_BASE_OFFSET  ((uint32_t)LSC_BASE_GPIO_JP2 - (uint32_t)HPS_LWMASTER_BASE)

// Define register offsets strictly as bytes
#define GPIO_REG         0x00
#define GPIO_DIR         0x04
#define GPIO_INTRRPT     0x08
#define GPIO_EDGE_CAP    0x0C

int main()
{ 
    int fd;
    void *virtual_base;

    // Open /dev/mem
    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
        printf( "ERROR: could not open \"/dev/mem\"...\n" );
        exit(1);
    }
    
    // Get virtual addr that maps to physical. Cast base to off_t.
    virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, (off_t)HPS_LWMASTER_BASE);
    if( virtual_base == MAP_FAILED ) {
        perror( "ERROR: mmap() failed..." );
        close(fd);
        exit(1);
    }

    // Safely calculate pointers using byte-level arithmetic (uint8_t *), then cast to volatile 32-bit pointers
    volatile uint32_t *jp1_dir  = (volatile uint32_t *)((uint8_t *)virtual_base + JP1_BASE_OFFSET + GPIO_DIR);
    volatile uint32_t *jp1_data = (volatile uint32_t *)((uint8_t *)virtual_base + JP1_BASE_OFFSET + GPIO_REG);
    
    volatile uint32_t *jp2_dir  = (volatile uint32_t *)((uint8_t *)virtual_base + JP2_BASE_OFFSET + GPIO_DIR);
    volatile uint32_t *jp2_data = (volatile uint32_t *)((uint8_t *)virtual_base + JP2_BASE_OFFSET + GPIO_REG);

    // Initialize interface directions
    *jp2_dir = 0x00000000; // Set every JP2 bit direction to input
    *jp1_dir = 0xFFFFFFFF; // Set every JP1 bit direction to output

    printf("Starting GPIO mirror loop. Press Ctrl+C to exit.\n");

    while (1)
    {
        // Continuously read the physical state of JP2
        uint32_t current_jp2_value = *jp2_data;
        
        // Write that state out to JP1
        *jp1_data = current_jp2_value; 
        
        // Optional: Print the value (add a small sleep to prevent terminal flooding)
        // printf("JP2 Value: 0x%08X\n", current_jp2_value);
        // usleep(100000); // Sleep for 100ms
    }

    // Cleanup (though unreachable in this while(1) loop without a signal handler)
    if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
        printf( "ERROR: munmap() failed...\n" );
    }
    close( fd );
    return 0;
}