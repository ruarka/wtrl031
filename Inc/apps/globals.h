/*! \file      globals.h
    \version   0.1
    \date      2018-04-16 17:54
    \brief     <A Documented file.>
    \details   <Details.>
    \bug       
    \copyright  
    \author    
*/
/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
/*
 * Type for custom logging functions
 */
typedef void (*logger_p)(int entity_level, const int level, const char * format, va_list ap);


/*!
 Logging macros and associated log levels.
 The current log level is kept in entity_level.
 */
enum {
	LOG_NONE = 0,
	LOG_OUTPUT,
	LOG_ERROR,
	LOG_WARNING,
	LOG_TRACE,
	LOG_DEBUG,
};

#ifdef  DBG_STREAM

#define BAR(fmt, ...)  printf(fmt, ##__VA_ARGS__)

/*
#define LOG(entity_level, level, format, ...) \
	do { \
			if( entity_level >= level ) \
			{ \
					printf( format, __VA_ARGS__ ); \
			} \
	} while(0)

*/

// #ifdef TIME_FUNC
// #define LOGTIME() printf( "\n%d %02d:%02d:%02d ",  sDateStructureGet.WeekDay, sTimeStructureGet.Hours, sTimeStructureGet.Minutes, sTimeStructureGet.Seconds )

// uint8_t 	hours;
// uint8_t 	minutes;
// uint8_t 	seconds;

#if 1
#define LOGTIME() printf( "\r\n%d %02d:%02d:%02d ",  0, fwkSystemTime.hours, fwkSystemTime.minutes, fwkSystemTime.seconds )
#else
#define LOGTIME() printf( "\r\n0 00:00:00 " )
#endif

#define LOG(entity_level, level, format, ...) \
	do { \
			if( entity_level >= level ) \
			{ \
					printf( format, ##__VA_ARGS__  ); \
			} \
	} while(0)


#define LOGT(entity_level, level, format, ...) \
		LOGTIME();  \
		LOG(entity_level, level,  format, ##__VA_ARGS__)

#define TRACE(entity_level, format, ...) \
  LOG(entity_level, LOG_TRACE,  format, ##__VA_ARGS__)

#define TRACET(entity_level, format, ...) \
	LOGTIME(); \
  LOG(entity_level, LOG_TRACE,  format, ##__VA_ARGS__)

#define WRN(entity_level,  format, ... ) \
	LOG(entity_level, LOG_WARNING,  format, ##__VA_ARGS__)

#define WRNT(entity_level,  format, ... ) \
	LOGTIME(); \
	LOG(entity_level, LOG_WARNING,  format, ##__VA_ARGS__)


#define DBG(entity_level, format, ... ) \
	LOG(entity_level, LOG_DEBUG, format, ##__VA_ARGS__)

#define DBGT(entity_level, format, ... ) \
	LOGTIME(); \
	LOG(entity_level, LOG_DEBUG, format, ##__VA_ARGS__)

#else

#define LOG(entity_level, level, ...)
#define LOGT(entity_level, level, format, ...)
#define TRACE(entity_level, ... )
#define TRACET(entity_level, format, ...)
#define WRN(entity_level, ...)
#define WRNT(entity_level,  format, ... )
#define DBG(entity_level, ... )
#define DBGT(entity_level, format, ... ) 

#define LOGTIME()

#endif


 /* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */
 
 /* ------------------------------------------------------------------------------------------------
 *                                    Functions declarations
 * ------------------------------------------------------------------------------------------------
 */
extern char pDisplayStrBuff[ 17 ];

extern const char* const weekDayStrs[]; 

extern const uint8_t weekdayRu[];

extern const char pIntToHex[];

extern const char pTimeFormatStr[];

extern void global_logger( int entity_level, const int level, const char * format, ... );

extern uint32_t mapDigitalValue( uint32_t val, uint32_t biggestVal, uint32_t lowestVal, uint32_t scalePoints );
