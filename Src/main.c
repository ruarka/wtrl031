/* USER CODE BEGIN Header */
/*!
  \file       main.c
  \version    0.1
  \date       2019-05-09 14:19
  \author     ruarka
    
  \brief      Main MCU file. All hardeware definitions done here.
  
  @verbatim 
  ====================================
  PINS for LQFP32 32l031

  AIN0 	- PA0 	  - analog        Humidity control
  AIN1 	- PA1 	  - analog        Watering control
  GPIO 	- PB4 	  - dig/out       Power supply domain management
  GPIO 	- PB5 	  - dig/out       Pump management 
  LED_APP - PB3  	- dig/out       Application Led
  I2C1 	- PA9 	  - I2C1_SCL     	LCD/HD44 Display
  I2C1 	- PA10 	  - I2C1_SDA     	LCD/HD44 Display
  USART	- PA2 	  - USART2_TX  	  Debug COM
  USART	- PA15	  - USART2_RX    	Debug COM
  ENC1 	- PA3	    - DT Enc        Encoder TIM CH2
  ENC2 	- PB6 	  - CLK Enc       Encoder TIM CH1
  ENCB	- PA12 	  - Btn Enc       Encoder Button GPIO_Ext 
  GPIO  - PB7     - dig/out       LCD Power management
  
  @endverbatim
    
  \bug
  
  

//  moveXY(&scr,4,0);
//  writeStringLCD(&scr,"Hello");

//  moveXY(&scr,3,1);
//  writeStringLCD(&scr,"LCD User!!! I`m working now :)");

  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "apptypes.h"

#include "events.h"
#include "eventq.h"
#include "settings.h"
#include "appfwk.h"

#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

// TODO: Investigate why ADC is too slow just 2 times per second
ADC_HandleTypeDef hadc;
/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef	sConfig;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

TIM_HandleTypeDef htim21;

LCD1602 scr;

/* USER CODE BEGIN PV */
RTC_DateTypeDef 				sDateStructureGet;
RTC_TimeTypeDef 				sTimeStructureGet;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM21_Init(void);
static void initApplContext(void);


uint8_t hwkWriteWordToEeprom( uint16_t base_offset, uint32_t word );

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void initApplContext(void)
{
  applContext.eppromSettings = getSettings();
  
  /* Check and handle if the system was resumed from StandBy mode */ 
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_WU) != RESET)
  {
    /* Clear WakeUp flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU); 
		
		// Silent wakeup
    applContext.startConditions = ByWakeUp;
  }
	else
	{
		 applContext.startConditions = ByReset;
	}	 
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  scr.hi2c = &hi2c1;
  scr.DevAddress = (0x27<<1);
  
  // Reset EQ
  eqReset();

  // Just for testing purposes assign default settings
  // assing_default_settings();
  loadSettings();

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Application context init */
  initApplContext();

/* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

#if 1  
  if(( applContext.startConditions == ByReset )
    ||( getSettings()->blDisplayOnMode ))
  {
    // TODO Up power domain and LCD Power here
    setLCDControl( 0x00 ); // On  - Inverse
  }
  else
    setLCDControl( 0x01 ); // Off - Inverse
#endif

  HAL_Delay( 1000 ); 
  
  if(( applContext.startConditions == ByReset )
    ||( getSettings()->blDisplayOnMode ))
  {
    MX_I2C1_Init();
    initLCD(&scr);     
  }
  
  MX_ADC_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_TIM21_Init();
  
  /* USER CODE BEGIN 2 */
  /* Apps Initialization */
	fwkAppInit();

  /* USER CODE END 2 */

  HAL_TIM_Encoder_Start_IT( &htim21, TIM_CHANNEL_ALL);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* Infinite loop */
  fwkMain( 0x01 );
}

