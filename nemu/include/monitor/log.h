#ifndef __MONITOR_LOG_H__
#define __MONITOR_LOG_H__

#include <common.h>

#ifdef DEBUG
extern FILE* log_fp;
#	define log_write(fp, ...) \
  do { \
    extern bool log_enable(); \
    if (fp != NULL && log_enable()) { \
      fprintf(fp, __VA_ARGS__); \
      fflush(fp); \
    } \
  } while (0)
#else
#	define log_write(...)
#endif

#define _Log(...) \
  do { \
    printf(__VA_ARGS__); \
    log_write(log_fp, __VA_ARGS__); \
  } while (0)

void strcatf(char *buf, const char *fmt, ...);

#endif
