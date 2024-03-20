/***************************************************************************//**
* \file cy_csdidac.h
* \version 2.10
*
* \brief
* This file provides the function prototypes and constants specific
* to the CSDIDAC middleware.
*
********************************************************************************
* \copyright
* Copyright 2019-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CSDIDAC_H)
#define CY_CSDIDAC_H

#include "cy_device_headers.h"
#include "cy_csd.h"

#if (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2))

/* The C binding of definitions to build with the C++ compiler. */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
* \addtogroup group_csdidac_macros
* \{
*/
/** Middleware major version */
#define CY_CSDIDAC_MW_VERSION_MAJOR             (2)

/** Middleware minor version */
#define CY_CSDIDAC_MW_VERSION_MINOR             (10)

/** Middleware version */
#define CY_CSDIDAC_MW_VERSION                   (210)

/** CSDIDAC ID. The user can identify the CSDIDAC middleware error codes by this macro. */
#define CY_CSDIDAC_ID                           (CY_PDL_DRV_ID(0x44u))

/**
* The CSDIDAC max code value. The user provides the code
* parameter for the Cy_CSDIDAC_OutputEnableExt() function
* in the range from 0u to CY_CSDIDAC_MAX_CODE.
*/
#define CY_CSDIDAC_MAX_CODE                     (127u)

/**
* The CSDIDAC max output current value. The user provides
* the value of the current parameter for the Cy_CSDIDAC_OutputEnable()
* function in range from 0 to +/-(CY_CSDIDAC_MAX_CURRENT_NA).
*/
#define CY_CSDIDAC_MAX_CURRENT_NA               (609600uL)

/** \} group_csdidac_macros */

/***************************************
* Enumerated Types and Parameters
***************************************/
/**
* \addtogroup group_csdidac_enums
* \{
*/

/** CSDIDAC return enumeration type */
typedef enum
{
    CY_CSDIDAC_SUCCESS           = (0u),
                                            /**< The operation executed successfully. */
    CY_CSDIDAC_BAD_PARAM         = (CY_CSDIDAC_ID + (uint32_t)CY_PDL_STATUS_ERROR + 1u),
                                            /**<
                                             * An input parameter is invalid.
                                             * The user checks whether all
                                             * the input parameters are valid.
                                             */
    CY_CSDIDAC_HW_BUSY           = (CY_CSDIDAC_ID + (uint32_t)CY_PDL_STATUS_ERROR + 2u),
                                            /**<
                                             * The CSD HW block is busy,
                                             * i.e. any of current channel (A or B)
                                             * is enabled.
                                             */
    CY_CSDIDAC_HW_LOCKED         = (CY_CSDIDAC_ID + (uint32_t)CY_PDL_STATUS_ERROR + 3u),
                                            /**<
                                             * The CSD HW block is acquired and
                                             * locked by other middleware
                                             * or application. The CSDIDAC
                                             * middleware waits for
                                             * the CSD HW block release
                                             * to acquire it for use.
                                             */
    CY_CSDIDAC_HW_FAILURE         = (CY_CSDIDAC_ID + (uint32_t)CY_PDL_STATUS_ERROR + 4u),
                                            /**<
                                             * A CSD HW block failure. The possible reasons:
                                             *  - No peripheral clock assigned to the CSD HW block.
                                             *  - Peripheral clock assigned to the CSD HW block is disabled.
                                             *  - The CSD HW clock frequency is less than 100 kHz.
                                             *  - The configuration CSD HW block registers are corrupted.
                                             *  - The CSD HW block is damaged.
                                             */
    CY_CSDIDAC_BAD_CONFIGURATION  = (CY_CSDIDAC_ID + (uint32_t)CY_PDL_STATUS_ERROR + 5u),
                                            /**<
                                             * The CSDIDAC configuration structure initialization issue.
                                             * The possible reasons:
                                             *  - The base pointer is initialized with NULL.
                                             *  - The csdCxtPtr pointer is initialized with NULL.
                                             *  - The configA or configB fields are not enumerators of the
                                             *    \ref cy_en_csdidac_channel_config_t type.
                                             *  - The ptrPinA (ptrPinB) field is initialized with NULL
                                             *    when configA (configB) is initialized with \ref CY_CSDIDAC_GPIO.
                                             *  - The configA and configB fields are initialized with \ref CY_CSDIDAC_DISABLED
                                             *    simultaneously.
                                             *  - The configA and configB fields are initialized with \ref CY_CSDIDAC_JOIN
                                             *    simultaneously.
                                             *  - The configA (configB) field is initialized with \ref CY_CSDIDAC_JOIN
                                             *    when configB (configA) is initialized with \ref CY_CSDIDAC_DISABLED.
                                             */
} cy_en_csdidac_status_t;

/**
* The CSDIDAC output current LSB enumeration type. The user can choose
* LSB when the Cy_CSDIDAC_OutputEnableExt() function is called and
* can check which LSB was chosen by the Cy_CSDIDAC_OutputEnable()
* function in the cy_stc_csdidac_context_t structure.
*/

typedef enum
{
    CY_CSDIDAC_LSB_37_IDX   = 0u,           /**< Index for 37.5 nA LSB */
    CY_CSDIDAC_LSB_75_IDX   = 1u,           /**< Index for 75.0 nA LSB */
    CY_CSDIDAC_LSB_300_IDX  = 2u,           /**< Index for 0.3 uA LSB */
    CY_CSDIDAC_LSB_600_IDX  = 3u,           /**< Index for 0.6 uA LSB */
    CY_CSDIDAC_LSB_2400_IDX = 4u,           /**< Index for 2.4 uA LSB */
    CY_CSDIDAC_LSB_4800_IDX = 5u,           /**< Index for 4.8 uA LSB */
}cy_en_csdidac_lsb_t;

/**
* The CSDIDAC polarity enumeration type. The user can choose the polarity
* when the Cy_CSDIDAC_OutputEnableExt() function is called and can
* check which polarity was chosen by the Cy_CSDIDAC_OutputEnable()
* function in the cy_stc_csdidac_context_t structure.
*/
typedef enum
{
    CY_CSDIDAC_SOURCE       = 0u,           /**< Source polarity */
    CY_CSDIDAC_SINK         = 1u,           /**< Sink polarity */
}cy_en_csdidac_polarity_t;

/**
* The CSDIDAC channel enabling enumeration type. The user can check which
* channel (A or B or both) is currently enabled
* in the cy_stc_csdidac_context_t structure.
*/
typedef enum
{
    CY_CSDIDAC_DISABLE      = 0u,           /**< The channel is disabled. */
    CY_CSDIDAC_ENABLE       = 1u,           /**< The channel is enabled. */
}cy_en_csdidac_state_t;

/**
* The CSDIDAC choosing enumeration type. The user can choose channel A or B
* to operate with the Cy_CSDIDAC_OutputEnableExt(), Cy_CSDIDAC_OutputDisable(),
* or Cy_CSDIDAC_OutputEnable() functions.
*/
typedef enum
{
  CY_CSDIDAC_A              = 0uL,          /**< The IDAC A is chosen for an operation */
  CY_CSDIDAC_B              = 1uL,          /**< The IDAC B is chosen for an operation */
  CY_CSDIDAC_AB             = 2uL,          /**< Both IDACs are chosen for an operation */
} cy_en_csdidac_choice_t;

/**
* The CSDIDAC channel configuration defines either disabled or enabled with
* specific routing.
*/
typedef enum
{
    CY_CSDIDAC_DISABLED   = 0u,             /**< The IDAC channel is disabled. */
    CY_CSDIDAC_GPIO       = 1u,             /**< The IDAC channel is enabled and routed to a pin. */
    CY_CSDIDAC_AMUX       = 2u,             /**< The IDAC channel is enabled and routed to a corresponding analog bus. */
    CY_CSDIDAC_JOIN       = 3u,             /**< The IDAC channel is enabled and routed to the other IDAC channel. */
}cy_en_csdidac_channel_config_t;

/** \} group_csdidac_enums */


/***************************************
* Data Structure Definitions
***************************************/
/**
* \addtogroup group_csdidac_data_structures
* \{
*/

/** The CSDIDAC pin structure. */
typedef struct {
    GPIO_PRT_Type * ioPcPtr;                /**< The pointer to the channel IO PC register. */
    uint8_t pin;                            /**< The channel IO pin. */
} cy_stc_csdidac_pin_t;

/** The CSDIDAC configuration structure */
typedef struct
{
    CSD_Type * base;                        /**< The pointer to the CSD HW Block. */
    cy_stc_csd_context_t * csdCxtPtr;       /**< The pointer to the context of the CSD driver. */
    cy_en_csdidac_channel_config_t configA; /**< The IDAC A channel configuration. */
    cy_en_csdidac_channel_config_t configB; /**< The IDAC B channel configuration. */
    const cy_stc_csdidac_pin_t * ptrPinA;   /**< The pointer to the IDAC A pin structure. */
    const cy_stc_csdidac_pin_t * ptrPinB;   /**< The pointer to the IDAC B pin structure. */
    uint32_t cpuClk;                        /**< CPU Clock in Hz. */
    uint8_t csdInitTime;                    /**< The CSD HW Block initialization time. */
} cy_stc_csdidac_config_t;

/** The CSDIDAC context structure, that contains the internal middleware data. */
typedef struct{
    cy_stc_csdidac_config_t cfgCopy;        /**< A configuration structure copy. */
    cy_en_csdidac_polarity_t polarityA;     /**< The current IdacA polarity. */
    cy_en_csdidac_lsb_t lsbA;               /**< The current IdacA LSB. */
    uint8_t codeA;                          /**< The current IdacA code. */
    cy_en_csdidac_state_t channelStateA;    /**< The IDAC channel A is enabled. */
    cy_en_csdidac_polarity_t polarityB;     /**< The current IdacB polarity. */
    cy_en_csdidac_lsb_t lsbB;               /**< The current IdacB LSB. */
    uint8_t codeB;                          /**< The current IdacB code. */
    cy_en_csdidac_state_t channelStateB;    /**< The IDAC channel B is enabled. */
}cy_stc_csdidac_context_t;

/** \} group_csdidac_data_structures */


/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/**
* \addtogroup group_csdidac_functions
* \{
*/
cy_en_csdidac_status_t Cy_CSDIDAC_Init(
                const cy_stc_csdidac_config_t * config,
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_DeInit(
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_WriteConfig(
                const cy_stc_csdidac_config_t * config,
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_Wakeup(
                const cy_stc_csdidac_context_t * context);
cy_en_syspm_status_t Cy_CSDIDAC_DeepSleepCallback(
                cy_stc_syspm_callback_params_t * callbackParams,
                cy_en_syspm_callback_mode_t mode);
cy_en_csdidac_status_t Cy_CSDIDAC_Save(
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_Restore(
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_OutputEnable(
                cy_en_csdidac_choice_t ch,
                int32_t current,
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_OutputEnableExt(
                cy_en_csdidac_choice_t outputCh,
                cy_en_csdidac_polarity_t polarity,
                cy_en_csdidac_lsb_t lsbIndex,
                uint32_t idacCode,
                cy_stc_csdidac_context_t * context);
cy_en_csdidac_status_t Cy_CSDIDAC_OutputDisable(
                cy_en_csdidac_choice_t ch,
                cy_stc_csdidac_context_t * context);

/** \} group_csdidac_functions */


/*******************************************************************************
* Function Name: Cy_CSDIDAC_IsIdacLsbValid
****************************************************************************//**
*
* Performs verification if the value passed through the idacLsbVal
* parameter is the enumerator of the \ref cy_en_csdidac_lsb_t type.
*
* \param idacLsbVal
* The input value for verification.
*
* \return status
* Returns the verification status:
* - true  - Indicates that the verification succeeded.
* - false - Indicates that the verification failed.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_CSDIDAC_IsIdacLsbValid(cy_en_csdidac_lsb_t idacLsbVal)
{
    bool retVal;

    switch(idacLsbVal)
    {
        case CY_CSDIDAC_LSB_37_IDX:
        case CY_CSDIDAC_LSB_75_IDX:
        case CY_CSDIDAC_LSB_300_IDX:
        case CY_CSDIDAC_LSB_600_IDX:
        case CY_CSDIDAC_LSB_2400_IDX:
        case CY_CSDIDAC_LSB_4800_IDX:
            retVal = true;
            break;

        default:
            retVal = false;
            break;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: Cy_CSDIDAC_IsIdacPolarityValid
****************************************************************************//**
*
* Performs verification if the value passed through the idacPolarityVal
* parameter is the enumerator of the \ref cy_en_csdidac_polarity_t type.
*
* \param idacPolarityVal
* The input value for verification.
*
* \return status
* Returns the verification status:
* - true  - Indicates that the verification succeeded.
* - false - Indicates that the verification failed.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_CSDIDAC_IsIdacPolarityValid(cy_en_csdidac_polarity_t idacPolarityVal)
{
    bool retVal;

    switch(idacPolarityVal)
    {
        case CY_CSDIDAC_SOURCE:
        case CY_CSDIDAC_SINK:
            retVal = true;
            break;

        default:
            retVal = false;
            break;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: Cy_CSDIDAC_IsIdacChoiceValid
****************************************************************************//**
*
* Performs verification if the value passed through the idacChoiceVal
* parameter is the enumerator of the \ref cy_en_csdidac_choice_t type.
*
* Performs verification that both IDACs are not disabled if CY_CSDIDAC_AB is
* passed through the idacChoiceVal parameter.
*
* \param idacChoiceVal
* The input value for verification.
*
* \param idacAConfigVal
* The IDACA channel configuration.
*
* \param idacBConfigVal
* The IDACB channel configuration.
*
* \return status
* Returns the verification status:
* - true  - Indicates that the verification succeeded.
* - false - Indicates that the verification failed.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_CSDIDAC_IsIdacChoiceValid(cy_en_csdidac_choice_t idacChoiceVal,
                                                  cy_en_csdidac_channel_config_t idacAConfigVal,
                                                  cy_en_csdidac_channel_config_t idacBConfigVal)
{
    bool retVal;

    switch(idacChoiceVal)
    {
        case CY_CSDIDAC_A:
        case CY_CSDIDAC_B:
            retVal = true;
            break;
        case CY_CSDIDAC_AB:
            if((CY_CSDIDAC_DISABLED != idacAConfigVal) &&
               (CY_CSDIDAC_DISABLED != idacBConfigVal))
            {
                retVal = true;
            }
            else
            {
                retVal = false;
            }
            break;

        default:
            retVal = false;
            break;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: Cy_CSDIDAC_IsIdacChConfigValid
****************************************************************************//**
*
* Performs verification if the value passed through the idacChConfigVal
* parameter is the enumerator of the \ref cy_en_csdidac_channel_config_t type.
*
* \param idacChConfigVal
* The input value for verification.
*
* \return status
* Returns the verification status:
* - true  - Indicates that the verification succeeded.
* - false - Indicates that the verification failed.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_CSDIDAC_IsIdacChConfigValid(cy_en_csdidac_channel_config_t idacChConfigVal)
{
    bool retVal;

    switch(idacChConfigVal)
    {
        case CY_CSDIDAC_DISABLED:
        case CY_CSDIDAC_GPIO:
        case CY_CSDIDAC_AMUX:
        case CY_CSDIDAC_JOIN:
            retVal = true;
            break;

        default:
            retVal = false;
            break;
    }

    return(retVal);
}


/*******************************************************************************
* Function Name: Cy_CSDIDAC_IsIdacConfigValid
****************************************************************************//**
*
* Performs verification of the CSDIDAC data structure initialization.
*
* \param config
* The pointer to the CSDIDSC configuration structure \ref cy_stc_csdidac_config_t.
*
* \return status
* Returns the verification status:
* - true  - Indicates that the verification succeeded.
* - false - Indicates that the verification failed.
*
*******************************************************************************/
__STATIC_INLINE bool Cy_CSDIDAC_IsIdacConfigValid(const cy_stc_csdidac_config_t * config)
{
    bool retVal = true;

    if((NULL != config->base) && (NULL != config->csdCxtPtr))
    {
        if(((CY_CSDIDAC_GPIO == config->configA) && (NULL == config->ptrPinA)) ||
           ((CY_CSDIDAC_GPIO == config->configB) && (NULL == config->ptrPinB)) ||
           ((CY_CSDIDAC_DISABLED == config->configA) && (CY_CSDIDAC_DISABLED == config->configB)) ||
           ((CY_CSDIDAC_JOIN == config->configA) && (CY_CSDIDAC_DISABLED == config->configB)) ||
           ((CY_CSDIDAC_JOIN == config->configB) && (CY_CSDIDAC_DISABLED == config->configA)) ||
           (false == Cy_CSDIDAC_IsIdacChConfigValid(config->configA)) ||
           (false == Cy_CSDIDAC_IsIdacChConfigValid(config->configB)) ||
           ((CY_CSDIDAC_JOIN == config->configA) && (CY_CSDIDAC_JOIN == config->configB)))
        {
            retVal = false;
        }
    }
    else
    {
        retVal = false;
    }

    return(retVal);
}




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2)) */

#endif /* CY_CSDIDAC_H */


/* [] END OF FILE */
