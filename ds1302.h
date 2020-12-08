#ifndef _DS1302_H
#define _DS1302_H

#include "stm32f10x.h"

void DS1302_Init(void);
void DS1302_ReadReg(u8 addr, u8 *value);
void DS1302_GetYear(u8 *year);
void DS1302_GetMonth(u8 *month);
void DS1302_GetDate(u8 *date);
void DS1302_GetHour(u8 *hour);
void DS1302_GetMinite(u8 *minute);
void DS1302_GetSecond(u8 *second);
void DS1302_SetTime(u8 yr, u8 mon, u8 date, u8 hr, u8 min, u8 sec);

#endif
