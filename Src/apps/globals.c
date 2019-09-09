/*! \file      globals.c
    \version   0.1  
    \date      2018-04-16 17:54
    \brief     Contents application layer GLOBAL variables and constants
    \author    ruarka
		
	\bug
				
	\details
*/
/* ------------------------------------------------------------------------------------------------
 *                                 Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>
#include <stdio.h>
#include "globals.h"
 
/* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */
 
/* ------------------------------------------------------------------------------------------------
 *                                 Global Variables
 * ------------------------------------------------------------------------------------------------
 */
/*! \brief week days acronims 
 *  \details week day number starts from 1. 
 *           0 fake index
 */
const char* const weekDayStrs[] =
{
  "NN",
  "Mn",
  "Tu",
  "We",
  "Th",
  "Fr",
  "Sa",
  "Sn"
};

/* Stirng Buffer for display */
char pDisplayStrBuff[ 17 ];

/*! Int into HEX conversion support variable */
const char pIntToHex[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

/*! English to Russaian week days indexes translation */
const uint8_t weekdayRu[] = { 7, 1, 2, 3, 4, 5, 6 };

/*! Time format string used for Debug purposes */
const char pTimeFormatStr[]="\n0 00:00:00 ";

/*
static void
std_logger(
		int entity_level,
		const int level,
		const char * format,
		va_list ap);
static logger_p _global_logger = std_logger;

void
global_logger(
		int entity_level,
		const int level,
		const char * format,
		... )
{
	va_list args;
	va_start(args, format);
	if (_global_logger)
		_global_logger( entity_level, level, format, args);
	va_end(args);
}

void
global_logger_set(
		logger_p logger)
{
	_global_logger = logger ? logger : std_logger;
}

logger_p
global_logger_get(void)
{
	return _global_logger;
}

static void
std_logger(
		int entity_level,
		const int level,
		const char * format,
		va_list ap)
{
	if (entity_level >= level) {
		printf( format, ap );
	}
}
*/


/**
 * \fn     uint32_t mapDigitalValue( uint32_t val, uint32_t biggestVal, uint32_t lowestVal, uint32_t scalePoints )

 * \brief
 * \param
 * \return
 */
uint32_t mapDigitalValue( uint32_t val, uint32_t biggestVal, uint32_t lowestVal, uint32_t scalePoints )
{
  if( lowestVal >= val )
    return 0;

  if( val >= biggestVal )
    return scalePoints-1;

  uint32_t interval =( biggestVal-lowestVal )/scalePoints;

  uint32_t normalizedVal =( val-lowestVal )/interval;

  return normalizedVal;
}
