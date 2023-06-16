#include <am.h>
#include <nemu.h>

static uint32_t boot_time_usec;
static uint32_t boot_time_sec;

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
	uint32_t now_usec = inl(RTC_ADDR);
	uint32_t now_sec = inl(RTC_ADDR + sizeof(uint32_t));

  long seconds = now_sec - boot_time_sec;
  long useconds = now_usec - boot_time_usec;
  uptime->us = seconds * 1000000 + (useconds + 500);
}

void __am_timer_init() {
	boot_time_usec = inl(RTC_ADDR);
	boot_time_sec = inl(RTC_ADDR + sizeof(uint32_t));
}
