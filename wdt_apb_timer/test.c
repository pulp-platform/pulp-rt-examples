/*
 * Copyright (C) 2019 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Antonio Mastrandrea
 */

#include <rt/rt_api.h>


#define APB_WDT_ADDR      	 0X1A130000 
#define OFFSET_CONFIG_WDT   	 0x00 //CONTAINS A READ-ONLY Signature
#define OFFSET_INIT_VALUE   	 0x04 //R/W REGISTER AS SCRATCH
#define OFFSET_COUNTER_VALUE	 0x08 // R/W reg


int main()
{
  unsigned int RST;
  volatile unsigned int* apb_wdt_config;
  volatile unsigned int* apb_wdt_init_value;
  volatile unsigned int* apb_wdt_counter_value;




  apb_wdt_config             = (volatile unsigned int* )(APB_WDT_ADDR + OFFSET_CONFIG_WDT);
  apb_wdt_init_value         = (volatile unsigned int* )(APB_WDT_ADDR + OFFSET_INIT_VALUE);
  apb_wdt_counter_value      = (volatile unsigned int* )(APB_WDT_ADDR + OFFSET_COUNTER_VALUE);

  if((*(apb_wdt_config))==0) RST=0;
  else RST=1;

  printf("\nRST: 0x%x\n",RST);


  printf("\nTEST WDT:\n");

  printf("\nREAD WDT registers:\n");
  printf("\tconfig wdt       is 0x%x\n", *(apb_wdt_config));
  printf("\tinit_value       is 0x%x\n", *(apb_wdt_init_value)); 
  printf("\tcounter value    is 0x%x\n", *(apb_wdt_counter_value)); 

  printf("\nWRITE WDT INIT register:\n");
  *(apb_wdt_init_value) =0xFFFF0000;
  printf("\tconfig wdt       is 0x%x\n", *(apb_wdt_config));
  printf("\tinit_value       is 0x%x\n", *(apb_wdt_init_value)); 
  printf("\tcounter value    is 0x%x\n", *(apb_wdt_counter_value)); 

  printf("\nCLEAR WDT registers\n");
  *(apb_wdt_config) = 2;
  printf("\tconfig wdt       is 0x%x\n", *(apb_wdt_config));
  printf("\tinit_value       is 0x%x\n", *(apb_wdt_init_value)); 
  printf("\tcounter value    is 0x%x\n", *(apb_wdt_counter_value));
  

  printf("\nENABLE WDT registers\n");
  *(apb_wdt_config) = 1;
  printf("config wdt       is 0x%x\n", *(apb_wdt_config));

  printf("Counter value:\n");
  int i=0;
  for(i=0;i<10;i++)
  {
	printf("\t%d\tcounter value    is 0x%x\n",i, (unsigned int)(*(apb_wdt_counter_value))); 
  }


  printf("\nWRITE WDT INIT register:\n");
  *(apb_wdt_init_value) =0xFFFF9000;
  printf("\tconfig wdt       is 0x%x\n", *(apb_wdt_config));
  printf("\tinit_value       is 0x%x\n", *(apb_wdt_init_value)); 
  printf("\tcounter value    is 0x%x\n", *(apb_wdt_counter_value));
  

  printf("\nCLEAR WDT register\n");
  *(apb_wdt_config) = 3;
  printf("\tconfig wdt       is 0x%x\n", *(apb_wdt_config));
  printf("\tinit_value       is 0x%x\n", *(apb_wdt_init_value)); 
  printf("\tcounter value    is 0x%x\n", *(apb_wdt_counter_value));


  if(RST==0)
  {
	  printf("\nThe WDT reset the CORE one time!!!\n"); 

	  for(i=0;i<601;i++)
	  {
		if (i%100 == 0)  printf("%d\tcounter value    is 0x%x\n",i, (unsigned int)(*(apb_wdt_counter_value)));
	  }
  }  

  printf("Enjoy!!! The test work!\n");

  return 0;
}

