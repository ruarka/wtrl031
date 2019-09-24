/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

extern RTC_DateTypeDef 				sDateStructureGet;

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void setLCDControl(uint8_t OnOff);
extern void setPumpControl(uint8_t OnOff);
extern void setPwrControl( uint8_t OnOff );
extern void setLedPinState( uint8_t ledId, uint8_t state );

extern void setADCChanel( uint32_t cnl, uint32_t OnOff );
extern void startADCConversion( void );
extern void stopADCConversion( void );

//extern void updateSystemDateFromRTC( void );
extern void updateSystemTimeFromRTC( void );
extern void hwSleepingInitiate(void);

extern void hwkReadBufferFromEeprom( uint16_t eeprom_offset, uint8_t* pBuf, uint32_t size); 
extern void hwkWriteBufferToEeprom( uint16_t eeprom_offset, uint8_t* pBuff, uint32_t size );

extern void mnTestInputEvents( void );

extern void deviceRedraw( void );

extern void getDateFromRTC(uint8_t* Year,  uint8_t* Month, uint8_t* Date, uint8_t* WeekDay);
extern void setDateForRTC(uint8_t year, uint8_t date, uint8_t month, uint8_t weekday);
extern void setTimeToRTC( uint8_t hours, uint8_t minutes, uint8_t seconds );

#ifdef __cplusplus
}
#endif

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCP_TX_Pin                    GPIO_PIN_2
#define VCP_TX_GPIO_Port              GPIOA
#define TMS_Pin                       GPIO_PIN_13
#define TMS_GPIO_Port                 GPIOA
#define TCK_Pin                       GPIO_PIN_14
#define TCK_GPIO_Port                 GPIOA
#define VCP_RX_Pin                    GPIO_PIN_15
#define VCP_RX_GPIO_Port              GPIOA
#define LD3_Pin                       GPIO_PIN_3
#define LD3_GPIO_Port                 GPIOB
#define GPIO_PWR_CONTROL_Pin          GPIO_PIN_4
#define GPIO_PWR_CONTROL_GPIO_Port    GPIOB
#define GPIO_PUMP_CONTROL_Pin         GPIO_PIN_5
#define GPIO_PUMP_CONTROL_GPIO_Port   GPIOB
#define GPIO_LCD_CONTROL_Pin          GPIO_PIN_7
#define GPIO_LCD_CONTROL_GPIO_Port    GPIOB

/* USER CODE BEGIN Private defines */

/* Application constants */
/*! \def TIME_TO_SLEEP_DEF_VAL
    \brief Seconds after menu inactivity deected before proc starts sleeping procedure
*/ 
#define TIME_TO_SLEEP_DEF_VAL						60

/*! \def PUMP_IS_ON_DEF_VAL
    \brief Def settings will ON for a PUMP
*/
#define PUMP_IS_ON_DEF_VAL							0x01

#define RTC_BKP_TIME_DATE_SET_FLAG      0x32F2
#define RTC_BKP_TIME_SET_FLAG           0xAAF2
#define RTC_BKP_DATE_SET_FLAG           0xF2AA

/* USER CODE END Private defines */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
