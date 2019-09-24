/*! \file       settings.c
    \version    0.1  
    \date       2017-06-08 22:02
    \brief      An interface between an App layer and Conf layer 
    \bug       
    \copyright  
    \author    

    \details    The number of functions provide an access interface to settings
                for applications. Also it is wrap for conreete hardware implementation for 
                saving app setings.
                Setting before using should be read into <setting> structure which kepts actual values.


  TODO: Add setting interface described through DEFINES

  AOT - Action On Timer
  AOS - Action on Sensor

*/
/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>
#include "eventq.h"
#include "apptypes.h"
#include "settings.h"
#include "appwatering.h"
#include "appfwk.h"
#include "main.h"


/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                    Typedefs
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                 Global Variables
 * ------------------------------------------------------------------------------------------------
 */
tSettings settings;     /**< Settings profile */

/* ------------------------------------------------------------------------------------------------
 *                                 Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                    Functions
 * ------------------------------------------------------------------------------------------------
 */
uint32_t calculateCrcEx( uint8_t* pBuff, uint32_t size );

/**
 * \fn     
 * \brief  
 * \param  
 * \return 
 */
void flushSettings( void )
{
  /* calculate CRC */
  settings.crc = calculateCrcEx(( uint8_t* )&settings, sizeof( settings )-sizeof( settings.crc ));
  
  hwkWriteBufferToEeprom( 0,( uint8_t* )&settings, sizeof( tSettings ));
}

tSettings* getSettings( void )
{
  return &settings;
}

void loadSettings( void )
{
	uint32_t tmp;
  
  hwkReadBufferFromEeprom( 0,( uint8_t* )&settings, sizeof( tSettings ));
	
  /* calculate CRC */
	tmp = calculateCrcEx(( uint8_t* )&settings, sizeof( settings )-sizeof( settings.crc ));
  
	if( settings.crc == tmp )
		return;
	
  assing_default_settings();
}

void assing_default_settings( void )
{
	settings.secondsToSleep     = TIME_TO_SLEEP_DEF_VAL;
  settings.blUsePump          = PUMP_IS_ON_DEF_VAL;
  settings.minutesWateringGap = 55;
  settings.secondsWatering    = WTR_WATERING_TIME;
  settings.blDisplayOnMode    = 0x01;
  settings.mode               = MODE_AOS;
//  settings.uiStartSensorThreshold = (uint8_t) mapDigitalValue( WTR_WATERING_START_SHD, 0x0fff, 0x0000, 10 );
//  settings.uiStopSensorThreshold  = (uint8_t) mapDigitalValue( WTR_WATERING_STOP_SHD, 0x0fff, 0x0000, 10 );
  settings.uiStartSensorThreshold = (uint8_t) mapDigitalValue( WTR_WATERING_START_SHD, 3500, 400, 10 );
  settings.uiStopSensorThreshold  = (uint8_t) mapDigitalValue( WTR_WATERING_STOP_SHD, 3500, 400, 10 );

  settings.crc = calculateCrcEx(( uint8_t* )&settings, sizeof( settings )-sizeof( settings.crc ));
}

uint32_t calculateCrcEx( uint8_t* pBuff, uint32_t size )
{
  uint32_t tmpCRC = 0;
  
  for( uint32_t i=0; i<size; i++)
    tmpCRC += pBuff[ i ];
  
  return tmpCRC;
}
  

  
