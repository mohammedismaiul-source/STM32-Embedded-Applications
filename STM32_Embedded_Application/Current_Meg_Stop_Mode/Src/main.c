/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * Portions of this file outside the USER CODE blocks are owned by their
  * respective copyright holders as described in the project.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pwr_modes.h"

/* Global state -------------------------------------------------------------*/
__IO uint32_t uwCounter        = 0x00;
__IO uint8_t  UserButtonStatus = RESET;

/* Local functions ----------------------------------------------------------*/
static void Mode_Exit(void);
static void LedsConfig(void);
static void LowPowerDemo_Init(void);

/**
  * @brief  Application entry point.
  * @retval int
  */
int main(void)
{
  /* Basic initialization for the low-power demo */
  LowPowerDemo_Init();

  /* Execute the selected STOP mode scenarios */
  Measure_Stop();

  /* Application remains here; device will enter/exit STOP through callbacks */
  while (1)
  {
    /* main loop intentionally empty */
  }
}

/**
  * @brief  Initialize peripherals required for the current consumption demo.
  *         - Enable PWR clock
  *         - Configure LED
  *         - Configure wakeup button as EXTI source
  */
static void LowPowerDemo_Init(void)
{
  /* Enable PWR APB1 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* LED used as a simple status indicator */
  LedsConfig();

  /* Configure user button as external interrupt (wakeup source) */
  ButtonPinInt_configuration();
}

/**
  * @brief  Execute the configured STOP mode measurements.
  *         The actual mode is selected via compile-time defines.
  */
void Measure_Stop(void)
{
#if defined StopMainRegFlashStop

  /* Wait for user action, then enter STOP (main regulator, Flash in STOP) */
  WaitUser_PushBotton();
  GPIO_AnalogConfig();
  PWR_StopMainRegFlashStop();
  Mode_Exit();
  UserButtonStatus = RESET;

#endif /* StopMainRegFlashStop */

#if defined StopMainRegFlashPwrDown

  /* STOP with main regulator and Flash in deep power-down */
  WaitUser_PushBotton();
  GPIO_AnalogConfig();
  PWR_StopMainRegFlashPwrDown();
  Mode_Exit();
  UserButtonStatus = RESET;

#endif /* StopMainRegFlashPwrDown */

#if defined StopLowPwrRegFlashStop

  /* STOP with low-power regulator, Flash kept in STOP */
  WaitUser_PushBotton();
  GPIO_AnalogConfig();
  PWR_StopLowPwrRegFlashStop();
  Mode_Exit();
  UserButtonStatus = RESET;

#endif /* StopLowPwrRegFlashStop */

#if defined StopLowPwrRegFlashPwrDown

  /* STOP with low-power regulator and Flash in deep power-down */
  WaitUser_PushBotton();
  /* Optional: configure GPIO as analog here if needed */
  GPIO_AnalogConfig();
  PWR_StopLowPwrRegFlashPwrDown();
  Mode_Exit();
  UserButtonStatus = RESET;

#endif /* StopLowPwrRegFlashPwrDown */

#if defined StopMainRegUnderDriveFlashPwrDown

  /* STOP with main regulator in under-drive mode and Flash power-down */
  WaitUser_PushBotton();
  GPIO_AnalogConfig();
  PWR_StopMainRegUnderDriveFlashPwrDown();
  Mode_Exit();
  UserButtonStatus = RESET;

#endif /* StopMainRegUnderDriveFlashPwrDown */

#if defined StopLowPwrRegUnderDriveFlashPwrDown

  /* STOP with low-power regulator in under-drive and Flash power-down */
  WaitUser_PushBotton();
  GPIO_AnalogConfig();
  PWR_StopLowPwrRegUnderDriveFlashPwrDown();
  Mode_Exit();
  UserButtonStatus = RESET;

#endif /* StopLowPwrRegUnderDriveFlashPwrDown */
}

/**
  * @brief  Put all GPIOs in analog mode to minimize current consumption.
  */
