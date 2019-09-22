/*! \file       appfwk.c
    \version    0.1
    \date       2017-05-30 22:28
    \author     ruarka
    
    \brief      File contains application layer framework functions.
    
    @verbatim 
    Sleeping - A Sleeping application is built into FWK. When sleeping condition is reached something
               like 20 seconds without any activity Sleeping application sends a EV_APP_SLEEP in 
               fwkSleepSecTicksEvHandler(). After that Sleeping applications waits this event in 
               fwkSleepSleepEvHandler() and if it comes (so all others APPs are agreed to come to 
               SLEEP) then initiates sleeping process.

               fwkSleepSleepEvHandler() should be the last in EV_APP_SLEEP handling sequence 
               pEvAppSleepHandlersList[] to achieve all apps is asked for sleeping.
       
    Key Pressing Application detector (KPA) - 
              KPA is built into FWK.  
              
    System Time -
              is used to roughly calculale time gaps. Roughly means that clock freq
              does not have enougth precision against RTC with crystal oscilator              
    
              fwkSystemTime keeps time 
               
    
    @endverbatim
    
    \bug
 */
/* ------------------------------------------------------------------------------------------------
*                                 Includes
* ------------------------------------------------------------------------------------------------
*/
#include <stdint.h>

#include "apptypes.h"

#include "hal.h"
#include "events.h"
#include "eventq.h"
#include "appwatering.h"
#include "fsmappl.h"
#include "settings.h"
#include "appfwk.h"
#include "main.h"


/* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */
uint8_t fwkDateTimeEvHandler(_tEQ* p);

uint8_t fwkSleepSleepEvHandler(_tEQ* p);

uint8_t fwkSleepIdleEvHandler(_tEQ* p);

uint8_t fwkSleepSecTicksEvHandler(_tEQ* p);

uint8_t fwkLedsProcessing(_tEQ* p);

uint8_t fwkSysTickEvHandler(_tEQ* p);

uint8_t fwkSleepInitEvHandler(_tEQ* p);

/* ------------------------------------------------------------------------------------------------
*                                     Macros
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
*                                 Global Variables
* ------------------------------------------------------------------------------------------------
*/
_tSysTime fwkSystemTime;

uint32_t timeToSleep;

tApplicationContext applContext;


/* ------------------------------------------------------------------------------------------------
*                                 Local Variables
* ------------------------------------------------------------------------------------------------
*/
/*! For each event the list of app handlers should be defined. */
/*! End of the list should be marked by 0 */
/*! Idle application handlers list */
pfnAppEventHandler const pEvAppIdleHandlersList[]       = { appMenuEvHandler, fwkSleepIdleEvHandler, 0 };

/*! Init application handlers list */
pfnAppEventHandler const pEvAppInitHandlersList[]       = { appMenuEvHandler, appWateringEvHandler, fwkSleepInitEvHandler, 0 };

/*! Check sleeping application handlers list */ 
pfnAppEventHandler const pEvAppSleepHandlersList[]      = { appMenuEvHandler, appWateringEvHandler, fwkSleepSleepEvHandler, 0 };

/*! Seconds Ticks application handlers list */ 
pfnAppEventHandler const pEvAppSecTickHandlersList[]    = { fwkDateTimeEvHandler, appMenuEvHandler, appWateringEvHandler, fwkSleepSecTicksEvHandler, 0 };

/*! UP key pressed application handlers list */ 
pfnAppEventHandler const pEvAppUpHandlersList[]         = { appMenuEvHandler, appWateringEvHandler, 0 };

/*! DOWN key pressec application handlers list */ 
pfnAppEventHandler const pEvAppDownHandlersList[]       = { appMenuEvHandler, appWateringEvHandler, 0 };

/*! Left key pressed application handlers list */
pfnAppEventHandler const pEvAppLeftHandlersList[]       = { appMenuEvHandler, appWateringEvHandler, 0 };

