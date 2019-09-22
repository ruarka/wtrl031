/*! \file      appwatering.c
    \version   0.1
    \date      2017-07-18 18:25
    \brief     Watering Application 
    \copyright  
    \author    ruarka
		\details  
		

		Watering App State sequence

		State     ->  	Possible Transitions
		NONE        	NONE (All except EV_APP_INIT ) 			// Wait for Apps initialization
									IDLE (EV_APP_INIT) 						 			// Initialization happened

		IDLE      ->		PWRON (EV_APP_SEC_TICK)							// Power is stable
									  IDLE (All except EV_APP_SEC_TICK) 	// ignore

		PWRON     ->    WATERING (EV_ADC_SCAN)              // Watering is needed
		                STOP_ERR (EV_APP_SEC_TICK)          // Something wrong with ADC
		                STOP (EV_ADC_SCAN)                  // Watering is not needed
		                IDLE(EV_APP_INIT)                   // initialization is applied
                    PWRON (All except EV_ADC_SCAN, EV_APP_SEC_TICK, EV_APP_INIT) // Just ignore it

    // watering process
    WATERING  ->    STOP (EV_ADC_SCAN, EV_APP_SEC_TICK) // Just show watering is finished
                    STOP_ERR (EV_APP_SEC_TICK)

    // Time to show before sleep that watering is finished
    STOP      ->    STOP(All except EV_APP_INIT,EV_APP_SEC_TICK)
                    IDLE(EV_APP_INIT)
                    WAIT_SLEEP(All except EV_APP_INIT)

    // Time to show before sleep there is some error
    STOP_ERR  ->    STOP_ERR (All except EV_APP_INIT,EV_APP_SEC_TICK )
                    WAIT_SLEEP (EV_APP_SEC_TICK)
                    IDLE(EV_APP_INIT)
    //
    WAIT_SLEEP ->   IDLE(EV_APP_INIT)
                    WAIT_SLEEP(All except EV_APP_INIT)


		Watering Application is asked before MC goes to sleeping.
		
		There are different 2 cases:
		a) MC was waked up by user - rConditions.action == PwrOn. It means user 
		   pressed "Start/Reset" key. When MC detects user inactivity for time gap
			 defined by settings()->secondsToSleep Watering Application is asked 
			 to check it is needed to water or not. Sleeping Management is done 
			 by EV_APP_SLEEP suppression.
			 Watering App suppress EV_APP_SLEEP until watering is in progress.

		b) MC was waked up by RTC Alarm - rConditions.action == RtcAlarm. There is 
		   no user activity. MC checks watering conditions and starts watering 
			 if it is met. After watering is finished Sleeping procedure is initiated 
			 by canceling EV_APP_SLEEP suppression.
			 If time is out of watering so EV_APP_SLEEP suppression is not activated
			 and MC goes to sleep.

		SLEEPING EVENT suppression is done by handling EV_APP_SLEEP event.
		When there is no watering activity EV_APP_SLEEP suppression is canceled.
		That leeds to RTC Alarm triggering and MC power down.     
		
		\bug
*/
// TODO: Rewrite details. It is completely different.
/* ------------------------------------------------------------------------------------------------
 *                                 Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>
#include <stdio.h>

#include "apptypes.h"
#include "events.h"
#include "eventq.h"
#include "settings.h"
#include "appfwk.h"
#include "main.h"
#include "fsmappl.h"
#include "appwatering.h"


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

/* ------------------------------------------------------------------------------------------------
 *                                 Local Variables
 * ------------------------------------------------------------------------------------------------
 */

uint16_t  uiStopWtSamples;
uint8_t   uiWtrAppState     = WTR_NONE;
uint8_t   uiWtrNoneSeconds  = 0;
uint8_t   uiWtrIdleSeconds  = 0;
uint8_t   uiWtrAdcSmpNum	  = 0;
uint8_t   uiStateTO		      = 0;
uint32_t  uiADCSum          = 0;
uint8_t   blSuspended       = 1;


/* ------------------------------------------------------------------------------------------------
 *                                          Functions
 * ------------------------------------------------------------------------------------------------
 */

