

#include "main_app.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

/* Use handles defined in CubeIDE main.c */
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef  hcan1;

/* Local helpers */
static void CAN_AppConfigFilter(void);
static void CAN_AppSendInitialFrame(void);

/* Small UART print helper */
static void CAN_AppPrint(const char *text)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), HAL_MAX_DELAY);
}

/*
 * Called once from main() after HAL and peripherals are initialized.
 */
void CAN_AppInit(void)
{
    /* Configure basic filter: accept all IDs into FIFO0 */
    CAN_AppConfigFilter();

    /* Enable some CAN interrupts: TX, RX FIFO0, Bus-off */
    if (HAL_CAN_ActivateNotification(&hcan1,
                                     CAN_IT_TX_MAILBOX_EMPTY |
                                     CAN_IT_RX_FIFO0_MSG_PENDING |
                                     CAN_IT_BUSOFF) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start CAN peripheral */
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Send one test frame at startup */
    CAN_AppSendInitialFrame();
}

/*
 * Called repeatedly from main() loop.
 * For this demo we don't need a lot here, but it keeps the structure clean.
 */
void CAN_AppTask(void)
{
    /* In a real project you could blink an LED, check flags, etc. */
}

/* -------------------- Local functions -------------------- */

static void CAN_AppConfigFilter(void)
{
    CAN_FilterTypeDef filter;

    memset(&filter, 0, sizeof(filter));

    filter.FilterActivation    = ENABLE;
    filter.FilterBank          = 0;
    filter.FilterFIFOAssignment= CAN_RX_FIFO0;
    filter.FilterIdHigh        = 0x0000;
    filter.FilterIdLow         = 0x0000;
    filter.FilterMaskIdHigh    = 0x0000;
    filter.FilterMaskIdLow     = 0x0000;
    filter.FilterMode          = CAN_FILTERMODE_IDMASK;
    filter.FilterScale         = CAN_FILTERSCALE_32BIT;

    if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK)
    {
        Error_Handler();
    }
}

static void CAN_AppSendInitialFrame(void)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    uint8_t data[5] = { 'H', 'E', 'L', 'L', 'O' };

    memset(&txHeader, 0, sizeof(txHeader));

    txHeader.DLC  = 5;
    txHeader.StdId= 0x65D;
    txHeader.IDE  = CAN_ID_STD;
    txHeader.RTR  = CAN_RTR_DATA;

    if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &txMailbox) != HAL_OK)
    {
        Error_Handler();
    }
}

/* -------------------- CAN callbacks -------------------- */

/* TX mailbox 0 */
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    CAN_AppPrint("CAN TX complete: mailbox 0\r\n");
}

/* TX mailbox 1 */
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    CAN_AppPrint("CAN TX complete: mailbox 1\r\n");
}

/* TX mailbox 2 */
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    CAN_AppPrint("CAN TX complete: mailbox 2\r\n");
}

/* RX FIFO0 pending */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t payload[8];
    char text[64];

    (void)hcan;

    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, payload) != HAL_OK)
    {
        Error_Handler();
    }

    /* Make sure the data is printable string (for demo only) */
    payload[rxHeader.DLC < 8 ? rxHeader.DLC : 7] = '\0';

    snprintf(text, sizeof(text), "CAN RX: %s\r\n", payload);
    CAN_AppPrint(text);
}

/* Error callback */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    CAN_AppPrint("CAN error detected\r\n");
}
