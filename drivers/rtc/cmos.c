#include <arch/x86/ports.h>
#include <drivers/rtc/cmos.h>

int32_t century_register = 0x00;

uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t day;
uint8_t month;
uint32_t year;
uint8_t century;

uint8_t last_second;
uint8_t last_minute;
uint8_t last_hour;
uint8_t last_day;
uint8_t last_month;
uint32_t last_year;
uint8_t last_century;
uint8_t registerB;

int get_update_in_progress_flag(){
	outb(0x0A, CMOS_ADDRESS);
	return (inb(CMOS_DATA) & 0x80);
}

uint8_t get_RTC_register(int reg){
	outb(reg, CMOS_ADDRESS);
	return inb(CMOS_DATA);
}

void read_rtc(){
	while (get_update_in_progress_flag());
	second = get_RTC_register(0x00);
	minute = get_RTC_register(0x02);
	hour = get_RTC_register(0x04);
	day = get_RTC_register(0x07);
	month = get_RTC_register(0x08);
	year = get_RTC_register(0x09);
	if(century_register != 0)
		century = get_RTC_register(century_register);
	
	do {
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;
		last_century = century;

		while (get_update_in_progress_flag());
		second = get_RTC_register(0x00);
        	minute = get_RTC_register(0x02);
        	hour = get_RTC_register(0x04);
        	day = get_RTC_register(0x07);
        	month = get_RTC_register(0x08);
        	year = get_RTC_register(0x09);
        	if(century_register != 0)
                	century = get_RTC_register(century_register);

	} while((last_second != second) || (last_minute != minute)
		|| (last_hour != hour) || (last_day != day) ||
		(last_month != month) || (last_year != year) ||
	     	(last_century != century));

	registerB = get_RTC_register(0x0B);

	if (!(registerB & 0x04)){
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
		if(century_register != 0){
			century = (century & 0x0F) + ((century / 16) * 10);
		}	
	}

	if(!(registerB & 0x02) && (hour & 0x80)){
		hour = ((hour & 0x7F) + 12) & 24;
	}

	if(century_register != 0){
		year += century * 100;
	} else {
		year += (CURRENT_YEAR / 100) * 100;
		if(year < CURRENT_YEAR) year += 100;
	}
}

uint8_t cmos_get_second(){
	read_rtc();
	return second;
}

uint8_t cmos_get_minute(){
	read_rtc();
	return minute;
}

uint8_t cmos_get_hour(){
	read_rtc();
	return hour;
}

uint8_t cmos_get_day(){
	read_rtc();
	return day;
}

uint8_t cmos_get_month(){
	read_rtc();
	return month;
}

uint32_t cmos_get_year(){
	read_rtc();
	return year;
}
