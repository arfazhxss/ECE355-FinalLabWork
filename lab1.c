// --------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: CENG 355 "Microprocessor-Based Systems".
// See "system/include/cmsis/stm32f0xx.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f0xx.c" for handler declarations.
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//                                 INCLUDES
// --------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// --------------------------------------------------------------------------
//                                  PRAGMA
// --------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


// --------------------------------------------------------------------------
//                                  DEFINE
// --------------------------------------------------------------------------

/* Clock prescaler for TIM2 timer: no prescaling */
#define TIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define TIM2_PERIOD ((uint32_t)0xFFFFFFFF)

// TIM3 controls LCD refresh
// want to refresh 5 times per second
// want to tick every millisecond
// core clock: 48000000
//so need to divide by 48000 to get 1000 ticks per second
#define TIM3_PRESCALER ((uint16_t)(48000))
// want to update LCD every 200 ms
#define TIM3_PERIOD ((uint16_t)(200))

// TIM16 is used for setting delays
// we want to count milliseconds to same as for TIM3
#define TIM16_PRESCALER ((uint16_t)(48000))
// and just need a default value for the period
#define TIM16_PERIOD ((uint16_t)(200))

#define DAC_MAX_VAL (4095)
// voltage output when DAC_MAX_VAL is written to DAC
#define DAC_MAX_VOLTS (2.92)
// forward voltage on optocoupler (from datasheet) 
#define OPTO_DROP (0.9) 

// define PB4 as LCK for LCD
#define LCK_PIN ((uint16_t)0x0010)

//LCD defines

// code for LCD command
#define LCD_CMD  ((uint8_t)(0x00))
//code for LCD character
#define LCD_CHAR ((uint8_t)(0x40))
//code for pulsen the LCD EN
#define LCD_EN ((uint8_t)(0x80))

// Clear command
#define LCD_CMD_CLEAR ((uint8_t)0x01)

// LCD ROW address codes
#define LCD_ROW_F ((uint8_t)0x80)
#define LCD_ROW_R ((uint8_t)0xC0)

// LCD Digit address codes
#define D0 ((uint8_t)0x00)
#define D1 ((uint8_t)0x01)
#define D2 ((uint8_t)0x02)
#define D3 ((uint8_t)0x03)
#define D4 ((uint8_t)0x04)
#define D5 ((uint8_t)0x05)
#define D6 ((uint8_t)0x06)
#define D7 ((uint8_t)0x07)


// DEBUG flags to print
// traces only when we want to
// 0 is silent, 1 is verbose

#define MAIN_DEBUG 0
#define ADC_DEBUG 0
#define EXTI_DEBUG 0
#define TIM2_DEBUG 0
#define TIM3_DEBUG 0
#define TIM16_DEBUG 0
#define SPI_DEBUG 0
#define LCD_DEBUG 0


// --------------------------------------------------------------------------
//                                TYPEDEFS
// --------------------------------------------------------------------------

/*
 * These object-oriented structs are intended to be used as singletons 
 * to wrap ADC, DAC, and SPI functionality.
 * This will make the code cleaner
 * This pattern is used in lots of system implementations,
 * for example the linux kernel
 */
typedef struct ADC_Typedef ADC_Typedef; // forward declaration
typedef struct DAC_Typedef DAC_Typedef; // forward declaration
typedef struct PWM_Typedef PWM_Typedef; // forward declaration
typedef struct LCD_Typedef LCD_Typedef; // forward declaration




// ------- ADC --------

typedef struct ADC_Data_Typedef
{
	uint32_t reading;
	uint32_t resistance;
} ADC_Data_Typedef;

typedef struct ADC_Typedef{

	// --------- Variable Members ----------
	ADC_Data_Typedef data;

	// --------- Function Members ----------
	void (*read)(volatile ADC_Typedef*);
} ADC_Typedef;

// prototype of ADC read method implementation
void ADC_read_impl(volatile ADC_Typedef* self);
// prototype of ADC constructor
void ADC_struct_init(volatile ADC_Typedef* self);





// ------- DAC --------

typedef struct DAC_Data_Typedef
{
	uint32_t value;
	float voltage;
} DAC_Data_Typedef;

