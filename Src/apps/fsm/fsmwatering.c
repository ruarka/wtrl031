/*! \file      fsmwatering.c
 *  \version   0.1
 *  \date      2017-06-03 22:26
 *  \brief     Menu FSM state provides watering function through menu
 *	\author    ruarka
 *
 *  \bug       
 * 	
 *  \details   User select start to launch watering. 
 *	\details   When user stops watering process by 'StopS' then seconds value is stored 
 *             to the setting structure. To save watering seconds into EPROM settings 
 *             structure should be stored.
 *  \details   When user uses 'Stop' it just finishes watering.
*/
/* ------------------------------------------------------------------------------------------------
 *                                 Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>

//#include "apptypes.h"

#include "events.h"
#include "eventq.h"
#include "fsmappl.h"
#include "fsmwatch.h"
#include "fsmsets0.h"
#include "fsmwatering.h"
#include "settings.h"
#include "appfwk.h"

/* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */
 
/* ------------------------------------------------------------------------------------------------
 *                                 Local Variables
 * ------------------------------------------------------------------------------------------------
 */
/*! */
const char* const menuWateringStart[] =
{
    "Start"
};

const char* const menuWateringStop[] =
{
    "Stop",
    "StopS"
};


tWateringState wState;

uint16_t uiWtSecs;


/* ------------------------------------------------------------------------------------------------
 *                                          Functions
 * ------------------------------------------------------------------------------------------------
 */
void menuWateringChangeStateBefore(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Watering:Before" );

    uiWtSecs = 0;

    wState = Start;

    /* Print Start as menu */
    dispClear();
//    printMenu(menuWateringStart, (sizeof(menuWateringStart) / sizeof(const char* const)), 0);

    /* Print watering time */
    // printUint16ToDspl(1, DSPL_WTG_SECS_COL, uiWtSecs);

    dispRedraw();
}

void menuWateringChangeStateAfter(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Watering:After" );
	
  /* Turn Off Pump - To be definitly sure */
  appPumpOnOff(0x00);	
}

uint8_t menuWateringEventHandler(_tEQ* p)
{
//  uint8_t uiTmp;

  DBGT( LOG_DEBUG, "FSM:Watering:EH-%d:%d", p->eId, p->reserved );
  
    switch (p->eId)
    {
    case EV_APP_INIT:
        applMenuChangeStateEx(&menuWatchState);
        break;

    case EV_APP_SEC_TICK:
        if ((wState == Stop) || (wState == StopS))
        {
            uiWtSecs++;

            /* Print Watering Time */
            // printUint16ToDspl(1, DSPL_WTG_SECS_COL, uiWtSecs);
            
            dispRedraw();
        }
        break;

    case EV_APP_LEFT:
        if ( wState == Start )
            applMenuChangeStateEx(&menuSets0State);

        if ((wState == Stop) || (wState == StopS))
				{
          applMenuChangeStateEx(&menuWateringState);
					
					/* Turn Off Pump */
          appPumpOnOff(0x00);
				}
        break;

    case EV_APP_RIGHT:
    case EV_APP_BTN_ACT:
        if (wState == Start)
        {
            wState = Stop;

            uiWtSecs = 0;

            /* Print Stop/StopS menu */
            // printMenu(menuWateringStop, (sizeof(menuWateringStop) / sizeof(const char* const)), 0);

            /* Print Watering Time */
            // printUint16ToDspl(1, DSPL_WTG_SECS_COL, uiWtSecs);

            dispRedraw();

            /* Turm On Pumpp */
            appPumpOnOff(0x01);
        }
        else if (wState == Stop )
        {
            /* Turn Off Pump */
            appPumpOnOff(0x00);

            applMenuChangeStateEx(&menuWateringState);
        }
        else if (wState == StopS)
        {
            /* Turn Off Pump */
            appPumpOnOff(0x00);

            /* save state uiWtSecs to  */
            getSettings()->secondsWatering = uiWtSecs;

            // settings_flush();
            flushSettings();

            applMenuChangeStateEx(&menuWateringState);
        }
        break;

    case EV_APP_UP:
    case EV_APP_DOWN:
        if(wState == Stop)
        {
            wState = StopS;
//            uiTmp = 1;
        }
        else if (wState == StopS)
        {
            wState = Stop;
//            uiTmp = 0;
        }

        if ((wState == Stop) || (wState == StopS))
        {
            dispClear();

            /* Print Stop/StopS menu */
            //printMenu(menuWateringStop, (sizeof(menuWateringStop) / sizeof(const char* const)), uiTmp);

            /* Print Watering Time */
            // printUint16ToDspl(1, DSPL_WTG_SECS_COL, uiWtSecs);

            dispRedraw();
        }
        break;

    case EV_APP_IDLE:
    case EV_APP_SLEEP:
        break;

    default:
        applMenuChangeStateEx(&menuWatchState);
        break;
    }

    return 0x00;
}

/* ------------------------------------------------------------------------------------------------
*                                 Global Variables
* ------------------------------------------------------------------------------------------------
*/
/*! \var tFsmMenuIfs menuWateringState
    \brief Menu Watering state interface value.
*/
tFsmMenuIfs menuWateringState =
{
    menuWateringChangeStateBefore,
    menuWateringChangeStateAfter,
    menuWateringEventHandler
};
