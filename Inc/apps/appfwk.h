/*! \file      appfwk.h
    \version   0.1
    \date      2017-05-18 11:55
    \brief     header for application framework 
    \author    ruarka
*/

#include "globals.h"

/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */
/*! \var DISPLAY_LINES_NUMBER
    \brief Lines number for display
 */ 
#define DISPLAY_LINES_NUMBER            3 

/*! \var APP_LEDS_NUMBER
    \brief Number of LEDS in defined in Application

    <Details>.
*/
#define APP_LEDS_NUMBER                 3

/*! \var DISPLAY_COLUMNS_NUMBER
    \brief Columns number for display
 */
#define DISPLAY_COLUMNS_NUMBER          16

#define RTC_SYSTEM_TIME_UPDATE_GAP      15 		/**< Period to update sytem time from RTC */

/*! \var APP_SLEEP_MAX_SLEEPING_TIME_SEC
    \brief <A type definition for a .>
 */
//#define APP_SLEEP_MAX_SLEEPING_TIME_SEC 20   /* 20 seconds - for testing seconds */
#define APP_SLEEP_MAX_SLEEPING_TIME_SEC 120   /* It should wake up after 120 s  */


/* ------------------------------------------------------------------------------------------------
*                                 Type definitions
* ------------------------------------------------------------------------------------------------
*/

typedef  uint8_t (*pfnAppInit)(void);

/*! \var typedef const bool(*const pfnAppEventHandler)(_tEQ* p)
    \brief Type of Application Event Handler Function definition
*/
typedef uint8_t(*pfnAppEventHandler)(_tEQ* p);

/**
 Framework application interface
*/

typedef struct _application_event_link
{
    const uint16_t event;
    const pfnAppEventHandler  * pEventHandlersList;
} _tAppEventLink;

/*! \struct _sys_time
    \brief Application time structure type

    _sys_time represent Application time structure and keeps time from reset which is used
    for debug and other messages instead of permanent requests to RTC.
    _sys_time was introduced to economy mcu resources against RTC reading when accuracy
    is not strong required.
    _sys_time contents time from MCU reset until time it is not restored from RTC.
*/
/*! \var typedef struct _sys_time  _tSysTime
    \brief Done for easy use of _sys_time
*/
typedef struct _sys_time 
{
    uint8_t 	hours;
    uint8_t 	minutes;
    uint8_t 	seconds;
    uint32_t 	utcSeconds;
} _tSysTime;

/* ------ Display Support functions ------ */
/*! \var    struct _display_entity tDisplayEntity
    \brief  Character object for Abstract application display
    
    There is an abstraction layer for display in application fwk. 
    Display character also has a boolean flag attribute blWasupdated 
    which is used to understand do hw display needs to update the symbol. 
*/
typedef struct _display_entity
{
  char      pEntityVal;   /**< Character */
  uint8_t   blWasupdated; /**<  Boolean flag character was updated */ 
} tDisplayEntity;

///**
// * A structure to represent 3d vectors 
// */
//typedef struct
//{
//  /*@{*/
//  double x ; /**< the x coordinate */
//  double y ; /**< the y coordinate */
//  double z ; /**< the z coordinate */
//  /*@}*/
//  /**
//   * @name group 2
//   */
//  /*@{*/
//  char * name       ; /**< the name of the point */
//  int    namelength ; /**< the size of the point name */
//  /*@}*/
//} point3d ;

typedef enum 
{
  ByReset = 0,
  ByWakeUp
} enStartCondition;

typedef struct _appl_context
{
  enStartCondition            startConditions;
  uint8_t                     blDateTimeValid;
  tSettings                   *eppromSettings;
} tApplicationContext;
  
/* ------------------------------------------------------------------------------------------------
*                                    Functions declarations
* ------------------------------------------------------------------------------------------------
*/
extern tDisplayEntity dsplDataScreen[DISPLAY_LINES_NUMBER][DISPLAY_COLUMNS_NUMBER];

extern void fwkLedSet( uint8_t uiLedId, uint8_t ontime, uint8_t offTime, uint8_t blPeriodic );
extern void fwkLedOn( uint8_t uiLedId );
extern void fwkLedOff( uint8_t uiLedId );
extern void fwkLedBlink( uint8_t uiLedId, uint8_t ontime, uint8_t offTime );

extern uint8_t fwkLedsProcessing(_tEQ* p);

extern void fwkAppEventLoop(_tEQ* pE);

extern void fwkSendEventToAppDirectly( uint8_t appId, _tEQ* pEv );

extern _tSysTime fwkSystemTime;

extern tApplicationContext applContext;

extern void fwkGetTime(_tSysTime* pSystemTime);
extern void fwkSetTime(_tSysTime* pSystemTime);

//extern void fwkSleepActivityUpdate( uint16_t seconds );
extern uint8_t fwkCheckForWatering(void);
extern void fwkSleepCalcWakeupTime(uint8_t* almWeekday, uint8_t* almHour, uint8_t* almMinute, uint8_t* almSeconds);

/* ------ Display Support functions ------ */
extern void dispRedraw(void);
extern void dispClear(void);
extern void dispSetChar(uint8_t row, uint8_t column, char ch);
extern void dispSetLineCh(uint8_t row, char ch);
extern void dispDrawStr(uint8_t row, uint8_t col, const char* const pcStr);
extern void dispDrawStrN(uint8_t row, uint8_t col, const char* const pcStr);
// extern uint8_t dispCheckStr(uint8_t row, const char* const pcStr);
extern char* dispGetString(uint8_t row, char* pBuff);

#ifdef __cplusplus
extern "C"
{
#endif

void fwkAppInit(void);
void fwkMain(uint8_t blDoContinuesLoop);
uint8_t dispCheckStr(uint8_t row, const char* const pcStr);

#ifdef __cplusplus
}
#endif