typedef struct DAC_Typedef{

	// --------- Variable Members ----------
	DAC_Data_Typedef data;

	// --------- Function Members ----------
	void (*write)(volatile DAC_Typedef*, ADC_Data_Typedef);
} DAC_Typedef;

// prototype of DAC write method implementation
void DAC_write_impl(volatile DAC_Typedef* self, ADC_Data_Typedef d);
// prototype of constructor
void DAC_struct_init(volatile DAC_Typedef* self);




// ------- PWM --------

typedef struct PWM_Data_Typedef
{
	double period;
	double frequency;
} PWM_Data_Typedef;

typedef enum Edge_Sequence_Typedef
{
	FIRST_EDGE,
	SECOND_EDGE,
} Edge_Sequence_Typedef;

typedef struct PWM_Typedef
{
	// --------- Variable Members ----------
	PWM_Data_Typedef data; // global signal frequency and period data
	// count of which edge has been seen on the signal waveform
	Edge_Sequence_Typedef edge; 

	// --------- Function Members ----------
	// none
} PWM_Typedef;

// prototype of constructor
void PWM_struct_init(volatile PWM_Typedef* self);




// ------- LCD --------

typedef struct LCD_Typedef
{
	// --------- Variable Members ----------
	// --------- Function Members ----------

	// writes frequency and resistance to LCD
	void (*write)(volatile LCD_Typedef*, uint32_t, uint32_t);
} LCD_Typedef;

void LCD_write_impl(volatile LCD_Typedef* self,
					uint32_t freq, uint32_t ohms);
void LCD_struct_init(volatile LCD_Typedef* self);


// --------------------------------------------------------------------
//                                PROTOTYPES
// --------------------------------------------------------------------

void myGPIOB_Init(void);

void myTIM2_Init(void);
void myTIM3_Init(void);
void myTIM16_Init(void);
void myEXTI_Init(void);

void myADC_Init(void);
void myDAC_Init(void);
void mySPI_Init(void);

uint32_t readADC(void);
uint32_t toOhms(uint32_t adc_val);
DAC_Data_Typedef toDacVal(uint32_t adc_val);

void writeDAC(uint32_t dac_val);
void delayMs(uint16_t ms);

void SPI_SendByte(uint8_t data);
void SPI_lock(void);
void SPI_unlock(void);

void LCD_SendByte(uint8_t msg_type, uint8_t msg);
void LCD_Clear(void);
void LCD_ToDigit(uint8_t row, uint8_t digit);

void LCD_WriteChar(uint8_t c);
void LCD_WriteNum(uint32_t num);
void LCD_WriteFreq(uint32_t freq);
void LCD_WriteOhms(uint32_t ohms);

// --------------------------------------------------------------------
//                                 GLOBALS
// --------------------------------------------------------------------

// These global variables represent the singleton resources
volatile ADC_Typedef pot;  //potentiometer
volatile DAC_Typedef opto; // optocoupler
volatile PWM_Typedef pwm;  // PWM signal
volatile LCD_Typedef lcd;  // LCD display

// --------------------------------------------------------------------
//                                   MAIN
// --------------------------------------------------------------------

int main(int argc, char* argv[])
{

	if (MAIN_DEBUG)
	{
		trace_printf("ECE355\n");
		trace_printf("System clock: %u Hz\n\n", SystemCoreClock);
	}

	// Initialize potentiometer resource (ADC)
	ADC_struct_init( &pot );
	// Initialize optocoupler resource (DAC)
	DAC_struct_init( &opto);
	// Initialize PWM signal resource (EXTI, TIM2)
	PWM_struct_init( &pwm );
	// Initialize the LCD resource (GPIOB, TIM3, TIM16, SPI)
	LCD_struct_init( &lcd );

	while (1)
	{
		// read potentiometer
		pot.read(&pot);
		// output processed reading to optocoupler
		opto.write(&opto, pot.data);

		if (MAIN_DEBUG)
		{
			trace_printf("=========== NEW READING ============\n\n");
			trace_printf(">>\t");
			trace_printf("Potentiometer reading:\t%d\n",
							pot.data.reading);
			trace_printf(">>\t");
			trace_printf("Potentiometer resistance:\t%d Ohms\n",
							pot.data.resistance);
			trace_printf("\n");
			trace_printf(">>\t");
			trace_printf("Optocoupler output:\t%d\n",
							opto.data.value);
			trace_printf(">>\t");
			trace_printf("Optocoupler voltage:\t%f Ohms\n",
							opto.data.voltage);
			trace_printf("\n");
		}

	}

	return 0;

}

