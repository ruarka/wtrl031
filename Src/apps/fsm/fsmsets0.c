/*! \file      fsmsets0.c
    \version   0.1  
    \date      2017-06-06 23:46
    \brief     Menu FSM state provides root management menu
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
#include "fsmedint.h"
#include "fsmsets0.h"
#include "fsmwatch.h"
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
 *                                 Global Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                 Local Variables
 * ------------------------------------------------------------------------------------------------
 */
/*! */
uint8_t uiSelectedItem;

uint8_t blBtnWasPressed;

/* ------------------------------------------------------------------------------------------------
 *                                          Functions
 * ------------------------------------------------------------------------------------------------
 */

#if 1

uint8_t menuYear, menuMonth, menuDay, menuWeekDay;
_tSysTime timeForUpdate;


extern void makeSaveSetLeafHandler( void );
extern void menuSetDate( void );
extern void menuSetTime( void );
extern void menuBeforeChangeDate( void );

extern void menuPrintYear( uint8_t row, char* pSBuff);
extern void menuPrintMonth(uint8_t row, char* pB );
extern void menuPrintDay(uint8_t row, char* pB );
extern void menuPrintSet( uint8_t row, char* pB );


tMenu Menu[]={
// Next       Prev        Parent   Child                                    R   Before Leaf Selected  Custom Prnt  M   M        Type              String
  {&Menu[1],  NULL,         NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,       "<-(Watch)"},  // 0
  {&Menu[2],  &Menu[0],     NULL,     NULL,                  &(settings.mode),                 NULL,         NULL, 0,  1,EnterUI8Val,            "Mode"},  // 1 Main Menu
  {&Menu[3],  &Menu[1],     NULL, &Menu[6],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,      "Management"},  // 2
  {&Menu[4],  &Menu[2],     NULL, &Menu[2],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,     "Time change"},  // 3
  {&Menu[5],  &Menu[3],     NULL,&Menu[22],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,     "Date change"},  // 4 - SetDate
  {NULL,      &Menu[4],     NULL, &Menu[2],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,        "Watering"},  // 5

  // Management State
  {&Menu[7],  NULL,         NULL, &Menu[0],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,          "<-Back"},  // 6
  {&Menu[8],  &Menu[6],     NULL,     NULL,             &(settings.blUsePump),                 NULL,         NULL, 0,  1,EnterUI8Val,        "Use Pump"},  // 7
  {&Menu[9],  &Menu[7],     NULL,     NULL,        &(settings.secondsToSleep),                 NULL,         NULL, 2,255,EnterUI8Val,       "Sec2Sleep"},  // 8
  {&Menu[10], &Menu[8],     NULL,     NULL,       &(settings.secondsWatering),                 NULL,         NULL, 0,255,EnterUI8Val,       "sWatering"},  // 9
  {&Menu[11], &Menu[9],     NULL,     NULL,       &(settings.blDisplayOnMode),                 NULL,         NULL, 0,  1,EnterUI8Val,         "On Disp"},  // 10
  {&Menu[12],&Menu[10],     NULL,&Menu[36],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,             "AOS"},  // 11
  {&Menu[13],&Menu[11],     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,             "AOT"},  // 12
  {&Menu[14],&Menu[12],     NULL,&Menu[18],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,         "Default"},  // 13
  {     NULL,&Menu[13],     NULL,&Menu[20],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,        "Save set"},  // 14

  // New lines are added - index correction is needed
  {     NULL,     NULL,     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,                ""},  // 15 New
  {     NULL,     NULL,     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,                ""},  // 16 New
  {     NULL,     NULL,     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,                ""},  // 17 New

  // Indexes are not valid from here
  // "Default" State
  {&Menu[19],     NULL,     NULL,&Menu[13],                              NULL,                 NULL,         NULL, 0,  0, SelectLeaf,            "NO "},  // 18
  {     NULL,&Menu[18],     NULL,&Menu[13],           assing_default_settings,                 NULL,         NULL, 0,  0,   MakeFunc,            "YES"},  // 19

  // "Save Set" State
  {&Menu[21],     NULL,     NULL,&Menu[14],                              NULL,                 NULL,         NULL, 0,  0, SelectLeaf,            "NO "},  // 20
  {     NULL,&Menu[20],&Menu[14],     NULL,            makeSaveSetLeafHandler,                 NULL,         NULL, 0,  0,   MakeFunc,            "YES"},  // 21

  // "Date change" State
  {&Menu[23],     NULL,     NULL, &Menu[4],                                 0, menuBeforeChangeDate,          NULL, 0,  0, SelectLeaf,         "<-Back"},  // 22
  {&Menu[24],&Menu[22],     NULL,     NULL,                         &menuYear,                 NULL, menuPrintYear, 18,25,EnterUI8Val,          "Year:"},  // 23
  {&Menu[25],&Menu[23],     NULL,     NULL,                        &menuMonth,                 NULL,menuPrintMonth, 1, 12,EnterUI8Val,          "Mnth:"},  // 24
  {&Menu[26],&Menu[24],     NULL,     NULL,                          &menuDay,                 NULL,  menuPrintDay, 1, 31,EnterUI8Val,           "Day:"},  // 25
  {     NULL,&Menu[25],     NULL,&Menu[27],                                 0,                 NULL,  menuPrintSet, 0,  0, SelectLeaf,            "Set"},  // 26

  // "Date change" State - Set
  {&Menu[28],     NULL,     NULL,&Menu[26],                              NULL,                 NULL,          NULL, 0,  0, SelectLeaf,            "NO "},  // 27
  {     NULL,&Menu[27],&Menu[26],     NULL,                       menuSetDate,                 NULL,          NULL, 0,  0,   MakeFunc,            "YES"},  // 28

  // "Time change" State
  {&Menu[30],     NULL, &Menu[3],     NULL,                                 0, menuBeforeChangeDate,          NULL, 0,  0, SelectLeaf,          "<-Back"}, // 29
  {&Menu[31],&Menu[29],     NULL,     NULL,            &(timeForUpdate.hours),                 NULL,          NULL, 0, 23,EnterUI8Val,           "Hour:"}, // 30
  {&Menu[32],&Menu[30],     NULL,     NULL,          &(timeForUpdate.minutes),                 NULL,          NULL, 0, 59,EnterUI8Val,            "Min:"}, // 31
  {&Menu[33],&Menu[31],     NULL,     NULL,          &(timeForUpdate.seconds),                 NULL,          NULL, 0, 59,EnterUI8Val,            "Sec:"}, // 32
  {&Menu[34],&Menu[32],     NULL,     NULL,                                 0,                 NULL,          NULL, 0,  0, SelectLeaf,             "Set"}, // 33

  // "Time change" State - Set
  {&Menu[35],     NULL,     NULL,&Menu[33],                              NULL,                 NULL,          NULL, 0,  0, SelectLeaf,            "NO "},  // 34
  {     NULL,&Menu[34],&Menu[33],     NULL,                       menuSetTime,                 NULL,          NULL, 0,  0,   MakeFunc,            "YES"},  // 35

  // AOS
  {&Menu[37],     NULL,     NULL,&Menu[11],                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,          "<-Back"},  // 36
  {&Menu[38],&Menu[36],     NULL,     NULL,&(settings.uiStartSensorThreshold),                 NULL,         NULL, 0,  9,EnterUI8Val,        "StartVal"},  // 37
  {     NULL,&Menu[37],     NULL,     NULL, &(settings.uiStopSensorThreshold),                 NULL,         NULL, 0,  9,EnterUI8Val,         "StopVal"},  // 38
  {     NULL,     NULL,     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,                ""},  // 39
  {     NULL,     NULL,     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,                ""},  // 40
  {     NULL,     NULL,     NULL,     NULL,                                 0,                 NULL,         NULL, 0,  0, SelectLeaf,                ""},  // 41

  // AOT

};

