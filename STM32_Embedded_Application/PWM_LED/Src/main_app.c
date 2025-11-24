/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main_app.h"
#include <string.h>

/* Private function prototypes ----------------------------------------------*/
static void GPIO_Init(void);
static void TIMER2_Init(void);
static void UART2_Init(void);
static void SystemClock_Config_HSE(uint8_t clock_freq);
static void Error_handler(void);

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef gTim2Handle;
UART_HandleTypeDef gUart2Handle;

/* -------------------------------------------------------------------------- */
/*                                  main                                      */
/* -------------------------------------------------------------------------- */

int main(void)
{
    uint16_t duty = 0;
    const uint16_t dutyStep = 20U;

    /* HAL & clock configuration */
    HAL_Init();
    SystemClock_Config_HSE(SYS_CLOCK_FREQ_50_MHZ);

    /* Peripherals used in this small demo */
    GPIO_Init();
    UART2_Init();
    TIMER2_Init();

    /* Start PWM output on TIM2 channel 1 */
    if (HAL_TIM_PWM_Start(&gTim2Handle, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_handler();
    }

    /* LED breathing effect using PWM duty cycle */
    while (1)
    {
        /* Increase brightness */
        while (duty < gTim2Handle.Init.Period)
        {
            duty = (uint16_t)(duty + dutyStep);
            __HAL_TIM_SET_COMPARE(&gTim2Handle, TIM_CHANNEL_1, duty);
            HAL_Delay(1);
        }

        /* Decrease brightness */
        while (duty > 0U)
        {
            duty = (uint16_t)(duty - dutyStep);
            __HAL_TIM_SET_COMPARE(&gTim2Handle, TIM_CHANNEL_1, duty);
            HAL_Delay(1);
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                          Clock configuration                               */
/* -------------------------------------------------------------------------- */

/**
  * @brief  Configure system clock using HSE and PLL.
  * @param  clock_freq: one of SYS_CLOCK_FREQ_xx_MHZ (see main.h)
  */
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
            flashLatency          = 1;
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
            flashLatency          = 2;
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
            flashLatency          = 3;
            break;

        default:
            return;
    }

    if (HAL_RCC_OscConfig(&oscCfg) != HAL_OK)
    {
        Error_handler();
    }

    if (HAL_RCC_ClockConfig(&clkCfg, flashLatency) != HAL_OK)
    {
        Error_handler();
    }

    /* Configure Systick for 1 ms tick */
    uint32_t hclkFreq = HAL_RCC_GetHCLKFreq();
    HAL_SYSTICK_Config(hclkFreq / 1000U);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* -------------------------------------------------------------------------- */
/*                          GPIO configuration                                */
/* -------------------------------------------------------------------------- */

/**
  * @brief  Configure GPIO for LED on PA5.
  */
static void GPIO_Init(void)
{
    GPIO_InitTypeDef ledCfg;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    memset(&ledCfg, 0, sizeof(ledCfg));
    ledCfg.Pin   = GPIO_PIN_5;
    ledCfg.Mode  = GPIO_MODE_OUTPUT_PP;
    ledCfg.Pull  = GPIO_NOPULL;
    ledCfg.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &ledCfg);
}

/* -------------------------------------------------------------------------- */
/*                          UART configuration                                */
/* -------------------------------------------------------------------------- */

static void UART2_Init(void)
{
    gUart2Handle.Instance        = USART2;
    gUart2Handle.Init.BaudRate   = 115200;
    gUart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
    gUart2Handle.Init.StopBits   = UART_STOPBITS_1;
    gUart2Handle.Init.Parity     = UART_PARITY_NONE;
    gUart2Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    gUart2Handle.Init.Mode       = UART_MODE_TX_RX;

    if (HAL_UART_Init(&gUart2Handle) != HAL_OK)
    {
        Error_handler();
    }
}

/* -------------------------------------------------------------------------- */
/*                          TIM2 PWM configuration                            */
/* -------------------------------------------------------------------------- */

static void TIMER2_Init(void)
{
    TIM_OC_InitTypeDef tim2PwmCfg;

    gTim2Handle.Instance = TIM2;
    gTim2Handle.Init.Period    = 10000U - 1U;
    gTim2Handle.Init.Prescaler = 4U;

    if (HAL_TIM_PWM_Init(&gTim2Handle) != HAL_OK)
    {
        Error_handler();
    }

    memset(&tim2PwmCfg, 0, sizeof(tim2PwmCfg));
    tim2PwmCfg.OCMode     = TIM_OCMODE_PWM1;
    tim2PwmCfg.OCPolarity = TIM_OCPOLARITY_HIGH;
    tim2PwmCfg.Pulse      = 0U;

    if (HAL_TIM_PWM_ConfigChannel(&gTim2Handle,
                                  &tim2PwmCfg,
                                  TIM_CHANNEL_1) != HAL_OK)
    {
        Error_handler();
    }
}

/* -------------------------------------------------------------------------- */
/*                               Error handler                                */
/* -------------------------------------------------------------------------- */

static void Error_handler(void)
{
    /* Stay here if something went wrong during initialization */
    while (1)
    {
    }
}
