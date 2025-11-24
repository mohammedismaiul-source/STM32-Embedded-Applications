/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_pwr_modes_H
#define __STM32F4xx_pwr_modes_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
/*************************** ART accelerator configuration*********************/

/* Uncomment the macros to enable ART with or without prefetch buffer */

/*Enable ART*/
 // #define ART_Enable

/*Enbale prefetch when ART is enabled */
 // #define  Prefetch_Enable

/********************* Power supply selection *********************************/

/* Uncomment the macros to select 3,3 V or 1,8 V as power supply */
 #define VDD3_3


/************************* Power sub-modes selection **************************/

/* Uncomment macro(s) to select sub-mode(s) to be measured for each power mode*/

/*----------------------- STOP sub-modes selection ---------------------------*/
//#define StopMainRegFlashStop
//#define StopMainRegFlashPwrDown
//#define StopLowPwrRegFlashStop
#define StopLowPwrRegFlashPwrDown
//#define StopMainRegUnderDriveFlashPwrDown
//#define StopLowPwrRegUnderDriveFlashPwrDown


/***************************** STOP mode  *************************************/

/* Stop mode, Main Regulator with Flash Stop */
void PWR_StopMainRegFlashStop (void);

/* Stop mode, Main Regulator with Flash Power down */
void PWR_StopMainRegFlashPwrDown (void);

/* Stop mode, Low power Regulator with Flash Stop */
void PWR_StopLowPwrRegFlashStop (void);

/* Stop mode, Low power Regulator with Flash Power down */
void PWR_StopLowPwrRegFlashPwrDown (void);

/* Stop mode, Main Under-drive Regulator with Flash Power down */
void PWR_StopMainRegUnderDriveFlashPwrDown (void);

/* Stop mode, Low power Under-drive Regulator with Flash Power down */
void PWR_StopLowPwrRegUnderDriveFlashPwrDown (void);



#define ALL_GPIOs           (RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOB| RCC_AHB1Periph_GPIOC | \
                            RCC_AHB1Periph_GPIOD| RCC_AHB1Periph_GPIOE| RCC_AHB1Periph_GPIOF | \
                            RCC_AHB1Periph_GPIOG| RCC_AHB1Periph_GPIOH)


#endif /* __STM32F4xx_pwr_modes_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
