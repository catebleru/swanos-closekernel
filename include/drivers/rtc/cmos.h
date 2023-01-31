#ifndef _CMOS_H
#define _CMOS_H

#define CURRENT_YEAR 2023
#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

#include <stdint.h>

uint8_t cmos_get_second();
uint8_t cmos_get_minute();
uint8_t cmos_get_hour();
uint8_t cmos_get_day();
uint8_t cmos_get_month();
uint32_t cmos_get_year();

#endif