// --------------------------------------------------------------------
//                                INIT FUNCTIONS
// --------------------------------------------------------------------


void myGPIOB_Init(void)
{
	// Enable clock for GPIOB peripheral
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB4 as output LCK for SPI

	// Configure as output
	GPIOB->MODER |= (GPIO_MODER_MODER4_0);
	// COnfigure push-pull mode
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_4);
	// Configure high-speed mode
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR4);
	// Configure no pull-up/pull-down
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4);

	// Configure PB3 as alternate function 0 (SPI1_SCK)

	// Configure AF0
	GPIOB->MODER |= (GPIO_MODER_MODER3_1);
	// Configure push-pull mode
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_3);
	// Configure high speed mode
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3);
	// Configure no pull-up/pull-down
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3);

	// Configure PB5 as alternate function 0 (SPI1_MOSI)

	// Configure AF0
	GPIOB->MODER |= (GPIO_MODER_MODER5_1);
	// Configure push-pull mode
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_5);
	// Configure high speed mode
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5);
	// Configure no pull-up/pull-down
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR5);
}


void myTIM2_Init()
{
	// TIM2 is used for pulse length counting

	/* Enable clock for TIM2 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM2->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM2->PSC = TIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = TIM2_PERIOD;

	/* Update timer registers */
	TIM2->EGR = ((uint16_t)0x0001);

	/* Assign TIM3 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	TIM2->DIER |= TIM_DIER_UIE;
}


void myTIM3_Init(void)
{
	// TIM3 is used for regularly refreshing the LCD

	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Configure TIM3: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM3->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM3->PSC = TIM3_PRESCALER;
	/* Set auto-reloaded delay */
	TIM3->ARR = TIM3_PERIOD;

	/* Update timer registers */
	TIM3->EGR = ((uint16_t)0x0001);

	/* Assign TIM3 interrupt priority = 1 in NVIC */
	// This is because we don't want to interrupt a pulse length count
	NVIC_SetPriority(TIM3_IRQn, 1);

	/* Enable TIM3 interrupts in NVIC */
	NVIC_EnableIRQ(TIM3_IRQn);

	/* Enable update interrupt generation */
	TIM3->DIER |= TIM_DIER_UIE;

	//Start counting
	TIM3->CR1 |= TIM_CR1_CEN;
}

void myTIM16_Init(void)
{
	// enable the clock to the timer
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	/* Configure TIM16: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM16->CR1 = ((uint16_t)0x008C);

	/* Set clock prescaler value */
	TIM16->PSC = TIM16_PRESCALER;
	/* Set auto-reloaded delay DEFAULT*/
	TIM16->ARR = TIM16_PERIOD;

	/* Update timer registers */
	TIM16->EGR = ((uint16_t)0x0001);

	// no interrupt generation, because we only actually
	// care about the flag, 
	// we wouldn't actually do anything in the handler anyway.
}