static void setFullBlock( void )
{
  DBGT(LOG_DEBUG, "WTR:PMP:OFF");
  setPumpControl( 0x00 );             /* switch OFF pump */
  DBGT(LOG_DEBUG, "WTR:PWR:OFF");
  setPwrControl( 0 );                 /* switch OFF pwr */
  fwkLedSet( WRT_ERR_LED, 0, 0, 0 );
  setADCChanel( ACD_CHNL_START, 0x00 );
  setADCChanel( ACD_CHNL_STOP, 0x00 );
  stopADCConversion();
}

static void setWtrAppState( uint8_t newState )
{
  DBGT(LOG_DEBUG, "WTR:%d->%d", uiWtrAppState, newState );

	switch (newState) {
		case WTR_NONE:
			uiWtrAppState 			= WTR_NONE;
			uiWtrNoneSeconds 		= 0;
			setFullBlock();
			fwkLedBlink( 0, 2, 8 );
    
      if( applContext.startConditions == ByReset )
        blSuspended = 0x01;
      else
        blSuspended = 0x00;
    
			break;

		case WTR_IDLE:
			uiWtrAppState 		= WTR_IDLE;
			uiWtrIdleSeconds 	= 1;
			setFullBlock();
			fwkLedBlink( 0, 2, 8 );
    
      if( applContext.startConditions == ByReset )
        blSuspended = 0x01;
      else
        blSuspended = 0x00;
      
			break;

		case WTR_PWRON:
		  uiStateTO         = 1;
      uiWtrAdcSmpNum    = 1;
      uiADCSum          = 0;
		  uiWtrAppState     = WTR_PWRON;

      DBGT(LOG_DEBUG, "WTR:PMP:OFF");
      setPumpControl( 0x00 ); /* switch OFF pump */
      DBGT(LOG_DEBUG, "WTR:PWR:ON");
      setPwrControl( 0x01 );  /* switch ON pwr */

      fwkLedBlink( 0, 2, 8 );

		  setADCChanel( ACD_CHNL_START, 0x01 );
		  startADCConversion();
			break;

		case WTR_WATERING:
      DBGT(LOG_DEBUG, "WTR:PMP:ON");
      setPumpControl( 0x01 ); /* switch ON pump */
		  stopADCConversion();
    
      setADCChanel( ACD_CHNL_START, 0x00 );
		  setADCChanel( ACD_CHNL_STOP, 0x01 );
		  startADCConversion();
    
		  uiStateTO         = 1;
		  uiStopWtSamples   = 1;
		  uiADCSum          = 0;
		  uiWtrAppState     = WTR_WATERING;

		  fwkLedOn( 0 );
		  break;

		case WTR_STOP:
      setFullBlock();
      uiStateTO = 1;
      uiWtrAppState = WTR_STOP;

      DBGT( LOG_DEBUG, "Stop Indication" );
      fwkLedBlink( 0, 3, 3 );
		  break;

		case WTR_STOP_ERR:
		  setFullBlock();
      uiStateTO = 1;
      uiWtrAppState = WTR_STOP_ERR;
      DBGT( LOG_DEBUG, "!!!ERROR Indication!!!" );
      fwkLedBlink( 0, 1, 1 );
			break;

		case WTR_WAIT_SLEEP:
		  setFullBlock();
		  uiWtrAppState = WTR_WAIT_SLEEP;

		  DBGT( LOG_DEBUG, "Waiting Sleep" );
		  fwkLedOff( 0 );
		  break;

		default:
			break;
	}
}

/* Event Handlers */
static uint8_t wtrNoneStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:               // until EV_APP_INIT no App activity
    setWtrAppState( WTR_IDLE );
    return 0;

  case EV_APP_SLEEP:              // Cycle is not finished
    return 1;

  case EV_PARK_ACT:
  case EV_DROP_ACT:
  case EV_RSM_ACT:
  case EV_APP_SEC_TICK:
  case EV_ADC_SCAN:
  default:
    return 0;
  }
}