// ------------------------- LCD Begin -------------------------------------
void deviceRedraw( void )
{
    uint8_t colCursorPointer = 255;
  
  if(( applContext.startConditions == ByReset )
    ||( getSettings()->blDisplayOnMode ))
  { 
    for (uint8_t j = 0; j<DISPLAY_COLUMNS_NUMBER; j++)
    {
      if( dsplDataScreen[1][j].blWasupdated )
      {
        if( j != colCursorPointer )
          moveXY(&scr,j,0);
        writeLCD(&scr, dsplDataScreen[1][j].pEntityVal);
        colCursorPointer = j+1;
      }
    }
    
    colCursorPointer = 255;
    for (uint8_t j = 0; j<DISPLAY_COLUMNS_NUMBER; j++)
    {
      if( dsplDataScreen[2][j].blWasupdated )
      {
        if( j != colCursorPointer )
          moveXY(&scr,j,1);
        writeLCD(&scr, dsplDataScreen[2][j].pEntityVal);
        colCursorPointer = j+1;
      }
    }
  }
}
// ------------------------- LCD End -------------------------------------

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode        = DISABLE;
  hadc.Init.Resolution              = ADC_RESOLUTION_12B;  

//  hadc.Init.ClockPrescaler          = ADC_CLOCK_SYNC_PCLK_DIV2;
//  hadc.Init.SamplingTime            = ADC_SAMPLETIME_1CYCLE_5;
//  hadc.Init.ScanConvMode            = ADC_SCAN_DIRECTION_FORWARD;

  hadc.Init.ClockPrescaler          = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.SamplingTime            = ADC_SAMPLETIME_7CYCLES_5;
  hadc.Init.ScanConvMode            = ENABLE;

  hadc.Init.DataAlign               = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode      = DISABLE;
  hadc.Init.DiscontinuousConvMode   = DISABLE;
  hadc.Init.ExternalTrigConvEdge    = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv        = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests   = DISABLE;
  hadc.Init.EOCSelection            = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun                 = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait        = DISABLE;
  hadc.Init.LowPowerFrequencyMode   = DISABLE;
  hadc.Init.LowPowerAutoPowerOff    = DISABLE;
  
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* ### - 2 - Start calibration ############################################ */
  if (HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }
  
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

void setADCChanel( uint32_t cnl, uint32_t OnOff )
{
  if( OnOff )
  {
    /* On channel */ 
    sConfig.Channel = cnl; 
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
      Error_Handler();
    }    
  }
  else
  {
    /* Off channel 1 */ 
    sConfig.Channel = cnl; 
    sConfig.Rank = ADC_RANK_NONE;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
      Error_Handler();  
    }
  }
}

void startADCConversion( void )
{
  /* Start the conversion process */  
  if (HAL_ADC_Start_IT(&hadc) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }  
}

void stopADCConversion( void )
{

}

/* Var for ADC Scan Event */
_tEQ eqAdcScan = { EV_ADC_SCAN, 00 };

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1)
{

  uint32_t ADC_Data = HAL_ADC_GetValue(hadc1);
    /* Add adc scan event into queue */
  eqAdcScan.reserved =  ADC_Data;
  eqAddEvent( &eqAdcScan );
}

// ------------------------- I2C ----------------------------------------
/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance                = I2C1;
  hi2c1.Init.Timing             = 0x2000090E;
  hi2c1.Init.OwnAddress1        = 0;
  hi2c1.Init.AddressingMode     = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode    = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2        = 0;
  hi2c1.Init.OwnAddress2Masks   = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode    = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode      = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

// ------------------------- RTC ----------------------------------------

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /** Initialize RTC Only 
  */
  hrtc.Instance               = RTC;
  hrtc.Init.HourFormat        = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv      = 127;
  hrtc.Init.SynchPrediv       = 255;
  hrtc.Init.OutPut            = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap       = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity    = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType        = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* Check DateTime was configured */
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != RTC_BKP_TIME_DATE_SET_FLAG)
  {
		/* RTC was released */
		applContext.blDateTimeValid = 0x00;
  }
	else
	{
		/* RTC also is valid */
		applContext.blDateTimeValid  = 0x01;
  }  
  
  /* USER CODE BEGIN Check_RTC_BKUP */
  /* Configure RTC Calendar if need */
  if( applContext.blDateTimeValid )
    return;
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  if( applContext.startConditions == ByReset )
  {
    sTime.Hours           = 11;
    sTime.Minutes         = 40;
    sTime.Seconds         = 12;
    sTime.DayLightSaving  = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation  = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)  
    {
      Error_Handler();
    }
    
    sDate.WeekDay         = RTC_WEEKDAY_FRIDAY;
    sDate.Month           = RTC_MONTH_MAY;
    sDate.Date            = 10;
    sDate.Year            = 19;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
      Error_Handler();
    }
      
