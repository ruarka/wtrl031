/*! \file      fsmbasic.c
    \version   0.1  
    \date      2017-06-03 23:14
    \brief     Defines Basic State and Start State for menu FSM.
		\author    ruarka

    \bug       
		
    \details   Parent object/state for FSM. 
		           State state fsm is also defiend here.
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

/* ------------------------------------------------------------------------------------------------
*                                          Functions
* ------------------------------------------------------------------------------------------------
*/

/*
    Starte State FSM interface functions
*/

/**
 * \fn     void menuStartChangeStateBefore(tFsmMenuState newState)
 * \brief  deligate called before change state
 * \param  tFsmMenuState newState - pointer to new FSM state
 * \return None
 */
//void menuStartChangeStateBefore(tFsmMenuState newState)
//{
//  DBGT( LOG_DEBUG, "FSM:Start:Before" );
//}

/**
 * \fn     void menuStartChangeStateAfter(tFsmMenuState newState)
 * \brief  deligate called after change state
 * \param  tFsmMenuState newState - pointer to new FSM state
 * \return None
 */
// void menuStartChangeStateAfter(tFsmMenuState newState)
//{
//  DBGT( LOG_DEBUG, "FSM:Start:After" );
//}

/**
 * \fn     uint8_t menuStartEventHandler(_tEQ* p)
 * \brief  Start FSM object event handler
 * \param  _tEQ* p - event object pointer routed to FSM
 * \return 0x01 - event should be supressed 
 *         0x00 - event supression is canceled
 */
//uint8_t menuStartEventHandler(_tEQ* p)
//{
//  DBGT( LOG_DEBUG, "FSM:Start:EH-%d:%d", p->eId, p->reserved );
//
//     switch (p->eId)
//     {
//     case EV_APP_INIT:
//         applMenuChangeState( watch );
//         break;
//
//     case EV_APP_IDLE:
//     case EV_APP_SLEEP:
//     case EV_APP_SEC_TICK:
//         // TODO: ReDraw seconds. Get actual time
//     case EV_APP_UP:
//     case EV_APP_DOWN:
//     case EV_APP_LEFT:
//     case EV_APP_RIGHT:
//     case EV_APP_STAR:
//         // TODO: Go to settings state
//     default:
//         // TODO: Unknown Event
//         break;
//     }
//
//    return 0x00;
//}

/*
    None State FSM interface functions
*/

/**
 * \fn     void menuNoneChangeStateBefore(tFsmMenuState newState)
 * \brief  deligate called before change state
 * \param  tFsmMenuState newState - pointer to new FSM state
 * \return None
 */
void menuNoneChangeStateBefore(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:None:Before" );
}

/**
 * \fn     void menuNoneChangeStateAfter(tFsmMenuState newState)
 * \brief  deligate called after change state
 * \param  tFsmMenuState newState - pointer to new FSM state
 * \return None
 */
void menuNoneChangeStateAfter(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:None:After" );
}

/**
 * \fn     uint8_t menuNoneEventHandler(_tEQ* p)
 * \brief  Basic object event handler
 * \param  _tEQ* p - event object pointer routed to FSM
 * \return 0x01 - event should be supressed 
 *         0x00 - event supression is canceled
 */
uint8_t menuNoneEventHandler(_tEQ* p)
{
  DBGT( LOG_DEBUG, "FSM:None:EH-%d:%d", p->eId, p->reserved );

  return 0x00;
}

/* ------------------------------------------------------------------------------------------------
*                                 Global Variables
* ------------------------------------------------------------------------------------------------
*/
/*! \var tFsmMenuIfs menuStartState
    \brief Menu Start state interface value. It happens mostly after initialization.  
*/
//tFsmMenuIfs menuStartState =
//{
//    menuStartChangeStateBefore,
//    menuStartChangeStateAfter,
//    menuStartEventHandler
//};

/*! \var tFsmMenuIfs menuNoneState 
    \brief None State definition
    
    Parent object for Menu FSM 
*/
tFsmMenuIfs menuNoneState =
{
    menuNoneChangeStateBefore,
    menuNoneChangeStateAfter,
    menuNoneEventHandler
};