/*! Right key pressed application handlers list */ 
pfnAppEventHandler const pEvAppRightHandlersList[]      = { appMenuEvHandler, appWateringEvHandler, 0 };

/*! Star key pressed application handlers list */ 
pfnAppEventHandler const pEvAppBtnLeftHandlersList[]       = { appMenuEvHandler, appWateringEvHandler, 0 };
//pfnAppEventHandler const pEvAppBtnPressedHandlersList[]    = { appMenuEvHandler, appWateringEvHandler, 0 };


/*! Adc scan event to detect key pressed application handlers list */
pfnAppEventHandler const pEvAppAdcKbPresedHandlersList[]= { appWateringEvHandler, 0 };

/*! Sytem Ticks application handlers list */
// pfnAppEventHandler const pEvSysTickHandlersList[]       = { fwkSysTickEvHandler, fwkLedsProcessing, 0 };
pfnAppEventHandler const pEvSysTickHandlersList[]       = { fwkSysTickEvHandler, 0 };

/*! Suspend application handlers list */
pfnAppEventHandler const pEvSuspendHandlersList[]       = { appWateringEvHandler, 0 };

/*! Resume application handlers list */
pfnAppEventHandler const pEvResumeHandlersList[]       = { appWateringEvHandler, 0 };

/*! Suspend application handlers list */
pfnAppEventHandler const pEvDropdHandlersList[]       = { appWateringEvHandler, 0 };

/*! List of application handlers list */
_tAppEventLink const eventHandlersList[] = {
    { EV_SYS_TICK,        pEvSysTickHandlersList },
    { EV_APP_SEC_TICK,    pEvAppSecTickHandlersList },
    { EV_ADC_SCAN,   		  pEvAppAdcKbPresedHandlersList },
    { EV_APP_IDLE,        pEvAppIdleHandlersList },
    { EV_APP_INIT,        pEvAppInitHandlersList },
    { EV_APP_SLEEP,       pEvAppSleepHandlersList },
    { EV_APP_UP,          pEvAppUpHandlersList },
    { EV_APP_DOWN,        pEvAppDownHandlersList },
    { EV_APP_LEFT,        pEvAppLeftHandlersList },
    { EV_APP_RIGHT,       pEvAppRightHandlersList },
    { EV_APP_BTN_ACT,     pEvAppBtnLeftHandlersList },
    { EV_PARK_ACT,        pEvSuspendHandlersList },
    { EV_DROP_ACT,        pEvDropdHandlersList},
    { EV_RSM_ACT,         pEvResumeHandlersList }
};

/*!  Display abstraction for application lavel variables */
tDisplayEntity dsplDataScreen[DISPLAY_LINES_NUMBER][DISPLAY_COLUMNS_NUMBER];

/* ------------------------------------------------------------------------------------------------
 *                                          Functions
 * ------------------------------------------------------------------------------------------------
 */

// =========================================================================
// LED Blinking support
// =========================================================================
typedef struct _LED_Entity
{
  uint8_t  ledId;
  uint8_t  OnNum;
  uint8_t  OffNum;
  uint8_t  blPeriodic;
  uint8_t  OnTicks;
  uint8_t  OffTicks;
  uint8_t  OnOffState;
} _tLed;

enum {
  OnTime = 0,
  OffTime,
  Block
};


_tLed LEDS[ APP_LEDS_NUMBER ];

void fwkLedSet( uint8_t uiLedId, uint8_t ontime, uint8_t offTime, uint8_t blPeriodic )
{
  LEDS[uiLedId].ledId       = uiLedId;
  LEDS[uiLedId].OnNum       = ontime;
  LEDS[uiLedId].OffNum      = offTime;
  LEDS[uiLedId].blPeriodic  = blPeriodic;
  LEDS[uiLedId].OnTicks     = 1;
  LEDS[uiLedId].OffTicks    = 1;
  

  if( LEDS[ uiLedId ].OnNum && LEDS[ uiLedId ].OffNum )
      setLedPinState( uiLedId, 1 );

  if( !LEDS[ uiLedId ].OnNum && LEDS[ uiLedId ].OffNum )
      setLedPinState( uiLedId, 0 );

  if( LEDS[ uiLedId ].OnNum && !LEDS[ uiLedId ].OffNum )
      setLedPinState( uiLedId, 1 );
}