#ifdef UNDER_DEV    
    // Set flag - Time is configured
    // Do not drop TimeDate flag by reset for debuging process
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BKP_TIME_DATE_SET_FLAG);
#endif
  }    
}

uint32_t blAlarmWasConfigured = 0;

/**
  * @brief  Alarm callback
  * @param  hrtc : RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	blAlarmWasConfigured = 0x00;
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
}

void hwSleepingInitiate(void)
{
	if( !blAlarmWasConfigured )
	{
		/* The Following Wakeup sequence is highly recommended prior to each Standby mode entry
			mainly  when using more than one wakeup source this is to not miss any wakeup event.
			- Disable all used wakeup sources,
			- Clear all related wakeup flags, 
			- Re-enable all used wakeup sources,
			- Enter the Standby mode.
		*/
		/* Disable all used wakeup sources*/
		HAL_RTC_DeactivateAlarm( &hrtc, RTC_ALARM_A );
    HAL_RTC_DeactivateAlarm( &hrtc, RTC_ALARM_B );
		
		/* Clear all related STANDBY flags */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);	
	
		/* Set Alarm to almWeekday almHour:almMinute:almSeconds 
			 RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
	
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    
    HAL_RTC_GetDate(&hrtc, &sDateStructureGet, RTC_FORMAT_BIN);	
    
    DBGT(LOG_DEBUG, "MN:RTC:%d:%d:%d", sTime.Hours, sTime.Minutes, sTime.Seconds );

    RTC_AlarmTypeDef sAlarm;

#if 0    
    uint8_t next_second = ( sTime.Seconds+30 )% 60;
    DBGT(LOG_DEBUG, "MN:RTC:NEWS:%d", next_second );

    sAlarm.AlarmTime.Hours = 0;
    sAlarm.AlarmTime.Minutes = 0;

//    sAlarm.AlarmTime.Seconds = RTC_ByteToBcd2(next_second);

    sAlarm.AlarmTime.Seconds = next_second;
    sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;    
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY
                     | RTC_ALARMMASK_HOURS 
                     | RTC_ALARMMASK_MINUTES;
#endif

#if 1
    uint32_t uiTmp = getSettings()->secondsToSleep;
    
    if( uiTmp > 3600 )
      uiTmp = 3600;

    uint32_t  uiNextTimeInSeconds = sTime.Hours*3600 
                                    +sTime.Minutes*60 
                                    +sTime.Seconds
                                    +uiTmp;     
    
    sAlarm.AlarmTime.Seconds  = uiNextTimeInSeconds%60;
    uiTmp                     = uiNextTimeInSeconds/60;
    sAlarm.AlarmTime.Minutes  =( uiTmp )%60 ;
    sAlarm.AlarmTime.Hours    = uiTmp/60;
    if( sAlarm.AlarmTime.Hours > 24  )
      sAlarm.AlarmTime.Hours = uiTmp%24;

    DBGT(LOG_DEBUG, "MN:RTC:NEWS:%d-%02d-%02d", sAlarm.AlarmTime.Hours, sAlarm.AlarmTime.Minutes, sAlarm.AlarmTime.Seconds );
    
    sAlarm.AlarmTime.TimeFormat     = RTC_HOURFORMAT12_AM;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;    
    sAlarm.AlarmMask                = RTC_ALARMMASK_DATEWEEKDAY;
#endif

    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 1;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmTime.SubSeconds = 0;

    sAlarm.Alarm = RTC_ALARM_A;
    if(HAL_RTC_SetAlarm_IT(&hrtc,&sAlarm,FORMAT_BIN) != HAL_OK)
		{
			/* Initialization Error */
			Error_Handler(); 
		}
    
		blAlarmWasConfigured = 0x01; 	// TODO: It is not really needed - just to debug STANDBY 
		
