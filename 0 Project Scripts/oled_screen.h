#ifndef OLED_SCREEN
#define OLED_SCREEN

#include <stdio.h>
#include "diag/Trace.h"
#include <string.h>

#include "cmsis/cmsis_device.h"

// Constants
#define myTIM3_PRESCALER ((uint16_t)48000U)
#define myTIMx_PERIOD (0xFFFFFFFF)
#define STARTING_COL (uint8_t)1U
#define REFRESH_PERIOD ((uint16_t)100U)

// Function Prototypes
void myGPIOB_Init(void);
void mySPI_Init(void);
void myTIM3_Init();


void oled_Write(unsigned char);
void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);

void oled_config(void);
void set_Page(uint8_t page);
void set_Segment(uint8_t seg);

void refresh_OLED( unsigned int res, unsigned int freq, uint8_t freq_number );
void refresh_OLED_test(void);
void TIM3_delay(uint8_t milliseconds);


#endif
