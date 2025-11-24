#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "stm32f4xx_hal.h"
#include "main_app.h"

/* Private function prototypes -----------------------------------------------*/
static void GPIO_Init(void);
static void UART2_Init(void);
static void RTC_Init(void);
static void RTC_CalendarConfig(void);
static void SystemClock_Config_HSE(uint8_t clock_freq);
static void RTC_AppError(void);
static void rtc_uart_printf(const char *format, ...);
static const char *rtc_get_weekday_name(uint8_t index);

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef gUart2Handle;
RTC_HandleTypeDef  gRtcHandle;

/* -------------------------------------------------------------------------- */
/*                              helper functions                              */
/* -------------------------------------------------------------------------- */

/**
  * @brief  Simple printf-style helper that sends text over UART2.
  */
static void rtc_uart_printf(const char *format, ...)
{
    char buffer[80];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    HAL_UART_Transmit(&gUart2Handle,
                      (uint8_t *)buffer,
                      strlen(buffer),
                      HAL_MAX_DELAY);
}

/**
  * @brief  Return weekday string from RTC weekday number.
  * @param  index: 1..7 according to STM32 RTC weekday encoding.
  */
static const char *rtc_get_weekday_name(uint8_t index)
{
    static const char *names[] =
    {
        "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday", "Sunday"
    };

    if (index == 0 || index > 7)
    {
        return "?";
    }
    return names[index - 1];
}

/* -------------------------------------------------------------------------- */
/*                                   main                                     */
/* -------------------------------------------------------------------------- */

int main(void)
{
    HAL_Init();

    GPIO_Init();
    SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);
    UART2_Init();
    RTC_Init();

    rtc_uart_printf("RTC standby example started\r\n");

    /* Check if we are returning from STANDBY */
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
    {
        /* Clear standby & wakeup flags */
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

        rtc_uart_printf("System woke up from STANDBY mode\r\n");

        /* Simulate EXTI callback to print current date/time */
        HAL_GPIO_EXTI_Callback(0);
    }

    /* Optionally configure date/time once */
    /* RTC_CalendarConfig(); */

    /* Enable Wakeup pin 1 */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    rtc_uart_printf("Entering STANDBY mode now\r\n");

    /* Enter STANDBY; execution will continue from reset on wakeup */
    HAL_PWR_EnterSTANDBYMode();

    /* Should never reach here */
    while (1)
    {
    }

    /* Just to satisfy compiler */
    return 0;
}

/* -------------------------------------------------------------------------- */
/*                        System clock configuration                          */
/* -------------------------------------------------------------------------- */

static void SystemClock_Config_HSE(uint8_t clock_freq)
{
    RCC_OscInitTypeDef oscCfg;
    RCC_ClkInitTypeDef clkCfg;
    uint8_t flashLatency = 0;

    memset(&oscCfg, 0, sizeof(oscCfg));
    memset(&clkCfg, 0, sizeof(clkCfg));

    oscCfg.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    oscCfg.HSEState            = RCC_HSE_ON;
    oscCfg.PLL.PLLState        = RCC_PLL_ON;
    oscCfg.PLL.PLLSource       = RCC_PLLSOURCE_HSE;

    switch (clock_freq)
    {
        case SYS_CLOCK_FREQ_50_MHZ:
            oscCfg.PLL.PLLM = 4;
            oscCfg.PLL.PLLN = 50;
            oscCfg.PLL.PLLP = RCC_PLLP_DIV2;
            oscCfg.PLL.PLLQ = 2;
            oscCfg.PLL.PLLR = 2;

            clkCfg.ClockType      = RCC_CLOCKTYPE_HCLK  |
                                    RCC_CLOCKTYPE_SYSCLK |
                                    RCC_CLOCKTYPE_PCLK1 |
                                    RCC_CLOCKTYPE_PCLK2;
            clkCfg.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
            clkCfg.AHBCLKDivider  = RCC_SYSCLK_DIV1;
            clkCfg.APB1CLKDivider = RCC_HCLK_DIV2;
            clkCfg.APB2CLKDivider = RCC_HCLK_DIV1;
            flashLatency = 1;
            break;

        case SYS_CLOCK_FREQ_84_MHZ:
            oscCfg.PLL.PLLM = 4;
            oscCfg.PLL.PLLN = 84;
            oscCfg.PLL.PLLP = RCC_PLLP_DIV2;
            oscCfg.PLL.PLLQ = 2;
            oscCfg.PLL.PLLR = 2;

            clkCfg.ClockType      = RCC_CLOCKTYPE_HCLK  |
                                    RCC_CLOCKTYPE_SYSCLK |
                                    RCC_CLOCKTYPE_PCLK1 |
                                    RCC_CLOCKTYPE_PCLK2;
            clkCfg.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
            clkCfg.AHBCLKDivider  = RCC_SYSCLK_DIV1;
            clkCfg.APB1CLKDivider = RCC_HCLK_DIV2;
            clkCfg.APB2CLKDivider = RCC_HCLK_DIV1;
            flashLatency = 2;
            break;

        case SYS_CLOCK_FREQ_120_MHZ:
            oscCfg.PLL.PLLM = 4;
            oscCfg.PLL.PLLN = 120;
            oscCfg.PLL.PLLP = RCC_PLLP_DIV2;
            oscCfg.PLL.PLLQ = 2;
            oscCfg.PLL.PLLR = 2;

            clkCfg.ClockType      = RCC_CLOCKTYPE_HCLK  |
                                    RCC_CLOCKTYPE_SYSCLK |
                                    RCC_CLOCKTYPE_PCLK1 |
                                    RCC_CLOCKTYPE_PCLK2;
            clkCfg.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
            clkCfg.AHBCLKDivider  = RCC_SYSCLK_DIV1;
            clkCfg.APB1CLKDivider = RCC_HCLK_DIV4;
            clkCfg.APB2CLKDivider = RCC_HCLK_DIV2;
            flashLatency = 3;
            break;

        default:
            return;
    }

    if (HAL_RCC_OscConfig(&oscCfg) != HAL_OK)
    {
        RTC_AppError();
    }

    if (HAL_RCC_ClockConfig(&clkCfg, flashLatency) != HAL_OK)
    {
        RTC_AppError();
    }

    /* Configure SysTick to generate 1ms interrupts */
    uint32_t hclk = HAL_RCC_GetHCLKFreq();
    HAL_SYSTICK_Config(hclk / 1000U);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* -------------------------------------------------------------------------- */
/*                             RTC configuration                              */
/* -------------------------------------------------------------------------- */

static void RTC_Init(void)
{
    gRtcHandle.Instance = RTC;
    gRtcHandle.Init.HourFormat     = RTC_HOURFORMAT_12;
    gRtcHandle.Init.AsynchPrediv   = 0x7F;
    gRtcHandle.Init.SynchPrediv    = 0xFF;
    gRtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    gRtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
    gRtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&gRtcHandle) != HAL_OK)
    {
        RTC_AppError();
    }
}

