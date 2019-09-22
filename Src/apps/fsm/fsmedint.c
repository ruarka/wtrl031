/*! \file      fsmedint.c
    \version   0.1  
    \date      2017-06-09 22:39
    \brief     <A Documented file.>
		\author     ruarka

    \bug       
		
    \details   
*/
/* ------------------------------------------------------------------------------------------------
 *                                 Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <stdint.h>
#include <string.h>

#include "apptypes.h"

#include "events.h"
#include "eventq.h"
#include "fsmappl.h"
#include "fsmbasic.h"
#include "fsmedint.h"
#include "fsmsets0.h"
#include "fsmwatch.h"
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
 *                                 Global Variables
 * ------------------------------------------------------------------------------------------------
 */

 /* ------------------------------------------------------------------------------------------------
  *                                 Local Variables
  * ------------------------------------------------------------------------------------------------
  */
uint32_t uiMinVal, uiMaxVal;
uint8_t blBackSelected;
uint8_t blPressed;

static void menuIntPrint(void)
{
  char pBuff[ 32 ]; // TODO change to the actual len 16-17 - because of warning
  uint32_t uiPrintedVal;

  switch (curr->subMenuType)
  {
    case EnterUI8Val:
      {
        uint8_t* pTmp = (uint8_t*)curr->pReturnVal;
        uiPrintedVal = *pTmp;
        break;
      }

    case EnterUI16Val:
      {
        uint16_t* pTmp = (uint16_t*)curr->pReturnVal;
        uiPrintedVal = *pTmp;
        break;
      }
    case EnterUI32Val:
      {
        uint32_t* pTmp = (uint32_t*)curr->pReturnVal;
        uiPrintedVal = *pTmp;
        break;
      }
    case EnterUIOnOff:
      {
        uint8_t* pTmp = (uint8_t*)curr->pReturnVal;
        uiPrintedVal = *pTmp;
        if( *pTmp )
        {
          pBuff[ 0 ]= 'Y';
          pBuff[ 1 ]= 'E';
          pBuff[ 2 ]= 'S';
          pBuff[ 3 ]= 0x00;
        }
        else
        {
            pBuff[ 0 ]= 'N';
            pBuff[ 1 ]= 'O';
            pBuff[ 2 ]= ' ';
            pBuff[ 3 ]= 0x00;
        }
        break;
      }

    default:
      /* Do watch state if nothing defined */
      applMenuChangeStateEx(&menuWatchState);
      return;
  }

  if( curr->subMenuType != EnterUIOnOff )
  {
    sprintf(pBuff, "%s %d", curr->Name, uiPrintedVal );
    dispDrawStrN( 1, 0, pBuff);
  }
}

static void increaseVal( void )
{
  uiMaxVal = curr->maxVal;
  uiMinVal = curr->minVal;
  
  switch (curr->subMenuType)
  {
    case EnterUI8Val:
      {
        uint8_t* pTmp = (uint8_t*)curr->pReturnVal;

        if( *pTmp < uiMaxVal )
          (*pTmp)++;

        break;
      }

    case EnterUI16Val:
      {
        uint16_t* pTmp = (uint16_t*)curr->pReturnVal;

        if( *pTmp < uiMaxVal )
          (*pTmp)++;

        break;
      }
    case EnterUI32Val:
      {
        uint32_t* pTmp = (uint32_t*)curr->pReturnVal;

        if( *pTmp < uiMaxVal )
          (*pTmp)++;

        break;
      }
    default:
      /* Do watch state if nothing defined */
      applMenuChangeStateEx(&menuWatchState);
      break;
  }
}

static void decreaseVal( void )
{
  uiMaxVal = curr->maxVal;
  uiMinVal = curr->minVal;
  
  switch (curr->subMenuType)
  {
    case EnterUI8Val:
      {
        uint8_t* pTmp = (uint8_t*)curr->pReturnVal;

        if( *pTmp > uiMinVal )
          (*pTmp)--;

        break;
      }

    case EnterUI16Val:
      {
        uint16_t* pTmp = (uint16_t*)curr->pReturnVal;

        if( *pTmp > uiMinVal )
          (*pTmp)--;

        break;
      }
    case EnterUI32Val:
      {
        uint32_t* pTmp = (uint32_t*)curr->pReturnVal;

        if( *pTmp > uiMinVal )
          (*pTmp)--;

        break;
      }
    default:
      /* Do watch state if nothing defined */
      applMenuChangeStateEx(&menuWatchState);
      break;
  }
}

void menuEdIntChangeStateBefore(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:EdInt:Before" );

    /* Print assigned value with name */
    dispSetLineCh( 0, ' ');
    dispSetLineCh( 2, ' ');

    blBackSelected = 0;
    blPressed = 0;

    menuIntPrint();

    dispRedraw();
}

void menuEdIntChangeStateAfter(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:EdInt:After" );
}

uint8_t menuEdIntEventHandler(_tEQ* p)
{
  DBGT( LOG_DEBUG, "FSM:EdInt:EH-%d:%d", p->eId, p->reserved );

    switch (p->eId)
    {
    case EV_APP_INIT:
        applMenuChangeStateEx(&menuWatchState);
        break;

    case EV_APP_SEC_TICK:
        break;

    case EV_APP_LEFT:
      if( blPressed )
      {
          decreaseVal();
          menuIntPrint();
      }
      else
      {
        if( !blBackSelected )
        {
//            dispSetLineCh( 1, ' ');
            dispDrawStrN( 1, 0, "Back");
            blBackSelected = 0x01;
        }
      }
      dispRedraw();
      break;

    case EV_APP_RIGHT:
      if( blPressed )
      {
          increaseVal();
          menuIntPrint();
      }
      else
      {
          if( blBackSelected )
          {
              menuIntPrint();
              blBackSelected = 0x00;
          }
      }
      dispRedraw();
      break;

    case EV_APP_BTN_ACT:

      blPressed =( p->reserved )? 0x00 : 0x01;

      // Button was left
      if(( blBackSelected )
          && !blPressed )
      {
          // Return back to parent menu
          // curr = curr->Parent;
          applMenuChangeStateEx( &menuSets0State );
      }


      break;

    case EV_APP_UP:
    case EV_APP_DOWN:
    case EV_APP_IDLE:
    case EV_APP_SLEEP:
        // TODO: Handle EV_APP_IDLE, EV_APP_SLEEP
    default:
//        applMenuChangeState(none);
        break;
    }

    return 0x00;
}

/* ------------------------------------------------------------------------------------------------
*                                 Global Variables
* ------------------------------------------------------------------------------------------------
*/
/*! \var tFsmMenuIfs menuEdIntState
    \brief Menu Tc state interface value. It happens mostly after initialization.

    <Details.>
*/
tFsmMenuIfs menuEdIntState =
{
    menuEdIntChangeStateBefore,
    menuEdIntChangeStateAfter,
    menuEdIntEventHandler
};