void fwkLedOn( uint8_t uiLedId )
{
  fwkLedSet( uiLedId, 10, 0, 1 );
}

void fwkLedOff( uint8_t uiLedId )
{
  fwkLedSet( uiLedId, 0, 10, 1 );
}

void fwkLedBlink( uint8_t uiLedId, uint8_t ontime, uint8_t offTime )
{
  fwkLedSet( uiLedId, ontime, offTime, 1 );
}

uint8_t fwkLedsProcessing(_tEQ* p)
{
  for(uint8_t i=0; i<APP_LEDS_NUMBER; i++ )
  {
      if( LEDS[ i ].OnNum && LEDS[ i ].OffNum )
      {
          switch (LEDS[ i ].OnOffState)
          {
            case OnTime:
              if( LEDS[ i ].OnTicks >= LEDS[ i ].OnNum )
              {
                  setLedPinState( LEDS[ i ].ledId, 0 );
                  LEDS[ i ].OffTicks = 1;
                  LEDS[ i ].OnOffState = OffTime;
              }
              else
                LEDS[ i ].OnTicks++;
              break;

            case OffTime:
              if( LEDS[ i ].OffTicks >= LEDS[ i ].OffNum )
              {
                  if( LEDS[ i ].blPeriodic )
                  {
                    setLedPinState( LEDS[ i ].ledId, 1 );
                    LEDS[ i ].OnTicks     = 1;
                    LEDS[ i ].OnOffState  = OnTime;
                  }
                  else
                    LEDS[ i ].OnOffState = Block;
              }
              else
                LEDS[ i ].OffTicks++;
              break;

            case Block:
              break;
          }
      }
  }
  return 0;
}
// =========================================================================
// Application support functions
// =========================================================================
/**
 * \fn     void fwkSendEventToAppDirectly( uint8_t appId, _tEQ* pEv )
 * \brief  Sends an Event to appropriate Appl. It uses to send commands directly
 *         between Applications
 * \param  uitn8 aapID Application number in event handlers list
 * \param  _tEQ* pEv An Event to send
 * \return None
 */
void fwkSendEventToAppDirectly( uint8_t appId, _tEQ* pEv )
{
  uint16_t uiEvId = pEv->eId;
  for (uint16_t i = 0; i < sizeof(eventHandlersList) / sizeof(_tAppEventLink); i++)
  {
      uint16_t uiTmp = eventHandlersList[i].event;
      if (uiTmp == uiEvId)
      {
          const pfnAppEventHandler * pHandlerList = eventHandlersList[i].pEventHandlersList;
          pHandlerList[appId](pEv);
          return;
      }
  }
}

/**
 * \fn     void fwkAppInit(void)
 * \brief  Intialize registered apllications before Events handling launch 
 * \param  None
 * \return None
 */