// State IDLE
static uint8_t wtrIdleStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:
    setWtrAppState( WTR_IDLE );
    return 0;

  case EV_APP_SLEEP:  // Cycle is not finished
    return 1;

  case EV_APP_SEC_TICK:
    if( uiWtrIdleSeconds > WTR_IDLE_TIME )
        setWtrAppState( WTR_PWRON );
    else
      uiWtrIdleSeconds++;

    return 0;

  case EV_DROP_ACT:
  case EV_PARK_ACT:
  case EV_RSM_ACT:
  case EV_ADC_SCAN:
  default:
    return 0;
  }
}

// PWRON
static uint8_t wtrPwrOnStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:
    setWtrAppState( WTR_IDLE );
    return 0;

  case EV_APP_SEC_TICK:
    if( uiStateTO >= WTR_PWRON_STAB_TIME )
      setWtrAppState( WTR_STOP_ERR );
    else
      uiStateTO++;
    return 0;

  case EV_APP_SLEEP:
    return 1;

  case EV_ADC_SCAN:
    uiADCSum = uiADCSum + p->reserved;

    DBGT( LOG_DEBUG, "WTR:ADC:%d:%d", ACD_CHNL_START, p->reserved );

#if 0
    if( uiWtrAdcSmpNum >= WTR_ADC_WATERING_SAMPLES )
    {
      uiADCSum = uiADCSum/WTR_ADC_WATERING_SAMPLES;
      if( uiADCSum > WTR_WATERING_START_SHD)
        setWtrAppState( WTR_WATERING ); // Have to Watering
      else
        setWtrAppState( WTR_STOP );  // Watering is not needed
    }
    else
    {
      uiWtrAdcSmpNum++;
      
      startADCConversion();  // Start new conversation
    }
#endif

#if 1
    if( uiWtrAdcSmpNum >= WTR_ADC_WATERING_SAMPLES )
    {
      uint8_t uiMappedADCSum =( uint8_t) mapDigitalValue( uiADCSum/WTR_ADC_WATERING_SAMPLES, 3500, 400, 10 );

      if( uiMappedADCSum > getSettings()->uiStartSensorThreshold )
        setWtrAppState( WTR_WATERING ); // Have to Watering
      else
        setWtrAppState( WTR_STOP );  // Watering is not needed
    }
    else
    {
      uiWtrAdcSmpNum++;

      startADCConversion();  // Start new conversation
    }

#endif

    return 0;

  case EV_PARK_ACT:
  case EV_DROP_ACT:
  case EV_RSM_ACT:
  default:
    return 0;
  }
}

// WATERING
static uint8_t wtrWateringStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:
    setWtrAppState( WTR_IDLE );
    return 0;

  case EV_APP_SLEEP:
    return 1;

  case EV_APP_SEC_TICK:
    if( uiStateTO >= getSettings()->secondsWatering )
       setWtrAppState( WTR_STOP );
    else
      uiStateTO++;

    return 0;

  case EV_ADC_SCAN:
    
    DBGT( LOG_DEBUG, "WTR:ADC:%d:%d", ACD_CHNL_START, p->reserved ); 
#if 0
    if( uiStopWtSamples++ > WTR_ADC_STOP_WATERING_SAMPLES )
    {
        if( uiADCSum/WTR_ADC_STOP_WATERING_SAMPLES <  WTR_WATERING_STOP_SHD )
          setWtrAppState( WTR_STOP );
        else
        {
            uiStopWtSamples = 1;
            uiADCSum = 0;

            startADCConversion();  // Start new conversation       
        }
    }
    else
    {
      uiADCSum += p->reserved;
      
      startADCConversion();  // Start new conversation
    }
    return 0;
#endif

#if 1
    if( uiStopWtSamples++ > WTR_ADC_STOP_WATERING_SAMPLES )
    {
        uint8_t uiMappedADCSum =( uint8_t) mapDigitalValue( uiADCSum/WTR_ADC_STOP_WATERING_SAMPLES, 3500, 400, 10 );

        if( uiMappedADCSum < getSettings()->uiStopSensorThreshold )
          setWtrAppState( WTR_STOP );
        else
        {
            uiStopWtSamples = 1;
            uiADCSum = 0;

            startADCConversion();  // Start new conversation
        }
    }
    else
    {
      uiADCSum += p->reserved;

      startADCConversion();  // Start new conversation
    }
    return 0;
