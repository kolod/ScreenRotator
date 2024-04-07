#pragma once

#define DBGLOG_LEVEL_TRACE 5
#define DBGLOG_LEVEL_DEBUG 4
#define DBGLOG_LEVEL_WARNING 3
#define DBGLOG_LEVEL_INFO 2
#define DBGLOG_LEVEL_ERROR 1
#define DBGLOG_LEVEL_CRITICAL 0

#ifndef DBGLOG_LEVEL
#define DBGLOG_LEVEL DBGLOG_LEVEL_INFO
#endif

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_TRACE
#define DBGLOG_TRACE(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define DBGLOG_TRACE(format, ...)
#endif

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG
#define DBGLOG_DEBUG(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define DBGLOG_DEBUG(format, ...)
#endif

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_WARNING
#define DBGLOG_WARNING(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define DBGLOG_WARNING(format, ...)
#endif

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_INFO
#define DBGLOG_INFO(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define DBGLOG_INFO(format, ...)
#endif

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_ERROR
#define DBGLOG_ERROR(format, ...) Serial.printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define DBGLOG_ERROR(format, ...)
#endif

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_CRITICAL
#define DBGLOG_CRITICAL(format, ...) \
	do { \
		Serial.printf_P(PSTR(format), ##__VA_ARGS__); \
		for (;;); \
	} while (0)
#else
#define DBGLOG_CRITICAL(format, ...)
#endif
