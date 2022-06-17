#include <am.h>
#include <nemu.h>
//#include <sys/time.h>
//#include <time.h>
//#include <unistd.h>

//static struct timeval boot_time={};

void __am_timer_init() {
 //  gettimeofday(&boot_time,NULL);

}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  /*	
  uptime->us =inl(RTC_ADDR);
  int d=inl(RTC_ADDR+4);
  uptime->us+=d;
 */
  /*fake
  long useconds=inl(RTC_ADDR);
  long seconds=inl(RTC_ADDR+4);
  useconds-=800000;
  seconds-=1640000000;
  uptime->us=seconds*1000000+(useconds+500);

  */
 /*bootime in this file
  
  long useconds=inl(RTC_ADDR)-boot_time.tv_usec;
  long seconds=inl(RTC_ADDR)-boot_time.tv_sec;
  uptime->us=seconds*1000000+(useconds+500);
  */
  uptime->us=inl(RTC_ADDR);

}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