#endif

  case EV_PARK_ACT:
  case EV_DROP_ACT:
  case EV_RSM_ACT:
  default:
    return 0;
  }
}

// STOP -  indication purposes for success cycle
static uint8_t wtrStopStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:
    setWtrAppState( WTR_IDLE );
    return 0;

  case EV_APP_SLEEP:
    return 1;

  case EV_APP_SEC_TICK:
    if( uiStateTO >= WRT_ERRSTOP_INDICATION_TIME )
        setWtrAppState( WTR_WAIT_SLEEP );
    else
      uiStateTO++;
    return 0;

  case EV_ADC_SCAN:
  case EV_SYS_TICK:
  case EV_PARK_ACT:
  case EV_DROP_ACT:
  case EV_RSM_ACT:
  default:
    return 0;
  }
}

// STOP_ERR - indication for error happened
static uint8_t wtrStopErrStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:
    setWtrAppState( WTR_IDLE );
    return 0;

  case EV_APP_SEC_TICK:
    if( uiStateTO >= WRT_ERRSTOP_INDICATION_TIME )
        setWtrAppState( WTR_WAIT_SLEEP );
    else
      uiStateTO++;
    return 0;

  case EV_APP_SLEEP:
    return 1;

  case EV_ADC_SCAN:
    return 0;

  // TODO: Error indication
  case EV_SYS_TICK:
    return 0;

  case EV_PARK_ACT:
  case EV_DROP_ACT:
  case EV_RSM_ACT:
  default:
    return 0;
  }
}

// WAIT_SLEEP
static uint8_t wtrWaitSleepStateEh(_tEQ* p)
{
  switch (p->eId)
  {
  case EV_APP_INIT:
    setWtrAppState( WTR_IDLE );
    return 1;

  case EV_APP_SLEEP:
  case EV_APP_SEC_TICK:
  case EV_ADC_SCAN:
  case EV_PARK_ACT:
  case EV_DROP_ACT:
  case EV_RSM_ACT:
  default:
    return 0;
  }
}

/*!
 \fn			uint8_t appWateringEvHandler(_tEQ* p)
 \brief 	Framework events handler
 \param 	_tEQ* p Pointer to event object from events queue
 \retval	0 - handled event should be suppressed
 \retval	1 - handled event should be delivered to other apps
 */
uint8_t appWateringEvHandler(_tEQ* p)
{
    DBGT(LOG_DEBUG, "WTR:EH:%X-%X-%d", (p->eId), (p->reserved), eqGetNumbers());

    uint16_t uiTmp = p->eId;
  
    /* Application control block */
    if(( uiTmp == EV_RSM_ACT )
        &&( blSuspended ))
    {
      setWtrAppState( WTR_IDLE );
      blSuspended = 0;
      return 0x00;
    }
    
    if((( uiTmp == EV_PARK_ACT )
      ||( uiTmp == EV_DROP_ACT ))
      &&( !blSuspended ))
    {
      setWtrAppState( WTR_IDLE );
      blSuspended = 1;
      return 0;
    }
    
    if( uiTmp == EV_APP_INIT )
    {
      if( applContext.startConditions == ByWakeUp )
        blSuspended = 0;
    }
    
    if( blSuspended )
      return 0;
    
    if( getSettings()->mode == MODE_AOS )
    {
      switch (uiWtrAppState) {
        case WTR_NONE:
          return wtrNoneStateEh( p );

        case WTR_IDLE:
          return wtrIdleStateEh( p );

        case WTR_PWRON:
          return wtrPwrOnStateEh( p );

        case WTR_WATERING:
          return wtrWateringStateEh( p );

        case WTR_STOP:
          return wtrStopStateEh( p );

        case WTR_WAIT_SLEEP:
          return wtrWaitSleepStateEh( p );

        case WTR_STOP_ERR:
        default:
          return wtrStopErrStateEh( p );
      }
    }
    else
    {
      // TODO: On timer mode handlers
      return 0;
    }
}

