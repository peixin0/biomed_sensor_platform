#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>     // Required for exit()
#include <unistd.h>     // Required for close()
#include <fcntl.h>
#include <stdint.h>
#include "../include/hw_address.h"
// This test program demonstrates how to use memory-mapped I/O to read from and write to the FPGA peripherals connected via the Lightweight HPS-to-FPGA AXI Bridge.
// It configures the GPIO directions for JP1 and JP2, then continuously reads the state
#define HW_REGS_SPAN         ( 0x00200000 )
#define HW_REGS_MASK         ( HW_REGS_SPAN - 1 )

// Calculate the base offsets for JP1 and JP2 GPIOs based on the defined physical addresses and the HPS lightweight bridge base
#define JP1_BASE_OFFSET            ( (uint32_t)LSC_BASE_GPIO_JP1 - (uint32_t)HPS_LWMASTER_BASE)
#define JP2_BASE_OFFSET            ( (uint32_t)LSC_BASE_GPIO_JP2 - (uint32_t)HPS_LWMASTER_BASE)
// JP1&JP2 Register offsets for JP1 and JP2 GPIOs
#define GPIO_REG             0x00/sizeof(int)
#define GPIO_DIR             0x04/sizeof(int)
#define GPIO_INTRRPT         0x08/sizeof(int)
#define GPIO_EDGE_CAP        0x0C/sizeof(int)
 

int main(void)
{ 
    int fd;
    void *virtual_base;
    // Open /dev/mem
    if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
            printf( "ERROR: could not open \"/dev/mem\"...\n" );
            exit(1);
    }
    
    // get virtual addr that maps to physical
    virtual_base = mmap(NULL,HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED,fd,HPS_LWMASTER_BASE);
    if( virtual_base == MAP_FAILED ) {
        perror( "ERROR: mmap() failed..." );
        close(fd);
        exit(1);
    }
   // init interface directions
   volatile uint32_t *jp2_dir = ( uint32_t *)(virtual_base + JP2_BASE_OFFSET + GPIO_DIR);
   *jp2_dir = 0; //set every JP2 bit direction to input
   volatile uint32_t *jp1_dir = ( uint32_t *)(virtual_base + JP1_BASE_OFFSET + GPIO_DIR);
   *jp1_dir = 0xffffffff; //set every JP1 bit dir to output
    volatile uint32_t jp2_value = *(uint32_t *)(virtual_base +JP2_BASE_OFFSET + GPIO_REG);
   while (1)
   {
      *jp1_dir = *jp2_dir; 
      printf("JP2 Value: 0x%02X\n", jp2_value);
   }
   if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
        printf( "ERROR: munmap() failed...\n" );
    }
    close( fd );
    return 0;
}