tMenu* curr = NULL;


/////////////////////////////////////////////////////////
// Menu Handlers
void makeSaveSetLeafHandler( void )
{
  // settings_flush();
  flushSettings();
}

void menuSetDate( void )
{
  setDateForRTC( menuYear,  menuMonth, menuDay, menuWeekDay);
}

void menuBeforeChangeDate( void )
{
  getDateFromRTC( &menuYear, &menuDay, &menuMonth,  &menuWeekDay );
}

void menuSetTime( void )
{
  fwkSetTime( &timeForUpdate );
  
  setTimeToRTC( timeForUpdate.hours, 
                timeForUpdate.minutes,
                timeForUpdate.seconds );
}

void menuPrintYear(uint8_t row, char* pB )
{
  sprintf(  pB, "Year: %d", menuYear );
}

void menuPrintMonth(uint8_t row, char* pB )
{
  sprintf(  pB, "Mnth: %d", menuMonth );
}

void menuPrintDay(uint8_t row, char* pB )
{
  sprintf(  pB, "Day: %d", menuDay );
}

void menuPrintSet( uint8_t row, char* pB )
{
  sprintf( pB, "Set %d:%d:%d", menuYear, menuMonth, menuDay );
}

void printMenuEx( void )
{
  pfnMenuCustomPrint pF; // =(pfnMenuPrintLeaf) newLeafMenu->;

  if( curr->Previous )
  {
    pF =( pfnMenuCustomPrint )curr->Previous->fnMenuCustomPrint;
    if( pF )
    {
      pF( 0, pDisplayStrBuff );
      dispDrawStrN( 0, 0, pDisplayStrBuff );
    }
    else
      dispDrawStrN( 0, 0, curr->Previous->Name);
  }else
    dispSetLineCh(0, ' ');

  if( curr->Next )
  {
    pF =( pfnMenuCustomPrint )curr->Next->fnMenuCustomPrint;
    if( pF )
    {
      pF( 2, pDisplayStrBuff );
      dispDrawStrN( 2, 0, pDisplayStrBuff );
    }
    else
      dispDrawStrN( 2, 0, curr->Next->Name);
  }else
    dispSetLineCh(2, ' ');

  /* Printing sel string  */
  pF =( pfnMenuCustomPrint )curr->fnMenuCustomPrint;
  if( pF )
  {
    pF( 1, pDisplayStrBuff );
    dispDrawStrN( 1, 0, pDisplayStrBuff );
  }
  else
    dispDrawStrN( 1, 0, curr->Name);

  dispRedraw();
}

