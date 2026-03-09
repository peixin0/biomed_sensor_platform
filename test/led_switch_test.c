#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#define ADDR_JP1PORT        ((volatile char *) 0xFF200060)
#define ADDR_JP2PORT        ((volatile char *) 0xFF200070)

#define PIN_REG             0x00/sizeof(int)
#define PIN_DIR             0x04/sizeof(int)
#define PIN_INTRRPT         0x08/sizeof(int)
#define PIN_EDGE_CAP        0x0C/sizeof(int)


bool port_init(int )



int main()
{ 
   // init interface directions
   *(ADDR_JP2PORT+4) = 0; //set every JP2 bit direction to input
   *(ADDR_JP1PORT+4) = 0xffffffff; //set every JP1 bit dir to output

   volatile int 
   while (1)
   {
      *ADDR_JP1PORT = *ADDR_JP2PORT;
      printf
   }
}