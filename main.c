/*

PB6  -> RST
PB10 -> CLK
PB11 -> DAT


 */
#include "stm32f10x.h"
#include "stdio.h"

#define SEC             0x40
#define MIN             0x41
#define HR              0x42
#define DATE            0x43
#define MONTH           0x44
#define DAY             0x45
#define YEAR            0x46
#define CONTROL         0x47
#define TRACKLE_CHARGER 0x48
#define CLOCK_BURST     0x5F
#define RAM0            0x60
#define RAM1            0x61
#define RAM30           0x7E
#define RAM_BURST       0x7F

#define RST_HIGH      GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define RST_LOW       GPIO_ResetBits(GPIOB, GPIO_Pin_6)     

#define CLK_HIGH      GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define CLK_LOW       GPIO_ResetBits(GPIOB, GPIO_Pin_10)

#define DAT_HIGH      GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define DAT_LOW       GPIO_ResetBits(GPIOB, GPIO_Pin_11)

#define DAT           GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)

void delay(int i) {
	while (i) {
		i--;
	}
}

int fputc(int ch, FILE *f) {
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
	USART_SendData(USART1, ch);
	return ch;
}

void USART1_Init() {
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// GPIOA.9 USART1_TX
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// GPIOA.10 USART1_RX
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate            = 115200;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void DS1302_Init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	GPIO_ResetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_10 | GPIO_Pin_11);
}

void DS1302_Out() {
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

void DS1302_In() {
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}

void DS1302_WriteByte(u8 value) {
	u8 i;
	DS1302_Out();
	for (i=0;i<8;i++){
		CLK_LOW;
		if (value&0x01) {
			DAT_HIGH;
		} else {
			DAT_LOW;
		}
		delay(1);
		CLK_HIGH;
		delay(1);		
		value >>= 1;
	}
}

void DS1302_ReadByte(u8 *value) {
	u8 i;
	DS1302_In();
	for (i=0;i<8;i++) {
		*value >>= 1;
		CLK_HIGH;
		delay(1);
		CLK_LOW;
		delay(1);
		if (DAT == 1) {
			*value |= 0x80;
		} else {
			*value &= 0x7F;
		}
	}
}

void DS1302_ReadReg(u8 addr, u8 *value) {
	CLK_LOW;
	RST_HIGH;
	DS1302_WriteByte((addr<<1)|0x01);
	DS1302_ReadByte(value);
	RST_LOW;
}

void DS1302_WriteReg(u8 addr, u8 value) {
	CLK_LOW;
	RST_HIGH;
	DS1302_WriteByte((addr<<1)&0xfe);
	DS1302_WriteByte(value);
	CLK_LOW;
	RST_LOW;
}

int main(void) {
	u8 value = 0;
	USART1_Init();
	DS1302_Init();
	printf("start...\n");
	//DS1302_WriteReg(SEC, 0x80);
	//DS1302_WriteReg(YEAR, 0x20);	
	//DS1302_WriteReg(MONTH, 0x11);
	//DS1302_WriteReg(DATE, 0x20);
	//DS1302_WriteReg(HR, 0x15);
	//DS1302_WriteReg(MIN, 0x23);
	//DS1302_WriteReg(SEC, 0x00);
	
	while(1){
		DS1302_ReadReg(YEAR, &value);
		printf("20%d%d-", (value&0xf0)>>4, value&0x0f);
		
		DS1302_ReadReg(MONTH, &value);
		printf("%d%d-", (value&0x10)>>4, value&0x0f);
		
		DS1302_ReadReg(DATE, &value);
		printf("%d%d ", (value&0x30)>>4, value&0x0f);

		DS1302_ReadReg(HR, &value);
		printf("%d%d:", (value&0x10)>>4, value&0x0f);

		DS1302_ReadReg(MIN, &value);		
		printf("%d%d:", (value&0x70)>>4, value&0x0f);
		
		DS1302_ReadReg(SEC, &value);
		printf("%d%d\n", (value&0x70)>>4, value&0x0f);

		delay(50000000);
	}	
}