void changeApplMenuLeaf( tMenu* newLeafMenu )
{
  // const
  pfnMenuBeforeLeaf pF;

  if(  newLeafMenu  ){
    DBGT( LOG_DEBUG, "FSM:MNL:%s->%s", curr->Name, newLeafMenu->Name );

    pF =(pfnMenuBeforeLeaf) newLeafMenu->fnMenuBeforeLeaf;

    // Call action before switch to new menu leaf
    if( pF )
      pF();

  }else{
    DBGT( LOG_DEBUG, "FSM:MNL:%s->NULL(Watch)", curr->Name );
  }

//  const pfnMenuBeforeLeaf pF =(pfnMenuBeforeLeaf) newLeafMenu->fnMenuBeforeLeaf;

  switch (curr->subMenuType)
  {
    case SelectLeaf:
      if( newLeafMenu )
      {
          curr = newLeafMenu;
          printMenuEx();
          return;
      }
      else
      {
        /* Selected Leaf NULL - change state to watch */
        applMenuChangeStateEx(&menuWatchState);
        curr = NULL;
      }
      break;

    case EnterUI8Val:
    case EnterUI16Val:
    case EnterUI32Val:
      applMenuChangeStateEx( &menuEdIntState );
      break;

    case MakeFunc:
      {
        const pfnMenuMake pH =(pfnMenuMake)( curr->pReturnVal);
        if( pH != NULL )
             pH();
        curr = curr->Parent;
        printMenuEx();
        break;
      }

//    case EnterUIOnOff:
//    case EnterUIYesNo:
    default:
      /* Do watch state if nothing defined */
      applMenuChangeStateEx(&menuWatchState);
      break;
  }

  return;
}

void menuSets0ChangeStateBefore( tFsmMenuState newState )
{
  DBGT( LOG_DEBUG, "FSM:Sets0:Before");

  if( !curr )
    curr = &Menu[0];

  /* Draw menu screen */
  printMenuEx();
}

void menuSets0ChangeStateAfter(tFsmMenuState newState)
{
  DBGT( LOG_DEBUG, "FSM:Sets0:After" );
}

uint8_t menuSets0EventHandler(_tEQ* p)
{
    DBGT( LOG_DEBUG, "FSM:Sets0:EH-%d:%d", p->eId, p->reserved );
  
    switch (p->eId)
    {
      case EV_APP_LEFT:
        if( curr->Previous )
        {
            curr = curr->Previous;
            printMenuEx();
        }
        break;


      case EV_APP_RIGHT :
        if( curr->Next )
        {
            curr = curr->Next;
            printMenuEx();
        }
        break;

      case EV_APP_BTN_ACT:
      {
        if( p->reserved  )
          changeApplMenuLeaf( curr->Child );
        break;
      }

      default:
//        applMenuChangeState(none);
        break;
    }

    return 0x00;
}
#endif

/*! \var tFsmMenuIfs menuWatchState
    \brief ROOT Menu Management state interface value.

 */
tFsmMenuIfs menuSets0State =
{
    menuSets0ChangeStateBefore,
    menuSets0ChangeStateAfter,
    menuSets0EventHandler
};
