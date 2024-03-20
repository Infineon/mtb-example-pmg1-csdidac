/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PMG1 MCU: IDAC Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
********************************************************************************
* Copyright 2023, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cybsp.h"
#include "cy_pdl.h"
#include "cy_csdidac.h"
#include <stdio.h>
#include <inttypes.h>

/*******************************************************************************
* Macros
*******************************************************************************/
/* UART Debug Print */
#define DEBUG_PRINT             (0u)

/* CY Assert Failure */
#define CY_ASSERT_FAILED        (0u)

/* Current minimum value (in nA) */
#define CURRENT_MIN_VALUE       (0u)

/* Current maximum value (in nA) */
#define CURRENT_MAX_VALUE       (609600u)

/* Current Increment while pressing switch (in nA) */
#define CURRENT_INCREMENT_VALUE (4800u)

/* LED Current (in nA) */
#define LED_CURRENT_MAX_VALUE   (-609600)

/* LED Toggle (in milliseconds) */
#define LED_TOGGLE_DELAY_MS     (1000u)

/*******************************************************************************
* Global Variables
*******************************************************************************/
cy_stc_csdidac_context_t csdidac_context;

long current_value = CURRENT_MIN_VALUE;

/* User Switch Interrupt Configuration */
const cy_stc_sysint_t User_Switch_intr_config =
{
    .intrSrc = CYBSP_USER_BTN_IRQ,       /* Source of interrupt signal */
    .intrPriority = 3u,                 /* Interrupt priority */
};

#if DEBUG_PRINT
/* Debug flag in Switch Interrupt Handler */
volatile uint8_t SwitchPressDebugFlag = 0;
#endif /* DEBUG_PRINT */

void User_Switch_Interrupt_Handler(void);

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
#if DEBUG_PRINT
/* Structure for UART context */
cy_stc_scb_uart_context_t CYBSP_UART_context;

/* Variable used for tracking the print status */
volatile bool ENTER_LOOP = true;

/*******************************************************************************
* Function Name: check_status
********************************************************************************
* Summary:
*  Prints the error message.
*
* Parameters:
*  error_msg - message to print if any error encountered.
*  status - status obtained after evaluation.
*
* Return:
*  void
*
*******************************************************************************/
void check_status(char *message, cy_rslt_t status)
{
    char error_msg[100];

    sprintf(error_msg, "Error Code: 0x%08" PRIX32 " n", status);

    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\r\n=====================================================\r\n");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\nFAIL: ");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, message);
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\r\n");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, error_msg);
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\r\n=====================================================\r\n");
}
#endif /* DEBUG_PRINT */

/*******************************************************************************
* Function Name: User_Switch_Interrupt_Handler
********************************************************************************
*
* Summary:
*  This function is executed when interrupt is triggered through the user switch press.
*
*******************************************************************************/
void User_Switch_Interrupt_Handler(void)
{
    Cy_CSDIDAC_OutputEnable (CY_CSDIDAC_A, current_value, &csdidac_context);

    /* Incrementing the current value */
    current_value += CURRENT_INCREMENT_VALUE;

    if (current_value > CURRENT_MAX_VALUE)
    {
        current_value = CURRENT_MIN_VALUE;
    }

#if DEBUG_PRINT
    /* Set Switch press debug flag to 1 */
    SwitchPressDebugFlag = 1;
#endif /* DEBUG_PRINT */

    /* Clear the Interrupt */
    Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_NUM);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* System entrance point. This function performs
*    1. Intial setup of device
*    2. Enables Switch Interrupt and UART
*    3. Enables CSDIDAC
*    4. Increases current while pressing Switch (Source mode)
*    5. Toggles LED (sink mode)
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    cy_en_sysint_status_t intr_result;
    cy_en_csdidac_status_t csdidac_status;
    long led_current_value = LED_CURRENT_MAX_VALUE;
    bool led_control = true;
    uint32_t toggle_delay = 0u;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize Switch GPIO interrupt */
    intr_result = Cy_SysInt_Init(&User_Switch_intr_config, &User_Switch_Interrupt_Handler);
    if (intr_result != CY_SYSINT_SUCCESS)
    {
        CY_ASSERT(CY_ASSERT_FAILED);
    }

    /* Clear any pending interrupt and enable the User Switch Interrupt */
    NVIC_ClearPendingIRQ(User_Switch_intr_config.intrSrc);
    NVIC_EnableIRQ(User_Switch_intr_config.intrSrc);

#if DEBUG_PRINT
    /* Configure and enable the UART peripheral */
    Cy_SCB_UART_Init(CYBSP_UART_HW, &CYBSP_UART_config, &CYBSP_UART_context);
    Cy_SCB_UART_Enable(CYBSP_UART_HW);

    /* Sequence to clear screen */
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\x1b[2J\x1b[;H");

    /* Print "IDAC (Current digital to analog converter)" */
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "****************** ");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "EZ-PDTM PMG1 MCU: IDAC (Current digital to analog converter)");
    Cy_SCB_UART_PutString(CYBSP_UART_HW, "****************** \r\n\n");

    Cy_SCB_UART_PutString(CYBSP_UART_HW, "Sourcing current:\r\n");
#endif /* DEBUG_PRINT */

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize the CSD IDAC */
    csdidac_status = Cy_CSDIDAC_Init(&CYBSP_CSD_csdidac_config, &csdidac_context);

    if (csdidac_status != CY_CSDIDAC_SUCCESS)
    {
#if DEBUG_PRINT
        check_status ("API Cy_CSDIDAC_Init failed with error code", csdidac_status);
#endif /* DEBUG_PRINT */
        CY_ASSERT (CY_ASSERT_FAILED);
    }

    for (;;)
    {
        /* CSDIDAC - Source mode */
#if DEBUG_PRINT
        if (SwitchPressDebugFlag)
        {
            char str[10];
            sprintf (str, "%ld\r\n", current_value);

            Cy_SCB_UART_PutArray (CYBSP_UART_HW, str, strlen(str));

            /* Clearing the Switch Press Debug Flag */
            SwitchPressDebugFlag = 0;
        }
#endif /* DEBUG_PRINT */

        /* CSDIDAC - Sink mode */
        if (LED_TOGGLE_DELAY_MS == toggle_delay)
        {
            if (led_control)
            {
                for (;led_current_value <= 0; led_current_value += CURRENT_INCREMENT_VALUE)
                {
                    Cy_CSDIDAC_OutputEnable (CY_CSDIDAC_B, led_current_value, &csdidac_context);
                    Cy_SysLib_Delay(25);
                }
            }
            else
            {
                Cy_CSDIDAC_OutputDisable (CY_CSDIDAC_B, &csdidac_context);
                led_current_value = LED_CURRENT_MAX_VALUE;
            }

            led_control = !led_control;
            toggle_delay = 0;
        }

        Cy_SysLib_Delay (LED_TOGGLE_DELAY_MS);
        toggle_delay += LED_TOGGLE_DELAY_MS;

#if DEBUG_PRINT
        if (ENTER_LOOP)
        {
            Cy_SCB_UART_PutString(CYBSP_UART_HW, "Entered for loop\r\n");
            ENTER_LOOP = false;
        }
#endif /* DEBUG_PRINT */
    }
}

/* [] END OF FILE */
