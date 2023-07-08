#include <common.h>
#include <sys/time.h>
#include "syscall.h"
#include <fs.h>

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
	uintptr_t *ret = &c->GPRx;

  switch (a[0]) {
		case SYS_brk: {
				*ret = 0;
				break;
		}
		case SYS_open: *ret = fs_open((const char *)a[1], a[2], a[3]); break;
		case SYS_lseek: *ret = fs_lseek(a[1], a[2], a[3]); break;
		case SYS_read: *ret = fs_read(a[1], (void *)a[2], a[3]); break;
		case SYS_write: *ret = fs_write(a[1], (const void *)a[2], a[3]); break;
		case SYS_close: *ret = fs_close(a[1]); break;
		case SYS_gettimeofday: {
			struct timeval *tv = (struct timeval *)a[1];
			AM_TIMER_RTC_T rtc = io_read(AM_TIMER_RTC);
			tv->tv_sec = rtc.rtc_sec;
			tv->tv_usec = rtc.rtc_usec;
			*ret = 0; 
			break;
		}
		case SYS_yield: yield(); break;	
		case SYS_exit: halt(a[1]); break;	
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