void fwkAppInit(void)
{
    _tEQ eq = { EV_APP_INIT, 00 };

    updateSystemTimeFromRTC();
    
#if 0
		if( RtcAlarm != rConditions.action )
		{
			// TODO: Clear testing data
			uint8_t year, date, month, weekDay;
			getDateFromRTC( &year, &month, &date, &weekDay );
			
			updateSystemTimeFromRTC();
			uint32_t seconds = 3600* sTimeStructureGet.Hours 
					+60*sTimeStructureGet.Minutes +sTimeStructureGet.Seconds; 
			
			seconds += 120; // Watering after 2 minutes seconds
			
			weekDay--;
			
			// Check swap for the next day
	//		if( seconds >= 86400 )
	//		{
	//			// disable previous timer
	//			getSettings()->dailyTimerSchedule[weekDay].minute = 0xff;
	//			getSettings()->dailyTimerSchedule[weekDay].hour   = 0xff;
	//			
	//			weekDay++;
	//			if( weekDay >= 7 )
	//				weekDay = 0;
	//			seconds -= 86400;
	//		}
	//		

			seconds = seconds / 60;
			uint8_t min = seconds % 60;
			seconds = seconds / 60;
			uint8_t hour = seconds % 24;
			
			if(( min == 0 )&&( hour == 0 ))
				min = 1;
			
			getSettings()->dailyTimerSchedule[weekDay].minute  = min;
			getSettings()->dailyTimerSchedule[weekDay].hour = hour;
			
			getSettings()->blUsePump = 0x01;		
			getSettings()->blRTCValid = 0x01;
			
			settings_flush();
		}
#endif		
		
		/* LEDS setting */
		for (uint8_t i = 0; i < APP_LEDS_NUMBER; ++i)
		{
		    fwkLedSet( i, 0, 0, 0 );
    }

    /* Display */
    dispClear();
    
    const char pMsg[] = "Initialization";
    
    dispDrawStr( 1, 0, pMsg);
    
    dispRedraw();
    
    eqReset();
        
    eqAddEvent(&eq);
}

/**
 * \fn     void fwkAppEventLoop(_tEQ* pE)
 * \brief  Poxies &pE event to registered Application Handlers 
 * \param  pE pointer to an Application Message 
 * \return None
 */
void fwkAppEventLoop(_tEQ* pE)
{
    uint16_t uiEvId = pE->eId;
    for (uint16_t i = 0; i < sizeof(eventHandlersList) / sizeof(_tAppEventLink); i++)
    {
        uint16_t uiTmp = eventHandlersList[i].event;
        if (uiTmp == uiEvId)
        {
            const pfnAppEventHandler * pHandlerList = eventHandlersList[i].pEventHandlersList;

            uint16_t j = 0;
            while (pHandlerList[j] != 0)
            {
                if (pHandlerList[j++](pE))
                    return;
            }
        }
    }
}

/**
 * \fn      void fwkMain(uint8_t blDoContinuesLoop)
 * \brief
 * \param   blDoContinuesLoop - 0x01 continues Event Queue handling without return
 *                            - 0x00 one time EQ reading (mostly for tetsting purposes)
 * \return  None.
 */
void fwkMain(uint8_t blDoContinuesLoop)
{
    _tEQ eqR;
    uint8_t blQeueNotEmpty;

    do
    {
        blQeueNotEmpty = eqGetEvent(&eqR);
        if (blQeueNotEmpty != 0 )
            fwkAppEventLoop(&eqR);
        else
        { 
            eqR.eId = EV_APP_IDLE;
            eqR.reserved = 0;
 //           fwkAppEventLoop(&eqR);
        }
    } while( blDoContinuesLoop || blQeueNotEmpty );
}


/* ------ Time & Date Fwk support ------ */
// TODO: Add comments here
void fwkGetTime(_tSysTime* pSystemTime)
{ 
  pSystemTime->seconds    = fwkSystemTime.seconds;
  pSystemTime->minutes    = fwkSystemTime.minutes;
  pSystemTime->hours      = fwkSystemTime.hours;
	pSystemTime->utcSeconds = fwkSystemTime.utcSeconds;
}


/**
 * \fn     void fwkSetTime(_tSysTime* pSystemTime)
 * \brief  Set a system time
 * \param  _tSysTime* pSystemTime pointer to filled system time structure
 * \return void
 */
void fwkSetTime(_tSysTime* pSystemTime)
{
  fwkSystemTime.seconds    = pSystemTime->seconds;
  fwkSystemTime.minutes    = pSystemTime->minutes;
  fwkSystemTime.hours      = pSystemTime->hours;
	fwkSystemTime.utcSeconds = pSystemTime->seconds + pSystemTime->minutes * 60 + pSystemTime->hours * 3600;
}

/*! The number of System Tick after start */
static uint32_t sysTicks = 0;

