/*! \file      settings.h
    \version   0.1
    \date      2017-06-08 22:01
    \brief     <A Documented file.>
    \details   <Details.>
    \bug       
    \copyright  
    \author    
*/
/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */
 
 /* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */
 
 /* ------------------------------------------------------------------------------------------------
 *                                    Functions declarations
 * ------------------------------------------------------------------------------------------------
 */
/* ------------------------------------------------------------------------------------------------
*                                     Macros
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
*                                 Type definitions
* ------------------------------------------------------------------------------------------------
*/

/*!

 */
enum {
  MODE_AOT = 0,   /**< Action On Timer mode   */
  MODE_AOS,       /**< Action On Sensor mode  */
  MODE_COUNT
};

/* ------------------------------------------------------------------------------------------------
*                                    Functions declarations
* ------------------------------------------------------------------------------------------------
*/

/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */
#define SETTINGS_STRUCT_OFFSET    	0x00    																		  /* Base offset for settings structure */
	
#define SETTINGS_SECONDS_TO_SLEEP		SETTINGS_STRUCT_OFFSET 											  /* */

#define SETTINGS_USE_PUMP						SETTINGS_SECONDS_TO_SLEEP +sizeof(uint32_t)	  /* */

#define SETTINGS_WATERING_SECONDS		SETTINGS_USE_PUMP +sizeof(uint32_t)					  /* */
	
#define SETTINGS_DISPLAY_ON_OFF     SETTINGS_WATERING_SECONDS +sizeof(uint32_t)	  /*  */

#define SETTINGS_DAILY_TIMER_SCHED	SETTINGS_DISPLAY_ON_OFF +sizeof(uint32_t)	    /* ON/OFF Display power mode */

#define SETTINGS_CRC								SETTINGS_DAILY_TIMER_SCHED +sizeof(uint32_t)*7*3 /* */

/* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */
typedef struct _settings
{
  uint8_t     			secondsToSleep;
  uint8_t        		blUsePump;
  uint16_t    			secondsWatering;
  uint16_t    			minutesWateringGap;
//		tCalendarDateTime	dailyTimerSchedule[ 7 ]; // 7 days per week
  uint8_t           blDisplayOnMode;
  uint8_t           mode;
  uint8_t           uiStartSensorThreshold;
  uint8_t           uiStopSensorThreshold;
  uint32_t					crc;
} tSettings;

/* ------------------------------------------------------------------------------------------------
 *                                    Functions
 * ------------------------------------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C"
{

  tSettings settings;

//  uint32_t settings_load( void );
  
  void loadSettings( void );

//  uint8_t settings_flush( void );

  void flushSettings( void );
  
  tSettings* getSettings( void );

  void assing_default_settings(void);

}
#else
extern tSettings settings;

// extern uint32_t settings_load( void );

extern void loadSettings( void );

// extern uint8_t settings_flush( void );
extern void flushSettings( void );

extern void flushSettings( void );

extern tSettings* getSettings( void );

extern void assing_default_settings(void);

#endif

// extern void assing_default_settings(void);