#if 1    
    DBGT( LOG_DEBUG, "MN:SLEEP");
    
    const char pMsg[] = "Sleep           ";
    dispDrawStr( 1, 0, pMsg);
    dispRedraw();
    
		/* Enter the Standby mode */
		HAL_PWR_EnterSTANDBYMode();
#endif   
  }
}

/**
 * \fn     void updateSystemTimeFromRTC( void )
 * \brief  updates Sytem Time counted by sysTicks by RTC value
 * \param  None
 * \return None
 */
void updateSystemTimeFromRTC( void )
{ 
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &sTimeStructureGet, RTC_FORMAT_BIN);

  /* Update system date from RTC */
  HAL_RTC_GetDate(&hrtc, &sDateStructureGet, RTC_FORMAT_BIN);	
  
  _tSysTime systemTime;
  systemTime.seconds = sTimeStructureGet.Seconds;
  systemTime.minutes = sTimeStructureGet.Minutes;
  systemTime.hours   = sTimeStructureGet.Hours;
  fwkSetTime( &systemTime );
  
//  RTC_CalendarShow();
  DBGT( LOG_DEBUG, "MN:RTC:UPD:%d:%d:%d", systemTime.hours, systemTime.minutes, systemTime.seconds );
}

// get time from RTC
void getDateFromRTC(uint8_t* Year,  uint8_t* Month, uint8_t* Date, uint8_t* WeekDay)
{
  /* Update system date from RTC */
  HAL_RTC_GetDate(&hrtc, &sDateStructureGet, RTC_FORMAT_BIN);	
	
	*Year 	= sDateStructureGet.Year;
	*Month 	= sDateStructureGet.Month;
	*Date  	= sDateStructureGet.Date;
	*WeekDay = sDateStructureGet.WeekDay;
}

void setDateForRTC(uint8_t year, uint8_t date, uint8_t month, uint8_t weekday)
{
	sDateStructureGet.Year 		= year;
	sDateStructureGet.Month 	= month;
	sDateStructureGet.Date 		= date;
	sDateStructureGet.WeekDay = weekday;
	
  if(HAL_RTC_SetDate(&hrtc,&sDateStructureGet,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }	
	
  uint32_t uiTmp = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
  
  switch( uiTmp )
  {
    case RTC_BKP_TIME_SET_FLAG:
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BKP_TIME_DATE_SET_FLAG);
      break;
      
    case RTC_BKP_TIME_DATE_SET_FLAG:  
      break;
    
    default:
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BKP_DATE_SET_FLAG);
      break;
  }
}

void setTimeToRTC( uint8_t hours, uint8_t minutes, uint8_t seconds )
{
  RTC_TimeTypeDef stimestructure;

  /*## Update the Time #################################################*/
  stimestructure.Hours          = hours;
  stimestructure.Minutes        = minutes;
  stimestructure.Seconds        = seconds;
  stimestructure.TimeFormat     = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  uint32_t uiTmp = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
  
  switch( uiTmp )
  {
    case RTC_BKP_DATE_SET_FLAG:
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BKP_TIME_DATE_SET_FLAG);
      break;
      
    case RTC_BKP_TIME_DATE_SET_FLAG:  
      break;
    
    default:
      HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_BKP_TIME_SET_FLAG);
      break;
  }  
}

// ------------------------- RTC End ------------------------------------



// ------------------------- UART ---------------------------------------
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void dbgPutCh(char ch)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
}
/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance                     = USART2;
  huart2.Init.BaudRate                = 9600;
  huart2.Init.WordLength              = UART_WORDLENGTH_8B;
  huart2.Init.StopBits                = UART_STOPBITS_1;
  huart2.Init.Parity                  = UART_PARITY_ODD;
  huart2.Init.Mode                    = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl               = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling            = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling          = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit  = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

// ------------------------- TIM21 Begin ------------------------------------

volatile uint32_t  newEncoderVal;
volatile uint32_t  oldEncoderVal = 0;

