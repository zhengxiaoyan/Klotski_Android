#ifndef DEFINE_H_
#define DEFINE_H_

#include <stdarg.h>
#include <stdio.h>
#include <android/log.h>

typedef enum BOOL {
	FALSE = 0,
	TRUE
}BOOL;

#ifndef NULL
#define NULL	(void *)0
#endif

#define TAG	"klotski_jni"

#define _LOG	TRUE
#ifdef _LOG
#define DEBUG(fmt, ...)	do{__android_log_print(ANDROID_LOG_DEBUG, TAG, "[%s()]:" fmt,__func__,##__VA_ARGS__);}while(0)
#define WARN(fmt, ...)	do{__android_log_print(ANDROID_LOG_WARN, TAG, "[%s()]:" fmt,__func__,##__VA_ARGS__);}while(0)
#define ERROR(fmt, ...)	do{__android_log_print(ANDROID_LOG_ERROR, TAG, "[%s()]:" fmt,__func__,##__VA_ARGS__);}while(0)
#else
#define DEBUG(fmt, ...)
#define WARN(fmt, ...)
#define ERROR(fmt, ...)
#endif

#define DRAW	TRUE
#define DRAWC(chr) do{if(DRAW)fprintf(stderr,"%c", (char)chr + 0x30);}while(0)
#define DRAWL do{if(DRAW)fprintf(stderr,"|");}while(0)
#define DRAWU do{if(DRAW)fprintf(stderr,"\n---------\n");}while(0)
#define YIELD(str) do{if(DRAW){fprintf(stderr, "\n" str "\n");}}while(0)

#endif /* DEFINE_H_ */