/*!
 \fn     uint8_t fwkSysTickEvHandler(_tEQ* p)
 \brief  Sytem ticks framework event handler
 \param  p pointer to event
 \return \b 0x01 - event should be suppressed
         \b 0x00 - event suppression is canceled
 */
uint8_t fwkSysTickEvHandler(_tEQ* p)
{
  sysTicks++;
  
  if( !(sysTicks % 10 ))
  {
    _tEQ eqSecondTick = { EV_APP_SEC_TICK, 00 };
    eqAddEvent( &eqSecondTick );

    if( !(sysTicks % (RTC_SYSTEM_TIME_UPDATE_GAP*10  )))
    {
      /* Update/Tune system time from RTC */
      updateSystemTimeFromRTC();
    }    
  }

  /* Check encoder input */
  mnTestInputEvents();
  
  return 0x00;
}

/*!
 \fn     uint8_t fwkDateTimeEvHandler(_tEQ* p)
 \brief  handler is called for Second Ticks to update system time
 \param  p pointer to event
 \retval 0x01 - event should be suppressed
 \retval 0x00 - event suppression is canceled
 */
uint8_t fwkDateTimeEvHandler(_tEQ* p)
{
  if (p->eId == EV_APP_SEC_TICK)
  {
    fwkSystemTime.seconds++;
    if( fwkSystemTime.seconds >= 60 )
    {
      fwkSystemTime.seconds = 0;
      fwkSystemTime.minutes++;
      if (fwkSystemTime.minutes >= 60)
      {
        fwkSystemTime.minutes = 0;
        fwkSystemTime.hours++;
        if (fwkSystemTime.hours >= 24)
          fwkSystemTime.hours = 0;
      }
    }
    
//    if( !((fwkSystemTime.utcSeconds++) % RTC_SYSTEM_TIME_UPDATE_GAP  ) )
//    {
//      /* Update/Tune system time from RTC */
//      updateSystemTimeFromRTC();
//    }       
    
    // Update utc like seconds
    fwkSystemTime.utcSeconds++;    
  }

  return 0x00;
}

/**
  * \fn     uint8_t fwkCheckTimeToSleep(void)
  * \brief
  * \param
  * \return 
  */
uint8_t fwkCheckTimeToSleep(void)
{
    return (fwkSystemTime.utcSeconds > timeToSleep);
}

/* ------ Display Support functions ------ */
/**
 * \fn     void dispRedraw(void)
 * \brief  updates display screen by display buffer
 */
void dispRedraw(void)
{
  deviceRedraw();

#ifdef VS_UTEST
  for( uint8_t i = 0; i<DISPLAY_LINES_NUMBER; i++)
  {
    DBG( LOG_DEBUG, "\r\n[" );

    for (uint8_t j = 0; j<DISPLAY_COLUMNS_NUMBER; j++)
        DBG( LOG_DEBUG, "%c",dsplDataScreen[i][j].pEntityVal);

    DBG( LOG_DEBUG, "]" );
  }
#endif
}

/**
 * \fn     void dispClear(void)
 * \brief  clear display
 */
void dispClear(void)
{
    for (uint8_t i=0; i <DISPLAY_LINES_NUMBER; i++)
    {
        for (uint8_t j=0; j <DISPLAY_COLUMNS_NUMBER; j++)
        {
            dsplDataScreen[i][j].blWasupdated = 0x01;
            dsplDataScreen[i][j].pEntityVal = ' ';
        }
    }
}

/**
 * \fn     void dispSetChar(uint8_t row, uint8_t column, char ch)
 * \brief  print char to row, column
 */
void dispSetChar(uint8_t row, uint8_t column, char ch)
{
    char chTmp = dsplDataScreen[row][column].pEntityVal;
    if( chTmp != ch )
    {
      dsplDataScreen[row][column].pEntityVal = ch;
      dsplDataScreen[row][column].blWasupdated = 0x01;
    }
    else
      dsplDataScreen[row][column].blWasupdated = 0x00;
}

