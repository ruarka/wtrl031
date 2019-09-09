/*! \file      fsmwatch.c
    \version   0.1
    \date      2017-06-03 22:06
    \brief     Menu FSM state provides watch screen
		\author    ruarka

    \bug       
		
    \details   
*/
/* ------------------------------------------------------------------------------------------------
 *                                 Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>

#include "apptypes.h"

#include "events.h"
#include "eventq.h"
#include "fsmappl.h"
#include "fsmbasic.h"
#include "fsmwatch.h"
#include "fsmsets0.h"
#include "settings.h"
#include "appfwk.h"
#include "main.h"

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

/* ------------------------------------------------------------------------------------------------
 *                                          Functions
 * ------------------------------------------------------------------------------------------------
 */
#if 0
void printMessageToBoard( void )
{
	uint8_t year, date, month, weekDay;
	
	/* clear display */
	// dispClear();

	/* Print Time, Minutes to watering, Charge Lvl to disp */
	printTimeToDspl();

	getDateFromRTC( &year, &month, &date, &weekDay);
	
	/* print weekday to board */
	const char* const pcStr =  weekDayStrs[ weekDay ];
	dispDrawStr( DSPL_WATERING_MINUTES_ROW, DSPL_WATERING_MINUTES_COL, pcStr);
	
	dispSetChar(DSPL_CHARGE_LVL_ROW, DSLP_CHARGE_LVL_COL, pIntToHex[5]);
	dispRedraw();
}	

void menuWatchChangeStateBefore(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Watch:Before" );
  
  /* clear display */
  dispClear();
//  dispRedraw();

	printMessageToBoard();
}

void menuWatchChangeStateAfter(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Watch:After" );
}

uint8_t menuWatchEventHandler(_tEQ* p)
{
  DBGT( LOG_DEBUG, "FSM:Watch:EH-%d:%d", p->eId, p->reserved );

    switch (p->eId)
    {
    case EV_APP_INIT:
        applMenuChangeState(watch);
        break;

    case EV_APP_SEC_TICK:
        /* Print Time, Minutes to watering, Charge Lvl to disp */
				printMessageToBoard();
        break;

    case EV_APP_UP:
    case EV_APP_DOWN:
    case EV_APP_LEFT:
    case EV_APP_RIGHT:
    case EV_APP_BTN_LEFT:
        applMenuChangeState(menu_settings0);
        break;

    case EV_APP_IDLE:
				break;
		
    case EV_APP_SLEEP:
				break;

    default:
        applMenuChangeState( none );
        break;
    }

    return 0x00;
}
#endif

// ---------------------------------------------------------------------
// New
// ---------------------------------------------------------------------

#if 1
void printMessageToBoard( void )
{
  uint8_t year, date, month, weekDay;

  getDateFromRTC( &year, &month, &date, &weekDay);

  /* print weekday to board */
  _tSysTime time;
  fwkGetTime(&time);

  sprintf( pDisplayStrBuff, "%02d:%02d:%02d %2s %c",
           time.hours, time.minutes, time.seconds,  weekDayStrs[ weekDay ], pIntToHex[5] );

  dispDrawStrN( DSPL_WATERING_MINUTES_ROW, 0, pDisplayStrBuff );

  dispRedraw();
}

void menuWatchChangeStateBefore(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Watch:Before" );

  /* clear display */
  dispClear();

  printMessageToBoard();
}

void menuWatchChangeStateAfter(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Watch:After" );
}

uint8_t menuWatchEventHandler(_tEQ* p)
{
  DBGT( LOG_DEBUG, "FSM:Watch:EH-%d:%d", p->eId, p->reserved );

    switch (p->eId)
    {
    case EV_APP_INIT:
        applMenuChangeStateEx(&menuWatchState);
        break;

    case EV_APP_SEC_TICK:
        /* Print Time, Minutes to watering, Charge Lvl to disp */
        printMessageToBoard();
        break;

    case EV_APP_UP:
    case EV_APP_DOWN:
    case EV_APP_LEFT:
    case EV_APP_RIGHT:
        applMenuChangeStateEx(&menuSets0State);
        break;
    
    case EV_APP_BTN_ACT:
        if( p->reserved )
          applMenuChangeStateEx(&menuSets0State);
        break;

    case EV_APP_IDLE:
      break;


    case EV_APP_SLEEP:
        return blMenuActive;

//    default:
//        applMenuChangeState( none );
//        break;
    }

    return 0x00;
}

#endif

/* ------------------------------------------------------------------------------------------------
 *                                 Global Variables
 * ------------------------------------------------------------------------------------------------
*/
/*! \var tFsmMenuIfs menuWatchState
    \brief Menu Watch state interface value.
 */
tFsmMenuIfs menuWatchState =
{
    menuWatchChangeStateBefore,
    menuWatchChangeStateAfter,
    menuWatchEventHandler
};
