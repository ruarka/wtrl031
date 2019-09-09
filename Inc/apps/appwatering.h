/*! \file      appwatering.h
    \version   
    \date      2017-07-18 18:25
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
#define WTR_IDLE_TIME                 10    /**< Seconds before watering process is initiated                 */
#define WTR_PWRON_STAB_TIME           5     /**< Seconds of power stabilization                               */
#define WTR_ADC_WATERING_SAMPLES      5     /**< The number of ADC sampling to understand watering is needed  */
#define WTR_ADC_STOP_WATERING_SAMPLES 5     /**< The number of ADC sampling to contorol flood                 */
#define WTR_WATERING_START_SHD        1500  /**< The value of humidity sensor 0 recoginized as dry            */
#define WTR_WATERING_TIME             10    /**< Seconds of watering                                          */
#define WTR_WATERING_STOP_SHD         600   /**< The value of humidity sensor 1 recoginized as overwatering   */
#define WRT_STOP_INDICATION_TIME      5     /**< Seconds of End indication                                    */
#define WRT_ERRSTOP_INDICATION_TIME   10    /**< Seconds error indication                                      */

#define WRT_ERR_LED           0


#define ACD_CHNL_START				ADC_CHANNEL_0   /**< ADC chanel for sensor 0 - dry testing   */
#define ACD_CHNL_STOP					ADC_CHANNEL_1   /**< ADC chanel for sensor 1 - overwatering  */

#define ADC_SAMPLING_TO				10

 
 /* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */
/**
	 Watering application states
 */
enum {
	WTR_NONE = 0,  			/**< After reset                          */
	WTR_IDLE,						/**< Wait until stabilization             */
	WTR_PWRON,					/**< Humidity sensor is on service        */
	WTR_CHECK_FOR_WRT,	/**< Gathering info from humidity sensor  */
	WTR_WATERING,				/**< Watering process in progress         */
	WTR_STOP,						/**< Stop watering before goes to sleep   */
	WTR_STOP_ERR,				/**< Stop with error                      */
  WTR_WAIT_SLEEP      /**< Ready to sleep                       */
};
 
 /* ------------------------------------------------------------------------------------------------
 *                                    Functions declarations
 * ------------------------------------------------------------------------------------------------
 */
extern uint8_t appWateringEvHandler(_tEQ* p);
