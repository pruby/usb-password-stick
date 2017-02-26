#include <stdint.h>
#include <stdio.h>
#include "pwgen.h"

int main (int argc, char** argv)
{
  char buf[64];
  
  pwgen_state_t pwgen;
  pwgen_init(&pwgen);
  while(1)
  {
    pwgen_output(&pwgen, (uint8_t*) buf, 64);
    printf("%s\n", buf);
  }
}