void GPIO_AnalogConfig(void)
{
  GPIO_InitTypeDef gpio;

  /* Enable all GPIO ports clocks */
  RCC_AHB1PeriphClockCmd(
      RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
      RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
      RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOH,
      ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

  gpio.GPIO_Mode  = GPIO_Mode_AN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  gpio.GPIO_Pin   = GPIO_Pin_All;

  GPIO_Init(GPIOA, &gpio);
  GPIO_Init(GPIOB, &gpio);
  GPIO_Init(GPIOC, &gpio);
  GPIO_Init(GPIOD, &gpio);
  GPIO_Init(GPIOE, &gpio);
  GPIO_Init(GPIOF, &gpio);
  GPIO_Init(GPIOG, &gpio);
  GPIO_Init(GPIOH, &gpio);

  /* Disable clocks again once configuration is done */
  RCC_AHB1PeriphClockCmd(
      RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
      RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
      RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOH,
      DISABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, DISABLE);
}

/**
  * @brief  Configure LED (PA5 on NUCLEO-F446RE) as push-pull output.
  */
static void LedsConfig(void)
{
  GPIO_InitTypeDef gpio;

  /* Enable port A clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  gpio.GPIO_Pin   = GPIO_Pin_5;
  gpio.GPIO_Mode  = GPIO_Mode_OUT;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &gpio);
}

/**
  * @brief  Configure PC13 as EXTI wakeup source.
  *         This function sets up GPIOC, SYSCFG and NVIC.
  *
  * @note   Board-specific for NUCLEO-F446RE.
  */
void ButtonPinInt_configuration(void)
{
  GPIO_InitTypeDef  gpio;
  EXTI_InitTypeDef  exti;
  NVIC_InitTypeDef  nvic;

  /* Enable GPIOC and SYSCFG clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* PC13 as input */
  gpio.GPIO_Pin   = GPIO_Pin_13;
  gpio.GPIO_Mode  = GPIO_Mode_IN;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &gpio);

  /* Route EXTI line 13 to PC13 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

  /* Configure EXTI line 13 */
  EXTI_DeInit();
  exti.EXTI_Line    = EXTI_Line13;
  exti.EXTI_Mode    = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = EXTI_Trigger_Rising;
  exti.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti);

  /* Configure NVIC for EXTI15_10_IRQn */
  nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority        = 0;
  nvic.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&nvic);
}

/**
  * @brief  Wait until user presses the push button.
  *         LED is turned on while waiting and turned off once pressed.
  */
void WaitUser_PushBotton(void)
{
  /* Turn LED on as visual indication */
  GPIO_SetBits(GPIOA, GPIO_Pin_5);

  /* Block until EXTI callback sets the flag */
  while (UserButtonStatus != SET)
  {
    /* spin */
  }

  /* Clear LED and status flag */
  GPIO_ResetBits(GPIOA, GPIO_Pin_5);
  UserButtonStatus = RESET;
}

/**
  * @brief  Reconfigure the system after exiting STOP mode.
  *         - Reset RCC configuration
  *         - Switch system clock back to HSI
  *         - Re-enable PWR and I/O used in the demo
  */
static void Mode_Exit(void)
{
  /* Reset RCC to default reset state */
  RCC_DeInit();

  /* Disable HSE just in case it was enabled */
  RCC_HSEConfig(RCC_HSE_OFF);

  /* Enable internal HSI oscillator */
  RCC_HSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {
    /* wait until HSI is ready */
  }

  /* Use HSI as system clock */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  while (RCC_GetSYSCLKSource() != 0x00)
  {
    /* wait until switch is complete */
  }

  /* Re-enable PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Re-initialize LED and button EXTI configuration */
  LedsConfig();
  ButtonPinInt_configuration();
}

/**
  * @brief  Dummy delay placeholder (not used in this demo).
  */
void Delay(__IO uint32_t nTime)
{
  (void)nTime;
}

/**
  * @brief  EXTI callback: called when PC13 interrupt fires.
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_Pin_13)
  {
    UserButtonStatus = SET;
  }
}

/**
  * @brief  Error handler.
  */
void _Error_Handler(char *file, int line)
{
  (void)file;
  (void)line;

  while (1)
  {
    /* stay here */
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif /* USE_FULL_ASSERT */