void myEXTI_Init()
{
	/* Map EXTI1 line to PA1 */
	SYSCFG->EXTICR[0] = ((uint32_t)0x0000);

	/* EXTI1 line interrupts: set rising-edge trigger */
	EXTI->RTSR = ((uint32_t)0x0002);

	/* Unmask interrupts from EXTI1 line */
	EXTI->IMR = ((uint32_t)0x0002);

	/* Assign EXTI1 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	/* Enable EXTI1 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void myADC_Init(void)
{
	// enable clock input to ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// do some safety stuff
	// found in the appendix of the reference manual
	if ((ADC1->CR & ADC_CR_ADEN) != 0)
		ADC1->CR |= ADC_CR_ADDIS;
	while ((ADC1->CR & ADC_CR_ADEN) != 0);
	ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;

	// start calibration
	if (ADC_DEBUG)
		trace_printf("Start ADC Calibration\n");
	
	ADC1->CR = ADC_CR_ADCAL;
	//wait until calibration finished
	while (ADC1->CR == ADC_CR_ADCAL);
	
	if (ADC_DEBUG)
		trace_printf("Finished ADC calibration\n");

	// continuous conversion and overun
	ADC1->CFGR1 |= (ADC_CFGR1_CONT | ADC_CFGR1_OVRMOD);

	//select channel 0 for PA0
	ADC1->CHSELR = ADC_CHSELR_CHSEL0;

	// enable and wait for ready
	if (ADC_DEBUG)
		trace_printf("Star ADC Enable and wait for ack\n");
	
	ADC1->CR |= ADC_CR_ADEN;
	
	while (!(ADC1->ISR & ADC_ISR_ADRDY));
	
	if (ADC_DEBUG)
		trace_printf("ADC Enabled\n");
}

void myDAC_Init(void)
{
	// enable DAC clock
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	// enable dac
	DAC->CR |= DAC_CR_EN1;
}

void mySPI_Init(void)
{
	// enable clock to SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// basically lifted right from the Interfacing slides
	SPI_InitTypeDef SPI_InitStructInfo;

	SPI_InitStructInfo.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructInfo.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructInfo.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructInfo.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructInfo.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructInfo.SPI_NSS = SPI_NSS_Soft;

	// baud rate prescaler 256 because we want as slow as possible
	// avoids the delay on the LCD as much as possible
	SPI_InitStructInfo.SPI_BaudRatePrescaler
				= SPI_BaudRatePrescaler_256;

	SPI_InitStructInfo.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructInfo.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructInfo);
	SPI_Cmd(SPI1, ENABLE);
}

// --------------------------------------------------------------------
//                        OBJECT METHOD IMPLEMENTATIONS
// --------------------------------------------------------------------

// implementation for ADC_Typedef.read
void ADC_read_impl(volatile ADC_Typedef* self)
{
	// read ADC and save value
	self->data.reading = readADC();
	// convert to resistance and save value
	self->data.resistance = toOhms(self->data.reading);
}

// constructor for ADC_Typedef
void ADC_struct_init(volatile ADC_Typedef* self)
{
	// initialize default values
	self->data.reading = 0;
	self->data.resistance = 0;
	// associate function pointer
	self->read = &ADC_read_impl;
	
	// Enable clock for GPIOA peripheral
	// if not already done
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// Configure PA0 as analog input
	GPIOA->MODER |= GPIO_MODER_MODER0;
	//no pull-up or down
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	myADC_Init();	// init ADC with PA0 as analog input
}

// implementation for DAC_Typedef.write
void DAC_write_impl(volatile DAC_Typedef* self,
					ADC_Data_Typedef d)
{
	// convert ADC value to a data value and voltage
	// and save these
	self->data = toDacVal(d.reading);
	// write the value to the DAC
	writeDAC(self->data.value);
}

// constructor for DAC_Typedef
void DAC_struct_init(volatile DAC_Typedef* self)
{
	// set default values
	self->data.value = 0;
	self->data.voltage = 0.0;
	// associate function pointer
	self->write = &DAC_write_impl;
	
	// Enable clock for GPIOA peripheral
	// if not already done
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// Configure PA4 as analog output
	GPIOA->MODER |= (GPIO_MODER_MODER4);
	GPIOA->MODER &= ~(GPIO_PUPDR_PUPDR4);
	
	// init DAC (automagically configured to PA4)
	myDAC_Init();
}

// constructor for PWM_Typedef
void PWM_struct_init(volatile PWM_Typedef* self)
{
	// set default values
	self->data.period = 0.0;
	self->data.frequency = 0.0;
	self->edge = FIRST_EDGE;
	
	// Enable clock for GPIOA peripheral
	// if not already done
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// Configure PA1 as input for 555 timer
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);
	//no pull-up/pull-down for PA1
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	myTIM2_Init();	// init timer and interrupts
	myEXTI_Init();	// init exti interrupts for PA1
}

// implementation for LCD_Typedef.write
void LCD_write_impl(volatile LCD_Typedef* self,
					uint32_t freq,
					uint32_t ohms)
{
	// write the frequency to the LCD
	LCD_WriteFreq(freq);
	// Write the Resistance to the LCD
	LCD_WriteOhms(ohms);
}

// constructor for LCD_Typedef
void LCD_struct_init(volatile LCD_Typedef* self)
{
	myGPIOB_Init(); // init port B
	mySPI_Init();
	myTIM16_Init(); // TIM16 is delay

	// associate write method to struct
	self->write = &LCD_write_impl;

	// initialize LCD
	// According to page 46 of Hitachi LCD datasheet
	// Must send  high-half word first three times
	// (times 3 each as per usual):
	//     - RS = 0, RW = 0, 0011
	// Then send RS = 0, RW = 0, 0010 (x3 as usual)
	// only high-half
	// Then send the remaining init instructions
	// (use LCD send command)
	//     00 0010 1000
	//     00 0010 1000
	//     00 0010 1000
	//     00 0010 1000

	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000011));
	SPI_SendByte( LCD_CMD | LCD_EN | ((uint8_t)0b00000011));
	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000011));

	delayMs(5);

	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000011));
	SPI_SendByte( LCD_CMD | LCD_EN | ((uint8_t)0b00000011));
	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000011));

	delayMs(1);

	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000011));
	SPI_SendByte( LCD_CMD | LCD_EN | ((uint8_t)0b00000011));
	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000011));

	delayMs(1);
	
	// sets into 4-bit interface mode
	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000010));
	SPI_SendByte( LCD_CMD | LCD_EN | ((uint8_t)0b00000010));
	SPI_SendByte( LCD_CMD | ((uint8_t)0b00000010));

	delayMs(3);
	
	// Set number of display lines and font
	LCD_SendByte(LCD_CMD, (uint8_t)0b00101000);
	// Turn off display (but don't reset controller
	LCD_SendByte(LCD_CMD, (uint8_t)0b00001100);
	// Clear display
	LCD_SendByte(LCD_CMD, (uint8_t)0b00000110);
	// Enable entry mode
	LCD_SendByte(LCD_CMD, (uint8_t)0b00000001);

	// now ready to write actual data

	// write "F:    Hz" to first row
	LCD_ToDigit(LCD_ROW_F, D0);
	LCD_WriteChar((uint8_t)'F');
	LCD_WriteChar((uint8_t)':');
	LCD_ToDigit(LCD_ROW_F, D6);
	LCD_WriteChar((uint8_t)'H');
	LCD_WriteChar((uint8_t)'z');

	// write "R:    Oh" to first row
	LCD_ToDigit(LCD_ROW_R, D0);
	LCD_WriteChar((uint8_t)'R');
	LCD_WriteChar((uint8_t)':');
	LCD_ToDigit(LCD_ROW_R, D6);
	LCD_WriteChar((uint8_t)'O');
	LCD_WriteChar((uint8_t)'h');

	// initialize TIM3 which controls refreshing the LCD
	myTIM3_Init();
}




// --------------------------------------------------------------------
//                               UTILITY FUNCTIONS
// --------------------------------------------------------------------

uint32_t readADC(void)
{
	// start conversion
	ADC1->CR |= ADC_CR_ADSTART;
	// wait for end of conversion flag
	while (!(ADC1->ISR & ADC_ISR_EOC));
	//reset end of conversion flag
	ADC1->ISR &= ~(ADC_ISR_EOC);
	return (uint32_t)((ADC1->DR) & ADC_DR_DATA);
}

// convert ADC reading to resistance
uint32_t toOhms(uint32_t adc_val)
{
	return (uint32_t)((((float)adc_val)/4096.0) * 5000.0);
}

// convert ADC reading to DAC output voltage and output value
DAC_Data_Typedef toDacVal(uint32_t adc_val)
{
	DAC_Data_Typedef ret; // return value
	
	// proportion of maximum output
	float output_ratio = ((float)adc_val) / ((float)DAC_MAX_VAL);
	
	// map the proportion to the range 0.9V - 2.92V
	float volt_range = DAC_MAX_VOLTS - OPTO_DROP;
	float dac_volts_out = ( (output_ratio * volt_range ) + OPTO_DROP );
	
	// convert the voltage to a DAC value and return both
	ret.value =
		(uint32_t)((dac_volts_out / DAC_MAX_VOLTS)*((float)DAC_MAX_VAL));
	ret.voltage = dac_volts_out;
	
	return ret;
}

// write to DAC
void writeDAC(uint32_t dac_val)
{
	// use 12-bit, right aligned, system default endianness
	DAC->DHR12R1 = dac_val;
}

// delay for a number of milliseconds
// required because of LCD latency
void delayMs(uint16_t ms)
{
	// clear counter and set new period
	// to out desired number of milliseconds
	TIM16->CNT = 0x0;
	TIM16->ARR = ms;

	// Update timer registers
	TIM16->EGR = ((uint16_t)0x0001);

	// Enable update interrupt generation
	TIM16->DIER |= TIM_DIER_UIE;

	//Start counting
	TIM16->CR1 |= TIM_CR1_CEN;

	// wait until its done
	// by checking it the flag is up
	while ( (TIM16->SR & TIM_SR_UIF) == 0 );

	// Stop timer
	TIM16->CR1 &= ~(TIM_CR1_CEN);

	// Clear flag
	TIM16->SR &= ~(TIM_SR_UIF);
}

void SPI_SendByte(uint8_t data)
{
	SPI_lock();

	// wait until SPI is ready
	while ( (SPI1->SR & SPI_SR_BSY) != 0
			&& (SPI1->SR & SPI_SR_TXE) == 0 )
	{
		if (SPI_DEBUG)
			trace_printf("SPI_send: waiting for BSY or TXE to send\n");
	}

	SPI_SendData8(SPI1, data);

	// wait until SPI is not busy
	while ((SPI1->SR & SPI_SR_BSY) != 0)
	{
		if (SPI_DEBUG)
			trace_printf("SPI_send waiting for BSY to unlock\n");
	}

	SPI_unlock();
}

// assert the LCK pin (PB4) low
void SPI_lock(void)
{
	GPIOB->BRR = LCK_PIN;
}

// deassert (set high) the LCK pin (PB4)
void SPI_unlock(void)
{
	GPIOB->BSRR = LCK_PIN;
}

// send one byte of data to the LCD
void LCD_SendByte(uint8_t msg_type, uint8_t msg)
{
	// type: either LCD_CMD or LCD_CHAR
	// msg: the byte to send
	
	// split message into two bytes
	uint8_t high = ((msg & 0xF0) >> 4);
	uint8_t low  = (msg & 0x0F);

	// send high portion in required sequence
	SPI_SendByte(msg_type |          high);
	SPI_SendByte(msg_type | LCD_EN | high);
	SPI_SendByte(msg_type |          high);

	// send low portion in required sequence
	SPI_SendByte(msg_type |          low);
	SPI_SendByte(msg_type | LCD_EN | low);
	SPI_SendByte(msg_type |          low);

	// wait for a bit for LCD to do its work
	delayMs(3);
}

void LCD_Clear(void)
{
	LCD_SendByte(LCD_CMD, LCD_CMD_CLEAR);
}

// move the cursor to spefic location
void LCD_ToDigit(uint8_t row, uint8_t digit)
{
	LCD_SendByte(LCD_CMD, row | digit );
}

// write a character to the display
void LCD_WriteChar(uint8_t c)
{
	LCD_SendByte(LCD_CHAR, c);
}

// write an integer to the screen
void LCD_WriteNum(uint32_t num)
{
	// array for holding char symbols
	// note that in ASCII,
	// a number ( <10 ) x is given by (0x30 + x)
	uint8_t charArray[4];
	// array to hold the digits of the number
	// doesn't need to be bigger,
	// because uint32_t only goes up to ~4 billion (10 digits)
	uint32_t digitArray[10];

	// extract the digits by working backwards
	// through the powers of ten
	// and dividing the number by the power of ten
	uint32_t power;
	uint32_t powVal = 1000000000; // 1 billion
	uint32_t* pDigit = digitArray;

	if(LCD_DEBUG)
		trace_printf("\n-------- Extracting Digits ---------\n");
	
	uint32_t leading_zeros = 0;

	// start at 1 billion (10^9)
	// because that is the biggest power of ten that fits
	// in a uint32_t
	for ( power = 9; power >= 0; power--)
	{
		// we move backwards through powers,
		// but start storing at the start of the array
		
		// take the modulus to get only the
		// last digit of the result
		*pDigit = (num / powVal) % 10;
	
		// track leading zeros
		if (*pDigit == 0 ) leading_zeros++;

		if(LCD_DEBUG)
			trace_printf("\t>>\t%d\n", *pDigit);

		if (power == 0) break; // protect against overrun
		
		// divide divisor by ten
		powVal /= 10;
		// increment storing location
		pDigit++;
	}

	// for now: just print the last four digits
	// because both our resistance and frequency
	// are known to only go that high
	unsigned int i;

	if(LCD_DEBUG)
		trace_printf("\n-------- Processing Digits ---------\n");

	uint8_t leading_zero_flag = 0;

	// store the last four digits in the character array
	// replace leading zeros with a space
	for (i = 6; i < 10; i++)
	{
		charArray[i - 6] = (uint8_t)(0x30 + digitArray[i]);

		if (charArray[i - 6] != (uint8_t)0x30 )
			leading_zero_flag = 1;

		if ( (leading_zero_flag == 0)
				&& (i != 9)
				&& (charArray[i - 6] == (uint8_t)0x30))
		{
			// if its a leading 0, make it a space
			charArray[ i - 6] = (uint8_t)0x20;
		}

		if(LCD_DEBUG)
			trace_printf("\t>>\t%d\n", charArray[i-6]);
	}

	// TODO: figure out how to display
	// all the numbers larger than 9999
	// ...


	// Write to the display
	for (i = 0; i < 4; i++)
	{
		LCD_WriteChar(charArray[i]);
	}

}

// write the frequency to the LCD
void LCD_WriteFreq(uint32_t freq)
{
	// move to the position just after "F:"
	LCD_ToDigit(LCD_ROW_F, D2);
	// write the frequency
	LCD_WriteNum(freq);
}

// write the resistance to the LCD
void LCD_WriteOhms(uint32_t ohms)
{
	// move to the position just after "R:"
	LCD_ToDigit(LCD_ROW_R, D2);
	// Write the resistance
	LCD_WriteNum(ohms);
}



// --------------------------------------------------------------------
//                                  INTERRUPTS
// --------------------------------------------------------------------

void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		if (TIM2_DEBUG)
			trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		TIM2->SR &= ~(TIM_SR_UIF);

		/* Restart stopped timer */
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}