/**
  * @brief TIM21 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM21_Init(void)
{

  /* USER CODE BEGIN TIM21_Init 0 */

  /* USER CODE END TIM21_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM21_Init 1 */

  /* USER CODE END TIM21_Init 1 */
  htim21.Instance               = TIM21;
  htim21.Init.Prescaler         = 0;
  htim21.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim21.Init.Period            = 65535;
  htim21.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode           = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity           = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection          = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler          = TIM_ICPSC_DIV1;
  sConfig.IC1Filter             = 0;
  sConfig.IC2Polarity           = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection          = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler          = TIM_ICPSC_DIV1;
  sConfig.IC2Filter             = 0;
  if (HAL_TIM_Encoder_Init(&htim21, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* USER CODE BEGIN TIM21_Init 2 */
  newEncoderVal = oldEncoderVal =  htim21.Instance->CNT;
  /* USER CODE END TIM21_Init 2 */
}

_tEQ eqUp = { EV_APP_UP, 00 };

void HAL_TIM_IC_CaptureCallback( TIM_HandleTypeDef *htim )
{
  newEncoderVal = htim->Instance->CNT;
}

_tEQ eqRight = { EV_APP_RIGHT, 00 };
_tEQ eqLeft = { EV_APP_LEFT, 00 };

void mnTestInputEvents( void )
{
  uint32_t tmp;
  _tEQ*    pEvt;
  
  if( newEncoderVal == oldEncoderVal )
    return;
  
  if( newEncoderVal > oldEncoderVal )
  { 
    if(( newEncoderVal-oldEncoderVal)>32000  )
    {
      tmp =( 65535 - newEncoderVal )+oldEncoderVal; 
      pEvt = &eqLeft;
    }
    else 
    {
      pEvt = &eqRight;
      tmp = newEncoderVal-oldEncoderVal;
    }
  }
  else
  {
    if(( oldEncoderVal-newEncoderVal)>32000)
    {
      /* overrun is presented */
      tmp = newEncoderVal+(65535-oldEncoderVal);
      pEvt = &eqRight;      
    }
    else
    {
      pEvt = &eqLeft;
      tmp = oldEncoderVal-newEncoderVal;
    }
  }
  
  for( uint32_t i=0; i<(tmp/8 +1); i++ )
  {
     eqAddEvent( pEvt );
  }

 // DBGT(LOG_DEBUG, "MN:CNT:%d:%d", oldEncoderVal, newEncoderVal );
  
  oldEncoderVal = newEncoderVal;    
}  

// ------------------------- GPIO Start ------------------------------------
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  // Set because Relay
  HAL_GPIO_WritePin(GPIOB, GPIO_PUMP_CONTROL_Pin
                          |GPIO_LCD_CONTROL_Pin, GPIO_PIN_SET);
  // Reset normal case
  HAL_GPIO_WritePin(GPIOB, LD3_Pin
                          |GPIO_PWR_CONTROL_Pin, GPIO_PIN_RESET); 

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin     = GPIO_PIN_12;
  GPIO_InitStruct.Mode    = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull    = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin GPIO_PWR_CONTROL_Pin GPIO_PUMP_CONTROL_Pin |GPIO_PIN_7 */
  GPIO_InitStruct.Pin     = LD3_Pin|GPIO_PWR_CONTROL_Pin|GPIO_PUMP_CONTROL_Pin|GPIO_PIN_7;
  GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull    = GPIO_NOPULL;
  GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

uint8_t encData;
uint8_t encBtn;

_tEQ eqStar = { EV_APP_BTN_ACT, 00 };

void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
  if( GPIO_Pin == GPIO_PIN_12 )
  {
    encBtn = HAL_GPIO_ReadPin( GPIOA, GPIO_PIN_12);
    
    eqStar.reserved = encBtn;
    
    /* Add encoder button event */
    eqAddEventFromIt( &eqStar );
  }
}
// ------------------------- GPIO End --------------------------------------

/* USER CODE BEGIN 4 */