/**
 * \fn     void dispSetLineCh(uint8_t row, char ch)
 * \brief  set hal line for character ch
 */
void dispSetLineCh(uint8_t row, char ch)
{
    for (uint8_t i = 0; i < DISPLAY_COLUMNS_NUMBER; i++)
        dispSetChar(row, i, ch);
}


/**
 * \fn     void dispDrawStr(uint8_t row, uint8_t col, const char* const pcStr)
 * \brief  print string from eeprom to display for row and from column
 */
void dispDrawStr(uint8_t row, uint8_t col, const char* const pcStr)
{
	uint8_t i = 0;
	uint8_t column = col;

	
	while( pcStr[ i ]!= 0x00 ) 
	{
			dispSetChar(row, column, pcStr[i]);
			i++;
			column++;
	}

	while( column < DISPLAY_COLUMNS_NUMBER)
			dispSetChar(row, column++, ' ');
}

void dispDrawStrN(uint8_t row, uint8_t col, const char* const pcStr)
{
  uint8_t i = 0;
  uint8_t column = col;


  while( pcStr[ i ]!= 0x00 )
  {
      dispSetChar(row, column, pcStr[i]);
      i++;
      column++;
  }

  while( column < DISPLAY_COLUMNS_NUMBER)
      dispSetChar(row, column++, ' ');
}

/**
 * \fn     uint8_t dispCheckStr(uint8_t row, const char* const pcStr)
 * \brief  compares string into display buffer and pcStr
 * \return 0x00 - false; 0x01 - true
 */
uint8_t dispCheckStr(uint8_t row, const char* const pcStr)
{
    uint8_t i = 0;

    if (row < DISPLAY_LINES_NUMBER)
    {
        while (pcStr[i] != 0x00)
        {
            if (i < DISPLAY_COLUMNS_NUMBER)
                if (dsplDataScreen[row][i].pEntityVal != pcStr[i])
                    return 0x00;
                else
                  i++;
            else
                return 0x00;
        }
          
        while (i < DISPLAY_COLUMNS_NUMBER)
            if (dsplDataScreen[row][i++].pEntityVal != ' ')
                return 0x00;

        return 0x01;
    }
    return 0x00;
}

/**
 * \fn     char* dispGetString(uint8_t row, char* pBuff)
 * \brief
 * \param
 * \return
 */
char* dispGetString(uint8_t row, char* pBuff)
{
  for (size_t i = 0; i < 16; i++)
  {
    pBuff[i] = dsplDataScreen[row][i].pEntityVal;
  }

  pBuff[16] = 0;
  return pBuff;
}

/* ------------------------------------------------------------------------- 
    FRAMEWORK APPLICATIONS
---------------------------------------------------------------------------- */
// =========================================================================
// Key pressing detection application
// =========================================================================

// =========================================================================
// Sleeping application
// Sleeping application monitors sleeping conditions
// When conditions are met then hw dependant func is called
// =========================================================================
/** the number of inactivity seconds before sleeping proc will be initiated */
uint8_t uiActivationSleepAppTO;

uint8_t fwkSleepInitEvHandler(_tEQ* p)
{
	uiActivationSleepAppTO = 3;
  return 0x00;
}

/* Monitoring Sleeping case */
uint8_t fwkSleepSleepEvHandler(_tEQ* p)
{
  /* Perform sleeping sequence */
  hwSleepingInitiate( );

  return 0x01;
}

uint8_t fwkSleepIdleEvHandler(_tEQ* p)
{
  return 0x01;
}

/* EV_APP_SEC_TICK event handler 
*/
uint8_t fwkSleepSecTicksEvHandler(_tEQ* p)
{
  if(!uiActivationSleepAppTO)
  {
    _tEQ eq = { EV_APP_SLEEP, 00 };
    eqAddEvent( &eq );
  }
  else
    uiActivationSleepAppTO--;

  return 0x01;
}

