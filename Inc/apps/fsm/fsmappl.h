/*! 
Filename:       fsmappl.h
Revised:        2017-04-24 13:13
Revision:

Description:    Final State Machine(FSM) application facade interface decleration.
                Here Application handlers are declared which collected in peered file. 
                
    \file      fsmappl.h
    \version   0.1
    \date      2017-04-24 13:13
    \brief     <A Documented file.>
    
    \details   Final State Machine(FSM) application facade interface decleration.
                Here Application handlers are declared which collected in peered file. 

    \copyright  
    \author   ruarka       
    \bug       

*/

/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
#define INACTIVITY_TIME_DETECTION         20

#define DSPL_WATERING_MINUTES_ROW         1
#define DSPL_WATERING_MINUTES_COL         9

#define DSPL_WATERING_APP_STATE_AREA_ROW  2

#define DSPL_CHARGE_LVL_ROW               1
#define DSLP_CHARGE_LVL_COL               14

#define APP_STATE_DISPLAY_STRING_LEN      17
#define APP_STATE_AREA_DISPLAY_STR_NUM    10

/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
typedef enum enumFsmMenuType
{
  SelectLeaf,
  EnterUI8Val,
  EnterUI16Val,
  EnterUI32Val,
  EnterUIOnOff,
  EnterUIYesNo,
//  SetTime,
//  SetDate,
  MakeFunc,
  None
} tFsmMenueType;


typedef const struct _fsm_menu_enter_ui8_item
{
  const char* pMenuStr;
  const uint8_t     minVal;
  const uint8_t     maxVal;
  const uint8_t*    pValDst;
  const void* pReturnState;
} tFsmMenuEnterUI8Item;

typedef const struct _fsm_menu_select_item
{
  const char* pMenuItemStr;
  const void* pSelectState;
  tFsmMenueType menuLeafType;
} tFsmMenuSelectItem, *pFsmMenuSelectItem;

typedef const struct _fsm_menu_select
{
  const char* pStateName;
  tFsmMenuSelectItem* pItems;
}tFsmMenuSelect;

//typedef const struct _fsm_menu_enter_ui8_val
//{
//  const tFsmMenuSelectItem* pMenuItemsStrs[];
//
//} tFsmMenuEnterUI8Val;

/** Defined a set of states for application FSM */
typedef enum enumFsmMenuState {
    start,					      /**< Start            */
    watch,					      /**< Watch wnd        */
    deep_sleep,				    /**< Stop             */
    watering,				      /**< Watering wnd     */
    menu_settings0,			  /**< Settings0        */
    menu_management,		  /**< management       */
    menu_watering,			  /**< watternig        */
    menu_timecorrection,	/**< Time correction  */
    menu_datecorrection,  /**< Date correction  */
    menu_editorUP,			  /**< Redactor         */
    menu_editorWS,
    menu_schedule,        /**< schedule menu    */
    menu_scheduleDay,     /**< Day schd menu    */
    menu_editorS2S,
    menu_def_sets,
    menu_save_settings,   /**< store configured settings   */
    none                  /**< Error or something unwaited */
} tFsmMenuState;

/** This is an interface to FSM module description */
typedef const struct _fsm_menu_interface
{
    void(*const pfnMenuChangeStateBefore)(tFsmMenuState newState);
    void(*const pfnMenuChangeStateAfter)(tFsmMenuState newState);
    uint8_t(*const pEventHandler)(_tEQ* p);
} tFsmMenuIfs;


/*
 * Application State Display Area interface
 */
typedef struct _app_state_display_area_entity
{
  char      appMsg[ APP_STATE_DISPLAY_STRING_LEN ];
  uint8_t   uiTimeToDisplay;
} tAppStateDisplayAreaEntity;

typedef struct _app_state_display_area_buf
{
  tAppStateDisplayAreaEntity strings[ APP_STATE_AREA_DISPLAY_STR_NUM ];
  uint8_t wrPtr;
  uint8_t rdPtr;
} tAppStateDisplayAreaBuf;


/* ------------------------------------------------------------------------------------------------
 *                                        Memory Attributes
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                        Externals Defines
 * ------------------------------------------------------------------------------------------------
 */
extern const char pIntToHex[];

extern uint8_t  blMenuActive;

extern uint8_t appMenuEvHandler(_tEQ* p);

extern void applMenuChangeStateEx( tFsmMenuIfs * newState );

extern void appPumpOnOff(uint8_t PumpState);

extern const tFsmMenuSelect sMenuSettingS0;

extern char* pAppStateDisplayString;

/*
 * Application State Display Area interface
 */
extern tAppStateDisplayAreaBuf appStateDislayArea;

extern void initAppStateDisplayArea(void);

extern void addAppStateDisplayArea( char* pMsgStr, uint8_t timeToDisplay );

extern void getAppStateDisplayArea( char* pReadBuf, uint8_t* pTomeToDisplay );
