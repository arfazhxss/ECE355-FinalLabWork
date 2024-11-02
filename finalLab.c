//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).


/*****************************************************************/
/**                              PRAGMA                         **/
/*****************************************************************/

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/*****************************************************************/
/**                             DEFINES                         **/
/*****************************************************************/

/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */

/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

/* TEST PRINTS */
#define MAIN_DEBUG 1
#define ADC_DEBUG 1
#define EXTI_DEBUG 0

/*****************************************************************/
/**                            TYPEDEFS                         **/
/*****************************************************************/

/*
 * These object-oriented structs are intended to be used as singletons
 * to wrap ADC, DAC, and SPI functionality.
 * This will make the code cleaner
 * This pattern is used in lots of system implementations,
 * for example the linux kernel
 */

/************************** ADC TYPEDEFS **************************/

typedef struct ADC_Typedef ADC_Typedef; 		// forward declaration

typedef struct ADC_Data_Typedef
{
	uint32_t reading;
	uint32_t resistance;
} ADC_Data_Typedef;

typedef struct ADC_Typedef{
	ADC_Data_Typedef data; 						// Variable Members
	void (*read)(volatile ADC_Typedef*); 		// Function Members
} ADC_Typedef;

/************************** DAC TYPEDEFS **************************/

//typedef struct DAC_Typedef DAC_Typedef; 		// forward declaration

/************************** PWM TYPEDEFS **************************/

//typedef struct PWM_Typedef PWM_Typedef; 		// forward declaration

/************************** LCD TYPEDEFS **************************/

//typedef struct LCD_Typedef LCD_Typedef; 		// forward declaration

/*****************************************************************/
/**                         PROTOTYPES                          **/
/*****************************************************************/

void myGPIOA_Init(void);
void myTIM2_Init(void);
void EXTI_Init(void);

/************************** ADC & DAC Prototypes **************************/
void myADC_Init(void);
void ADC_read_impl(volatile ADC_Typedef* self);	// ADC Read Method Implementation Prototype
void cons_ADC(volatile ADC_Typedef* self);		// ADC constructor Prototype
uint32_t readADC(void);
uint32_t toOhms(uint32_t adc_val);


// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.

/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{
	// Disable the PLL
    RCC->CR &= ~(RCC_CR_PLLON);
    // Wait for the PLL to unlock
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
    // Configure the PLL for 48-MHz system clock
    RCC->CFGR = 0x00280000;
    // Enable the PLL
    RCC->CR |= RCC_CR_PLLON;
    // Wait for the PLL to lock
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
    // Switch the processor to the PLL clock source
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
    // Update the system with the new clock frequency
    SystemCoreClockUpdate();

}

/*****************************************************************/
/**                       Global Variables                      **/
/*****************************************************************/

volatile ADC_Typedef pot;  //potentiometer

/*****************************************************************/
/**                              MAIN                           **/
/*****************************************************************/

int main(int argc, char* argv[])
{

	SystemClock48MHz();
	if (MAIN_DEBUG)
	{
		trace_printf("Arfaz and Aly's ECE355 Final Project\n");
		trace_printf("System clock: %u Hz\n\n", SystemCoreClock);
	}

	myGPIOA_Init();				// Initialize I/O port PA
	myTIM2_Init();				// Initialize timer TIM2
	EXTI_Init();				// Initialize EXTI


	cons_ADC ( &pot );	// Initialize potentiometer resource (ADC)
	while (1) {}
	return 0;

}


void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	// Relevant register: RCC->AHBENR
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA2 as input */
	// Relevant register: GPIOA->MODER
	//	GPIOA->MODER &= 0xFFFFFFFCC;
	GPIOA->MODER &= ~(GPIO_MODER_MODER2);


	/*Ensure no pull-up/pull-down for PA0*/
	//	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

	/* Ensure no pull-up/pull-down for PA2 */
	// Relevant register: GPIOA->PUPDR
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);
}


void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	// Relevant register: TIM2->CR1
	TIM2->CR1 = ((uint16_t)0x008C);
	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;
	/* Update timer registers */
	// Relevant register: TIM2->EGR
	TIM2->EGR |= ((uint16_t)0x0001);
	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
	NVIC_SetPriority(TIM2_IRQn, 0);
	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(TIM2_IRQn);
	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
	TIM2->DIER |= TIM_DIER_UIE;
}