// Update the LCD
void TIM3_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM3->SR & TIM_SR_UIF) != 0)
	{
		// Update the LCD here
		if (TIM3_DEBUG)
		{
			trace_printf("TIM3 IRQ Handler!\n");
		}
		
		// write the current frequency and resistances
		// to the LCD
		lcd.write(&lcd,
				(uint32_t)round(pwm.data.frequency),
				(uint32_t)pot.data.resistance);


		/* Clear update interrupt flag */
		TIM3->SR &= ~(TIM_SR_UIF);

		/* Restart stopped timer */
		TIM3->CR1 |= TIM_CR1_CEN;
	}
}

// Calculate the frequency of the PWM signal
void EXTI0_1_IRQHandler()
{
	double period = 0.0;
	double freq = 0.0;
	uint32_t count = 0;

	/* Check if EXTI1 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR1) != 0)
	{
		// if this is the first edge (start of the period)
		// increment the pwm edge counter and exit
		if (pwm.edge == FIRST_EDGE)
		{
			TIM2->CNT = 0x0;
			TIM2->CR1 |= TIM_CR1_CEN; /* Restart stopped timer */
			pwm.edge = SECOND_EDGE;
		}
		// if this is the second edge,
		// calculate the period and frequency
		else if (pwm.edge == SECOND_EDGE)
		{
			TIM2->CR1 &= ~(TIM_CR1_CEN);
			//Stop timer
			//Get the period in ticks
			count = TIM2->CNT;
			// Calculate period
			period = ((double)count) / ((double)SystemCoreClock);
			freq = ((double)1.0) / period;

			// store the calculated data in the pwm struct
			pwm.data.frequency = freq;
			pwm.data.period = period;

			//Reset edges to 0
			pwm.edge = FIRST_EDGE;

			if (EXTI_DEBUG)
			{
				trace_printf("=========== Signal ============\n\n");
				trace_printf(">>\t");
				trace_printf("Signal period:\t\t%f s\n", period);
				trace_printf(">>\t");
				trace_printf("Signal frequency:\t%f Ohms\n", freq);
				trace_printf("\n");
			}
		}
		EXTI->PR |= ((uint32_t)0x0002);
	}
}


#pragma GCC diagnostic pop

// --------------------------------------------------------------------
