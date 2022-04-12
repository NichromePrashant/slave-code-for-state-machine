/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : Config_CMT3.c
* Version      : 2.1.0
* Device(s)    : R5F52315AxFL
* Description  : This file implements device driver for Config_CMT3.
* Creation Date: 2022-04-04
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_CMT3.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_CMT3_Create
* Description  : This function initializes the CMT3 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMT3_Create(void)
{
    /* Disable CMI3 interrupt */
    IEN(CMT3,CMI3) = 0U;

    /* Cancel CMT stop state in LPC */
    MSTP(CMT3) = 0U;

    /* Set control registers */
    CMT3.CMCR.WORD = _0001_CMT_CMCR_CLOCK_PCLK32 | _0040_CMT_CMCR_CMIE_ENABLE | _0080_CMT_CMCR_DEFAULT;

    /* Set compare match register */
    CMT3.CMCOR = _0225_CMT3_CMCOR_VALUE;

    /* Set CMI3 priority level */
    IPR(CMT3,CMI3) = _0A_CMT_PRIORITY_LEVEL10;

    R_Config_CMT3_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_CMT3_Start
* Description  : This function starts the CMT3 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMT3_Start(void)
{
    /* Enable CMI3 interrupt in ICU */
    IEN(CMT3,CMI3) = 1U;

    /* Start CMT3 count */
    CMT.CMSTR1.BIT.STR3 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_CMT3_Stop
* Description  : This function stop the CMT3 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMT3_Stop(void)
{
    /* Stop CMT3 count */
    CMT.CMSTR1.BIT.STR3 = 0U;

    /* Disable CMI3 interrupt in ICU */
    IEN(CMT3,CMI3) = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