void EXTI_Init() {
	/* Map EXTI2 and EXTI0 line to PA2 and PA0 respectively */
	// Relevant register: SYSCFG -> EXTICR[0]
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0 | SYSCFG_EXTICR1_EXTI2);
	SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA | SYSCFG_EXTICR1_EXTI2_PA);


	// SYSCFG->EXTICR[0] &= 0xFF0F;

	/* EXTI2 and EXTI0 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= (EXTI_RTSR_TR0 | EXTI_RTSR_TR2);

	/* Unmask interrupts from EXTI2 and EXTI0 line */
	// Relevant register: EXTI->IMR
	//	EXTI-> IMR |= 0x05;
	EXTI->IMR |= (EXTI_IMR_IM0 | EXTI_IMR_IM2);

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	/* Enable EXTI2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	NVIC_SetPriority(EXTI2_3_IRQn, 1);
	/* Enable EXTI2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(EXTI2_3_IRQn);
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		TIM2->SR &= ~TIM_SR_UIF;		// Clear update interrupt flag
		TIM2->CR1 |= TIM_CR1_CEN;		// Restart stopped timer
	}
}

void EXTI0_1_IRQHandler()
{

	if ((EXTI->PR & EXTI_PR_PR0) != 0){
		if((GPIOA->IDR & GPIO_IDR_0) != 0){
			// Wait for button to be released (PA0 = 0)
			while((GPIOA->IDR & GPIO_IDR_0) != 0) {}
			trace_printf("<<<<<<<<<<<<<<< Button detected >>>>>>>>>>>>>>>\n");
		}
	}
	EXTI->PR |= EXTI_PR_PR0;
}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...
    unsigned int count = 0;
    float period = 0;
    float frequency = 0;

	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{
		//
		// 1. If this is the first edge:
		//	- Clear count register (TIM2->CNT).
		//	- Start timer (TIM2->CR1).
		//    Else (this is the second edge):
		//	- Stop timer (TIM2->CR1).
		//	- Read out count register (TIM2->CNT).
		//	- Calculate signal period and frequency.
		//	- Print calculated values to the console.
		//	  NOTE: Function trace_printf does not work
		//	  with floating-point numbers: you must use
		//	  "unsigned int" type to print your signal
		//	  period and frequency.
		//
		if((TIM2->CR1 & TIM_CR1_CEN) == 0){
			TIM2->CNT = 0;
			TIM2->CR1 |= TIM_CR1_CEN;
		}
		else{
            TIM2->CR1 &= ~TIM_CR1_CEN;
            count = TIM2->CNT;
            period = (float)count/(float)SystemCoreClock;
            frequency = 1/period;
            if (EXTI_DEBUG) {
            	trace_printf("Count: %u\n", count);
            	trace_printf("Period: %u\n", (unsigned int)(period*1000000));
            	trace_printf("Frequency: %u\n", (unsigned int)frequency);
            }
		}
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		//
		EXTI->PR |= EXTI_PR_PR2;
	}
}

/*****************************************************************/
/**                           UTILITIES                         **/
/*****************************************************************/

// Analog to Digital Conversion
uint32_t readADC(void)
{
	/// Start the conversion process of ADC Control Register
	ADC1->CR |= ADC_CR_ADSTART; 			// ADC group regular conversion start
	/// Waiting until the ADC1's end-of-conversion (EOC) flag is set.
	while (!(ADC1->ISR & ADC_ISR_EOC));
	ADC1->ISR &= ~(ADC_ISR_EOC); 			// Reset end of conversion flag
	return (uint32_t)((ADC1->DR) & ADC_DR_DATA);
}

/*** Initializing Analog to Digital Conversion ***/
void myADC_Init() {
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 	// Enabling ADC1 clock
	/// If the ADC is enabled, it disables the ADC by setting the ADDIS bit.
	if ((ADC1->CR & ADC_CR_ADEN) != 0) { ADC1->CR |= ADC_CR_ADDIS; }
	while ((ADC1->CR & ADC_CR_ADEN) != 0); 	// Wait until ADC is disabled

	if (ADC_DEBUG) { trace_printf("Start ADC Calibration\n"); }
	ADC1->CR = ADC_CR_ADCAL;				// Start ADC self-calibration process
	while (ADC1->CR == ADC_CR_ADCAL);		// Wait until ADC calibration completes
	if (ADC_DEBUG) { trace_printf("Finished ADC calibration\n"); }

	/// Set ADC to continuous conversion mode and overwrite old data on overrun
	ADC1->CFGR1 |= (ADC_CFGR1_CONT | ADC_CFGR1_OVRMOD);
	ADC1->CHSELR = ADC_CHSELR_CHSEL1; 		// Select channel 1 for ADC conversion (PROBABLY)
	if (ADC_DEBUG) { trace_printf("Start Enabling ADC, waiting for acknowledgment\n"); }

	ADC1->CR |= ADC_CR_ADEN; 				// Enable ADC by setting the ADEN bit high
	while (!(ADC1->ISR & ADC_ISR_ADRDY)); 	// Wait until ADC is ready for conversion
	if (ADC_DEBUG) { trace_printf("ADC Enabled\n"); }
}

// convert ADC reading to resistance
uint32_t toOhms(uint32_t adc_val)
{
	return (uint32_t)((((float)adc_val)/4096.0) * 5000.0);
}

// Implementing ADC_Typedef.read
void ADC_read_impl(volatile ADC_Typedef* self)
{
	self->data.reading = readADC();							// read ADC and save value
	self->data.resistance = toOhms(self->data.reading);		// convert to resistance and save value
}

// ADC_Typedef Constructor
void cons_ADC(volatile ADC_Typedef* self)
{
	self->data.reading = 0;
	self->data.resistance = 0;			// initialize default values
	self->read = &ADC_read_impl;		// associate function pointer

	myGPIOA_Init();
	myADC_Init();						// init ADC with PA2 as analog input
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