/**
  * @brief  Configure a fixed calendar value in the RTC.
  *         Example: 12 June 2018, 12:11:10 PM, Tuesday.
  */
static void RTC_CalendarConfig(void)
{
    RTC_TimeTypeDef timeCfg;
    RTC_DateTypeDef dateCfg;

    timeCfg.Hours      = 12;
    timeCfg.Minutes    = 11;
    timeCfg.Seconds    = 10;
    timeCfg.TimeFormat = RTC_HOURFORMAT12_PM;

    if (HAL_RTC_SetTime(&gRtcHandle, &timeCfg, RTC_FORMAT_BIN) != HAL_OK)
    {
        RTC_AppError();
    }

    dateCfg.Date    = 12;
    dateCfg.Month   = RTC_MONTH_JUNE;
    dateCfg.Year    = 18;
    dateCfg.WeekDay = RTC_WEEKDAY_TUESDAY;

    if (HAL_RTC_SetDate(&gRtcHandle, &dateCfg, RTC_FORMAT_BIN) != HAL_OK)
    {
        RTC_AppError();
    }
}

/* -------------------------------------------------------------------------- */
/*                             GPIO & UART init                               */
/* -------------------------------------------------------------------------- */

static void GPIO_Init(void)
{
    GPIO_InitTypeDef ledCfg;
    GPIO_InitTypeDef btnCfg;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* LED on PA5 */
    ledCfg.Pin   = GPIO_PIN_5;
    ledCfg.Mode  = GPIO_MODE_OUTPUT_PP;
    ledCfg.Pull  = GPIO_NOPULL;
    ledCfg.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &ledCfg);

    /* User button on PC13 as EXTI */
    btnCfg.Pin  = GPIO_PIN_13;
    btnCfg.Mode = GPIO_MODE_IT_FALLING;
    btnCfg.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &btnCfg);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

static void UART2_Init(void)
{
    gUart2Handle.Instance        = USART2;
    gUart2Handle.Init.BaudRate   = 115200;
    gUart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
    gUart2Handle.Init.StopBits   = UART_STOPBITS_1;
    gUart2Handle.Init.Parity     = UART_PARITY_NONE;
    gUart2Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    gUart2Handle.Init.Mode       = UART_MODE_TX;

    if (HAL_UART_Init(&gUart2Handle) != HAL_OK)
    {
        RTC_AppError();
    }
}

/* -------------------------------------------------------------------------- */
/*                                Callbacks                                   */
/* -------------------------------------------------------------------------- */

/**
  * @brief  EXTI line detection callback.
  *         Prints current time and date using RTC values.
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    RTC_TimeTypeDef timeNow;
    RTC_DateTypeDef dateNow;

    (void)GPIO_Pin;

    HAL_RTC_GetTime(&gRtcHandle, &timeNow, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&gRtcHandle, &dateNow, RTC_FORMAT_BIN);

    rtc_uart_printf("Time : %02d:%02d:%02d\r\n",
                    timeNow.Hours,
                    timeNow.Minutes,
                    timeNow.Seconds);

    rtc_uart_printf("Date : %02d-%02d-%02d  <%s>\r\n",
                    dateNow.Month,
                    dateNow.Date,
                    dateNow.Year,
                    rtc_get_weekday_name(dateNow.WeekDay));
}

/* -------------------------------------------------------------------------- */
/*                             Error handling                                 */
/* -------------------------------------------------------------------------- */

static void RTC_AppError(void)
{
    while (1)
    {
        /* stay here */
    }
}