/*! \file      fsmappl.c
    \version   0.1
    \date      2017-04-24 13:34
		\author    ruarka

    \brief     Main Menu FSM file provides facade to menu realization    
                
               Application Final State Machine(FSM) facase implementation.
               Actions handlers related to application service logic are 
               gathered here. 
               So actions to be done during state to state 
               transition should be presented here. 

    \bug   
 */

// TODO: Remove all EV_APP_INIT handlers form menu child states

/* ------------------------------------------------------------------------------------------------
 *                                 Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>
#include <stdbool.h>

#include "apptypes.h"

#include  "events.h"
#include  "eventq.h"
#include  "fsmappl.h"
#include  "fsmbasic.h"
#include  "fsmwatch.h"
#include  "fsmsets0.h"
#include  "fsmtc.h"
#include  "fsmdc.h"
#include  "fsmwatering.h"
#include  "settings.h"
#include  "appfwk.h"
#include  "main.h"

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
/*! \var tFsmMenuIfs *pMenuApplState 
    \brief Keeps actual menu FSM state 
 */
// tFsmMenuIfs *pMenuApplState = &menuStartState;
tFsmMenuIfs *pMenuApplState = &menuWatchState;

/* ------------------------------------------------------------------------------------------------
 *                                 Local Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
*                                          Functions
* ------------------------------------------------------------------------------------------------
*/

uint32_t uiSecondsToDisactivateMenu = INACTIVITY_TIME_DETECTION;
uint8_t  blMenuActive     = 1;

/**
 \fn     bool const appMenuEvHandler(_tEQ* p)
 \brief  Performs calling event handler for current FSM state
 \param  p Event
 \return translates current state return value
 */
uint8_t appMenuEvHandler(_tEQ* p) 
{
  uint16_t uiTmp = p->eId;

  if(( uiTmp == EV_APP_LEFT )
      ||(uiTmp == EV_APP_DOWN)
      ||(uiTmp == EV_APP_RIGHT )
      ||(uiTmp == EV_APP_UP)
      ||( uiTmp == EV_APP_BTN_ACT ))
  {
    uiSecondsToDisactivateMenu = INACTIVITY_TIME_DETECTION;
    if( !blMenuActive )
    {
        // Block/Stop watering application
        _tEQ stopEv = { EV_DROP_ACT , 0};
        // Send STOP event to cancel watering
        fwkSendEventToAppDirectly( 0 /* Watering */, &stopEv);
    }
    blMenuActive = 1;
  }

  if( uiTmp == EV_APP_SEC_TICK )
  {
      if( !uiSecondsToDisactivateMenu )
      {
          // Inactivity Menu App - Launch/Unblock watering application
          if( blMenuActive )
          {
              // Watch screen is displayed
              applMenuChangeStateEx(&menuWatchState);

              _tEQ rsmEv = { EV_RSM_ACT , 0};
              // Send Resume Event to watering
              fwkSendEventToAppDirectly( 0 /* Watering */, &rsmEv);
          }
          blMenuActive = 0;
      }
      else
        uiSecondsToDisactivateMenu--;
  }

  if( uiTmp == EV_APP_INIT )
  {
      // Set initial value for menu pointer
      curr = NULL;
    
      if( applContext.startConditions == ByReset )
      {
          blMenuActive = 1;
          uiSecondsToDisactivateMenu = INACTIVITY_TIME_DETECTION;

          // Watch screen is displayed
          applMenuChangeStateEx(&menuWatchState);
      }
      else
        uiSecondsToDisactivateMenu = 0;
      return 0;
  }

  if( uiTmp == EV_APP_SLEEP )
    return blMenuActive;

  return (*pMenuApplState->pEventHandler)( p );
}

/**
 * \fn     void appPumpOnOff(bool PumpState)
 * \brief  Switch ON/OFF the PUMP 
 * \param  0x00 - Off, others - on
 * \return None
 */
void appPumpOnOff(uint8_t PumpState)
{
    if (getSettings()->blUsePump)
    {
        if (PumpState)
        {
					/* Pump On */
					setPumpControl(0x01);
        }
        else
        {
					/* Pump off */
					setPumpControl(0x00);
        }
    }
    else
    {
			/* Pump is off in settings - off for insurance */
			setPumpControl(0x00);
    }
}

void applMenuChangeStateEx( tFsmMenuIfs * newState )
{
     if(pMenuApplState->pfnMenuChangeStateAfter)
         pMenuApplState->pfnMenuChangeStateAfter( none );

     pMenuApplState  = newState;

     if(pMenuApplState->pfnMenuChangeStateBefore)
         pMenuApplState->pfnMenuChangeStateBefore( none );
}
