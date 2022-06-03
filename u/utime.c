#include <time.h>
#include <stdio.h>
#include "utime.h"

#define UTIME_BUFF_SIZE 100

static char utime_buff[UTIME_BUFF_SIZE];

char* utimestr()
{
  time_t t;
  struct tm* tp;

  if (time(&t) == -1) {
    return NULL;
  }
  tp = localtime(&t);
  snprintf(utime_buff,
	   UTIME_BUFF_SIZE,
	   "%d-%d-%d %d:%d:%d",
	   1900 + tp->tm_year,
	   1 + tp->tm_mon,
	   tp->tm_mday,
	   tp->tm_hour,
	   tp->tm_min,
	   tp->tm_sec);
  return utime_buff;
}
