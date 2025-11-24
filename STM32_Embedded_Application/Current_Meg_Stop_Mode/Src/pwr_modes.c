/**
  ******************************************************************************
  * @file    pwr_modes.c
  * @brief   Power modes helper functions
  *
  * Original file based on STMicroelectronics application example.
  * This version has been slightly restructured and commented.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx.h"
#include "pwr_modes.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define WakeupCounter       0xA000u
#define HSE_OF_NUCLEO_F446RE 8u

/* Private variables ---------------------------------------------------------*/
ErrorStatus     HSEStartUpStatus;
GPIO_InitTypeDef GPIO_InitStructure;

/* Local helpers -------------------------------------------------------------*/
static void PWR_PrepareForStopMode(void);

/**
  * @brief  Disable clocks for almost all AHBx/APBx peripherals.
  *         Can be used to reduce dynamic power before entering low-power mode.
  */
void DisableAllAHBxAPBxPeriClocks(void)
{
  /* Clear all enable bits in clock control registers.
   * For details see RM (RCC section). */
  RCC->AHB1ENR &= ~(0xFFFFFFFFu);
  RCC->AHB2ENR &= ~(0xFFFFFFFFu);
  RCC->AHB3ENR &= ~(0xFFFFFFFFu);
  RCC->APB1ENR &= ~(0xFFFFFFFFu);
  RCC->APB2ENR &= ~(0xFFFFFFFFu);
}

/**
  * @brief  Restore a useful default clock configuration for AHBx/APBx domains.
  *         Values are taken from the original example and may be adapted.
  */
void EnableAllAHBxAPBxPeriClocks(void)
{
  RCC->AHB1ENR |= 0x606410FFu;
  RCC->AHB2ENR |= ((1u << 0) | (1u << 7));
  RCC->AHB3ENR |= 0x00000003u;
  RCC->APB1ENR |= 0x3FFFC9FFu;
  RCC->APB2ENR |= 0x00C77F66u;
}

/* -------------------------------------------------------------------------- */
/*                          STOP mode functions                               */
/* -------------------------------------------------------------------------- */

/**
  * @brief Common preparation used by all STOP mode variants.
  *        - Clear wakeup flag
  *        - Configure button / wakeup source
  */
static void PWR_PrepareForStopMode(void)
{
  /* Clear Wakeup flag to avoid spurious wakeups */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Configure the button EXTI as wakeup source */
  ButtonPinInt_configuration();
}

/**
  * @brief  STOP mode:
  *           - Regulator in main mode
  *           - Flash in STOP
  *           - Wakeup via PC13
  */
void PWR_StopMainRegFlashStop(void)
{
  PWR_PrepareForStopMode();

  /* Ensure Flash is NOT in deep power-down */
  PWR_FlashPowerDownCmd(DISABLE);

  /* Enter STOP with main regulator */
  PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}

/**
  * @brief  STOP mode:
  *           - Regulator in main mode
  *           - Flash in deep power-down
  *           - Wakeup via PC13
  */
void PWR_StopMainRegFlashPwrDown(void)
{
  PWR_PrepareForStopMode();

  /* Put Flash into deep power-down */
  PWR_FlashPowerDownCmd(ENABLE);

  /* Enter STOP with main regulator */
  PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}

/**
  * @brief  STOP mode:
  *           - Regulator in low-power mode
  *           - Flash in STOP
  *           - Wakeup via PC13
  */
void PWR_StopLowPwrRegFlashStop(void)
{
  PWR_PrepareForStopMode();

  /* Flash kept in STOP (not deep power-down) */
  PWR_FlashPowerDownCmd(DISABLE);

  /* Enter STOP with low-power regulator */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/**
  * @brief  STOP mode:
  *           - Regulator in low-power mode
  *           - Flash in deep power-down
  *           - Wakeup via PC13
  */
void PWR_StopLowPwrRegFlashPwrDown(void)
{
  PWR_PrepareForStopMode();

  /* Flash deep power-down enabled */
  PWR_FlashPowerDownCmd(ENABLE);

  /* Enter STOP with low-power regulator */
  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/**
  * @brief  STOP mode:
  *           - Main regulator in under-drive
  *           - Flash in deep power-down
  *           - Wakeup via PC13
  */
void PWR_StopMainRegUnderDriveFlashPwrDown(void)
{
  PWR_PrepareForStopMode();

  /* Enable under-drive feature */
  PWR_UnderDriveCmd(ENABLE);

  /* Enter under-drive STOP with main regulator */
  PWR_EnterUnderDriveSTOPMode(PWR_MainRegulator_UnderDrive_ON,
                              PWR_STOPEntry_WFI);
}

/**
  * @brief  STOP mode:
  *           - Low-power regulator in under-drive
  *           - Flash in deep power-down
  *           - Wakeup via PC13
  */
void PWR_StopLowPwrRegUnderDriveFlashPwrDown(void)
{
  PWR_PrepareForStopMode();

  /* Enable under-drive feature */
  PWR_UnderDriveCmd(ENABLE);

  /* Enter under-drive STOP with low-power regulator */
  PWR_EnterUnderDriveSTOPMode(PWR_LowPowerRegulator_UnderDrive_ON,
                              PWR_STOPEntry_WFI);
}