void setLCDControl(uint8_t OnOff)
{
  if( OnOff == 1 )
    HAL_GPIO_WritePin(GPIO_LCD_CONTROL_GPIO_Port, GPIO_LCD_CONTROL_Pin, GPIO_PIN_SET);
  
  if( OnOff== 0 )
     HAL_GPIO_WritePin(GPIO_LCD_CONTROL_GPIO_Port, GPIO_LCD_CONTROL_Pin, GPIO_PIN_RESET);	
}

void setPumpControl(uint8_t OnOff)
{
#if 0 
  if( OnOff == 1 )
    HAL_GPIO_WritePin(GPIO_PUMP_CONTROL_GPIO_Port, GPIO_PUMP_CONTROL_Pin, GPIO_PIN_SET);
  
  if( OnOff== 0 )
     HAL_GPIO_WritePin(GPIO_PUMP_CONTROL_GPIO_Port, GPIO_PUMP_CONTROL_Pin, GPIO_PIN_RESET);	
#endif
 
// Inverse control - because relay  
#if 1 
  if( OnOff == 0 )
    HAL_GPIO_WritePin(GPIO_PUMP_CONTROL_GPIO_Port, GPIO_PUMP_CONTROL_Pin, GPIO_PIN_SET);
  
  if( OnOff== 1)
     HAL_GPIO_WritePin(GPIO_PUMP_CONTROL_GPIO_Port, GPIO_PUMP_CONTROL_Pin, GPIO_PIN_RESET);	
#endif
  
}

void setPwrControl( uint8_t OnOff )
{
  if( OnOff == 1 )
    HAL_GPIO_WritePin(GPIO_PWR_CONTROL_GPIO_Port, GPIO_PWR_CONTROL_Pin, GPIO_PIN_SET);
  
  if( OnOff== 0 )
     HAL_GPIO_WritePin(GPIO_PWR_CONTROL_GPIO_Port, GPIO_PWR_CONTROL_Pin, GPIO_PIN_RESET);	
}

void setLedPinState( uint8_t ledId, uint8_t state )
{
  switch ( ledId )
  {
    case 0:
      if( state == 1  )
        HAL_GPIO_WritePin( LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET );
      
      if( state == 0 )
        HAL_GPIO_WritePin( LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET );
  
      default:
        break;
  }
}


// ------- Eeprom -------------------------------------
#define EEPROM_START_ADDR 	((uint32_t)0x08080000)    /* Data EEPROM base address */
#define EEPROM_END_ADDR			((uint32_t)0x080803FF)    /* Data EEPROM end address */

#ifdef STM32L031xx
void hwkWriteBufferToEeprom( uint16_t eeprom_offset, uint8_t* pBuff, uint32_t size )
{
	uint32_t adr = EEPROM_START_ADDR +eeprom_offset;
	uint32_t eepromByte = 0;
  uint32_t idx = 0;

  /* Erasing EEPROM - could erase additional bytes 
    if size is not packed to word(4) */
  uint32_t uiEndErasedAdr = EEPROM_START_ADDR +eeprom_offset +size;
    
  if( size % sizeof(uint32_t))
    uiEndErasedAdr += 4;

  HAL_FLASHEx_DATAEEPROM_Unlock();
  
  while( adr < uiEndErasedAdr )
  {
    if( HAL_FLASHEx_DATAEEPROM_Erase( adr ) != HAL_OK )
      Error_Handler();   
    
    adr += 4;
  }
  
  /* EEPROM Programming */
  // assign to start
  adr = EEPROM_START_ADDR +eeprom_offset;
  
  for( idx = 0; idx<size; idx++ )
  {
    eepromByte = pBuff[ idx ];

    HAL_FLASHEx_DATAEEPROM_Program( FLASH_TYPEPROGRAMDATA_BYTE, adr++, eepromByte );  
  }
  HAL_FLASHEx_DATAEEPROM_Lock();   
}

void hwkReadBufferFromEeprom( uint16_t eeprom_offset, uint8_t* pBuf, uint32_t size) 
{
	uint32_t addr = EEPROM_START_ADDR +eeprom_offset;

	for( uint32_t i=0; i<size; i++ )
		pBuf[ i ]= (*(__IO uint8_t*)( addr +i ));
}
#endif 

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
