/*
 * can_code.c
 *
 *  Created on: Feb 4, 2022
 *      Author: Dnyaneshwar
 */

/***********************************************************************************************************************
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : can_code.c
* Device(s)    : RSKRX231
* Tool-Chain   : Renesas RXC
* Description  : This is file contains RSCAN initialization routines and interrupt
*                callback functions which support the main.c application.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*           08.09.2016 1.00     Initial Release
***********************************************************************************************************************/

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_smc_entry.h"
#include <stdio.h>              // sprintf()
#include <string.h>             // strlen()
#include "platform.h"
#include "Config_CMT0.h"
#include "Config_CMT1.h"
#include "Config_CMT2.h"
#include "Config_CMT3.h"
#include "Config_ICU.h"

#include "r_rscan_rx_if.h"      // The RSCAN FIT module API interface file
#include "r_rscan_rx_config.h"

#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"

#include "can_variables.h"
#include "can_functions.h"

#include "global_variables.h"
#include "global_functions.h"

#include "manual_functions.h"
#include "manual_variables.h"

#include "calibration_variables.h"
#include "calibration_functions.h"


#include "runcycle_variables.h"
#include "runcycle_functions.h"

#include "stepper_variables.h"
#include "stepper_functions.h"

#include "adc_cs5532_variables.h"
#include "adc_cs5532_functions.h"

#include "delay_functions.h"

/*****************************************************************************

Private global variables and functions
******************************************************************************/

/*****************************************************************************
* Function Name : void Init_CAN(void)
* Description   : This function initializes the MCU pins for RSCAN channel 0.
*                 It then opens the CAN driver and configures it for a 500kHz
*                 network, the use of a receive FIFO, a receive rule that
*                 accepts all messages, and then puts the channel into
*                 communications mode. Also, a transmit message buffer is
*                 initialized.
* Arguments     : none
* Return Value  : none
******************************************************************************/
void Initialize_CAN_Communication(void)
{
   /* Configure CAN port for channel 0 */
	can_rx231_init_ports();

    /* Open driver. Timestamp source is arbitrary. */
    config.timestamp_src = CAN_TIMESTAMP_SRC_HALF_PCLK;
    config.timestamp_div = CAN_TIMESTAMP_DIV_1024;

    err = R_CAN_Open(&config, MainCallback);

    if (err != CAN_SUCCESS)
    {
        while(1);
    }

    /* Initialize channel 0 (includes mailboxes) */
    baud.prescaler = CAN_RSK_8MHZ_XTAL_500KBPS_PRESCALER;
    baud.tseg1 = CAN_RSK_8MHZ_XTAL_500KBPS_TSEG1;
    baud.tseg2 = CAN_RSK_8MHZ_XTAL_500KBPS_TSEG2;
    baud.sjw = CAN_RSK_8MHZ_XTAL_500KBPS_SJW;

    err = R_CAN_InitChan(CAN_CH0, &baud, ChanCallback);     // CAN_CFG_CLOCK_SOURCE 1

    if (err != CAN_SUCCESS)
    {
        while(1);
    }

    /* Initialize a receive FIFO */
    err = R_CAN_ConfigFIFO(CAN_BOX_RXFIFO_0,
                           CAN_FIFO_THRESHOLD_1,    // set int flag at every message received
                           CAN_BOX_NONE);           // unused field for RXFIFO
    if (err != CAN_SUCCESS)
    {
        while(1) ;
    }

    /* Do not need transmit FIFO */
    /* Do not need history FIFO */

    /*Initialize a receive rule filter for all message IDs */
    g_filter.check_ide = TRUE;    // check the IDE field in message
    g_filter.ide = 0;             // 0 = 11-bit ID
    g_filter.check_rtr = FALSE;   // do not check the RTR field in message

    g_filter.rtr = 0;             // (value does not matter here; not checking)
    g_filter.id = 0x0E;          // (value does not matter here; not checking)
    g_filter.id_mask = 0x000;     // all messages are accepted

    g_filter.min_dlc = 0;         // message data can be 0 to 8 bytes
    g_filter.label   = 0x800;       // arbitrary label (unused)


    /* Add rule to channel 0. Route filtered messages to receive FIFO 0 */
    err = R_CAN_AddRxRule(CAN_CH0, &g_filter, CAN_BOX_RXFIFO_0);
    if (err != CAN_SUCCESS)
    {
        while(1);
    }

    /* Initialization complete. Go to communications mode */
    err = R_CAN_Control(CAN_CMD_SET_MODE_COMM, 0);

    if (err != CAN_SUCCESS)
    {
        while(1) ;
    }

    /* Setup a message to transmit. ID, DLC, and data will be filled in by by user via UART */
    g_txmsg.ide = 0;
    g_txmsg.rtr = 0;
    g_txmsg.one_shot = FALSE;       // do normal retries when sending
   // g_txmsg.log_history = FALSE;    // do not log this message in history FIFO
    g_txmsg.log_history = TRUE;     // log this message in history FIFO*/



    return;
}

/******************************************************************************
* Function Name: can_rx231_init_ports
* Description  : Initializes RX231 port 5 for CAN channel 0
* Arguments    : None
* Return Value : None
*******************************************************************************/
void can_rx231_init_ports(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* init CAN channel 0 */
    PORT1.PODR.BIT.B5 = 1;
    PORT1.PODR.BIT.B4 = 0;
    MPC.P14PFS.BYTE = 0x10;     // Pin Function Select P14 CTXD0
    MPC.P15PFS.BYTE = 0x10;     // Pin Function Select P15 CRXD0
    PORT1.PDR.BIT.B4 = 1;       // set TX pin direction to output
    PORT1.DSCR.BIT.B4 = 1;      // High-drive output
    PORT1.PDR.BIT.B5 = 0;       // set RX pin direction to input (dflt)
    PORT1.PMR.BIT.B4 = 1;       // set TX pin mode to peripheral
    PORT1.PMR.BIT.B5 = 1;       // set RX pin mode to peripheral

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);

    return;
}


/******************************************************************************
* Function Name: MainCallback
* Description  : This function is called from interrupt level whenever a message
*                is received in a receive FIFO or a global error occurs. Global
*                errors include FIFO overflows or a DLC error detection.
* Arguments    : event-
*                   CAN_EVT_RXFIFO_THRESHOLD when threshold messages in an RXFIFO
*                   CAN_EVT_GLOBAL_ERR when a global error is detected
*                p_args-
*                   unused (future)
* Return Value : None
*******************************************************************************/
void MainCallback(can_cb_evt_t event, void *p_args)
{
    uint32_t    mask;
    can_err_t   err;


    if (event == CAN_EVT_RXFIFO_THRESHOLD)
    {

        mask = R_CAN_GetStatusMask(CAN_STAT_FIFO_THRESHOLD, 0, &err);
        if (mask & CAN_MASK_RXFIFO_0)
        {
         //   R_CAN_GetMsg(CAN_BOX_RXFIFO_0, &g_rxmsg);
            g_rxcnt++;
        }
    }

    else if (event == CAN_EVT_GLOBAL_ERR)   // Should never happen with this demo
    {
        /* Get mask of which global error(s) occurred */
        mask = R_CAN_GetStatusMask(CAN_STAT_GLOBAL_ERR, 0, &err);

        if (mask & CAN_MASK_ERR_DLC)
        {
            /* handle DLC error */
        	RSCAN.GERFLL.BIT.DEF = 0;    // dt
        }

        if (mask & CAN_MASK_ERR_FIFO_OVFL)
        {
            mask = R_CAN_GetStatusMask(CAN_STAT_FIFO_OVFL, 0, &err);

            /* check the RXFIFOs, GWFIFO, and HIST_FIFOs in use */
            if (mask & CAN_MASK_RXFIFO_0)
            {
                g_ovfl_cnt++;

                /* clear overflow interrupt flags */
				RSCAN.RFSTS0.BIT.RFMLT = 0;
				RSCAN.RFSTS1.BIT.RFMLT = 0;
				RSCAN0.THLSTS0.BIT.THLELT = 0;

                /* drain FIFO */
                while (R_CAN_GetMsg(CAN_BOX_RXFIFO_0, &g_rxmsg) == CAN_SUCCESS)
                {
                    ;
                }
            }

            if (mask & CAN_MASK_HIST_FIFO)
            {
                g_ovfl_cnt++;

                /* drain FIFO */
                while (R_CAN_GetHistoryEntry(CAN_BOX_HIST_FIFO, &g_entry) == CAN_SUCCESS)
                {
                    ;
                }
            }
        }
    }
}


/******************************************************************************
* Function Name: ChanCallback
* Description  : This function is called from interrupt level whenever a channel
*                transmit, channel error, or gateway receive threshold event
*                occurs
* Arguments    : chan-
*                    Channel in which the interrupt occurred on
*                event-
*                    CAN_EVT_TRANSMIT for mailbox transmit complete, abort
*                           complete, or TX/GW/HIST FIFO at threshold
*                    CAN_EVT_CHANNEL_ERR for channel error detection, which
*                           includes (among others) error passive state entry,
*                           bus off entry, and bus off recovery.
*                    CAN_EVT_GWFIFO_RX_THRESHOLD for GW FIFO at receive
*                           threshhold
* Return Value : None
*******************************************************************************/
void ChanCallback(uint8_t       chan,
                  can_cb_evt_t  event,
                  void          *p_args)
{
    uint32_t   mask;
    can_err_t  err;


    /* NOTE: THESE EVENTS WILL NOT OCCUR IN THIS DEMO, BUT ARE PROVIDED AS A TEMPLATE */

    if (event == CAN_EVT_TRANSMIT)
    {
        /* determine if mailbox transmit completes */
        mask = R_CAN_GetStatusMask(CAN_STAT_CH_TXMBX_SENT, chan, &err);
        if (mask != 0)
        {
            /* only using one mailbox, do not have to check which one */
            // do something; typically send next message
            R_BSP_NOP();
        }

        /* determine if mailbox transmit abort completed */
        mask = R_CAN_GetStatusMask(CAN_STAT_CH_TXMBX_ABORTED, chan, &err);
        if (mask != 0)
        {
            /* only using one mailbox, do not have to check which one */
            // do something
        }

        /* determine if TX, GW, or HIST FIFOs at threshhold */
        mask = R_CAN_GetStatusMask(CAN_STAT_FIFO_THRESHOLD, 0, &err);
        if (mask & CAN_MASK_TXFIFO)
        {
            // do something; typically load more messages into TXFIFO for transmit
        }

        if (mask & CAN_MASK_HIST_FIFO)
        {
            // do something; typically read entries from HIST_FIFO
        }
    }


    else if (event == CAN_EVT_CHANNEL_ERR)
    {
        /* get mask of which errors detected */
        mask = R_CAN_GetStatusMask(CAN_STAT_CH_ERROR, chan, &err);

        /* check only for specific errors */
        if (mask & CAN_MASK_ERR_PASSIVE)
        {
            /* handle error */
            R_BSP_NOP();
        }

        if (mask & CAN_MASK_ERR_BUS_OFF_ENTRY)
        {
            /* handle error */
            R_BSP_NOP();
        }

        if (mask & CAN_MASK_ERR_BUS_OFF_EXIT)
        {
            /* handle recovery */
            R_BSP_NOP();
        }
    }

}


void Check_CAN_Status(void)
{
	uint32_t    status;
	can_err_t   can_err;

	//Check if received CAN message

	if (R_CAN_GetMsg(CAN_BOX_RXFIFO_0, &g_rxmsg) == CAN_SUCCESS)
	{
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Process_Can_Msg(&g_rxmsg);
	}

   /* Check if any CAN channel errors */
	status = R_CAN_GetStatusMask(CAN_STAT_CH_ERROR, CAN_CH0, &can_err);
	if(status!=0)
	{
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
		Clear_Watchdog();
	}
}


void Process_Can_Msg(can_rxmsg_t *rxmsg)
{
		Clear_Watchdog();


		unsigned int temp_command;

		HEADID = CAN_SlaveID;


		if((rxmsg->id) == 0x00)
		{
			Flag_CalibOperation  = 0;
			Flag_ManualOperation = 0;

			Dump     = 0;
			Recharge = 0;

			Dump     = (unsigned int)(rxmsg->data[0]<<8);
			Dump     = (unsigned int)(Dump|(rxmsg->data[1]));

			Recharge = (unsigned int)(rxmsg->data[2]<<8);
			Recharge = (unsigned int)(Recharge|(rxmsg->data[3]));

			Dump     =  Dump&0x0fff;
			Recharge =  Recharge&0x0fff;

			Dump     = (Dump>>(HEADID)-1);
			Dump     =  Dump & 0x1;

			Recharge = (Recharge>>(HEADID)-1);
			Recharge =  Recharge&0x1;

			if((Dump != 0) && (Auto_Tare_Flag == 0) && (Settle_TMR == 0))
			{
				Start_Weight_Cal_Flag      = 1;
				Head_In_Recharge           = 0;
				Head_In_Combi              = 1;
				Flag_RunCyc                = 0;
				Flag_Run_Stage1            = 0;
				Flag_WB_Trig               = 0;
				Flag_RunOperation          = 1;
				LongTime_Not_In_Combi_flag = 0;
				Flag_Run                   = 1;
			}
			else if((Recharge != 0) && (Auto_Tare_Flag == 0) && (Settle_TMR == 0))
			{
				Start_Weight_Cal_Flag      = 1;
				Head_In_Combi              = 0;
				Head_In_Recharge           = 1;
				Flag_RunCyc                = 0;
				Flag_Run_Stage1            = 0;
				Flag_WB_Trig               = 0;
				Flag_RunOperation          = 1;
				LongTime_Not_In_Combi_flag = 0;
				Flag_Run                   = 1;
			}
			else
			{
				Head_In_Combi         = 0;
				Head_In_Recharge      = 0;
				Flag_RunOperation     = 0;
				Flag_Run              = 0;
				Start_Weight_Cal_Flag = 0;
				LongTime_Not_In_Combi_Count++;
			}
		}


		// zeroing

		if((rxmsg->id) == 0X8F + HEADID )
		{
			Auto_Tare_Flag = 1;
			Flag_WB_Trig   = 1;
			Flag_DumpCyc   = 1;
		}


		// write para

		if((rxmsg->id) == 0xAD)
		{
			GAIN = (unsigned int)(rxmsg->data[0]<<8);
			GAIN = (unsigned int)(GAIN|(rxmsg->data[1]));

			SPEED = (unsigned int)(rxmsg->data[2]<<8);
			SPEED = (unsigned int)(SPEED|(rxmsg->data[3]));

			ADSAMPLES = (unsigned int)(rxmsg->data[4]<<8);
			ADSAMPLES = (unsigned int)(ADSAMPLES|(rxmsg->data[5]));

			Diff_Cnt_Val = (unsigned int)(rxmsg->data[6]<<8);
			Diff_Cnt_Val = (unsigned int)(Diff_Cnt_Val|(rxmsg->data[7]));
		}
		else if((rxmsg->id) == 0xAE)
		{
			Stable_Count_Val = (unsigned int)(rxmsg->data[0]<<8);
			Stable_Count_Val = (unsigned int)(Stable_Count_Val|(rxmsg->data[1]));

			Auto_Zero_Interval = (unsigned int)(rxmsg->data[2]<<8);
			Auto_Zero_Interval = (unsigned int)(Auto_Zero_Interval|(rxmsg->data[3]));

			Settle_Time = (unsigned int)(rxmsg->data[6]<<8);
			Settle_Time = (unsigned int)(Settle_Time|(rxmsg->data[7]));
		}
		else if((rxmsg->id) == 0xAF)
		{
			Firing_Angle = (signed int)(rxmsg->data[2]<<8);
			Firing_Angle = (signed int)(Firing_Angle|(rxmsg->data[3]));

			Firing_Angle = ((-80)*Firing_Angle)+8000;

			SD_LF_Time = (unsigned int)(rxmsg->data[4]<<8);
			SD_LF_Time = (unsigned int)(SD_LF_Time|(rxmsg->data[5]));

			VBRT_Cyl = (unsigned int)(rxmsg->data[6]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[7]));
		}
		else if((rxmsg->id) == 0xB1)
		{
			FB_OD_Time = (unsigned int)(rxmsg->data[0]<<8);
			FB_OD_Time = (unsigned int)(FB_OD_Time|(rxmsg->data[1]));

			FB_OT_Time = (unsigned int)(rxmsg->data[2]<<8);
			FB_OT_Time = (unsigned int)(FB_OT_Time|(rxmsg->data[3]));

			WB_OD_Time = (unsigned int)(rxmsg->data[4]<<8);
			WB_OD_Time = (unsigned int)(WB_OD_Time|(rxmsg->data[5]));

			WB_OT_Time = (unsigned int)(rxmsg->data[6]<<8);
			WB_OT_Time = (unsigned int)(WB_OT_Time|(rxmsg->data[7]));
		}
		else if((rxmsg->id) == 0xB3)
		{
			Calib_Slope_9 = (signed int)(rxmsg->data[2]<<8);
			Calib_Slope_9 = (signed int)(Calib_Slope_9|(rxmsg->data[3]));

			Calib_Slope_10 = (signed int)(rxmsg->data[4]<<8);
			Calib_Slope_10 = (signed int)(Calib_Slope_10|(rxmsg->data[5]));

			if(CAN_SlaveID==1)
				CalibSlope = Calib_Slope_1;
			else if(CAN_SlaveID==2)
				CalibSlope = Calib_Slope_2;
			else if(CAN_SlaveID==3)
				CalibSlope = Calib_Slope_3;
			else if(CAN_SlaveID==4)
				CalibSlope = Calib_Slope_4;
			else if(CAN_SlaveID==5)
				CalibSlope = Calib_Slope_5;
			else if(CAN_SlaveID==6)
				CalibSlope = Calib_Slope_6;
			else if(CAN_SlaveID==7)
				CalibSlope = Calib_Slope_7;
			else if(CAN_SlaveID==8)
				CalibSlope = Calib_Slope_8;
			else if(CAN_SlaveID==9)
				CalibSlope = Calib_Slope_9;
			else if(CAN_SlaveID==10)
				CalibSlope = Calib_Slope_10;
		}
		else if((rxmsg->id) == 0xB4)
		{
			Tare_Cnt_1 = (signed int)(rxmsg->data[0]<<8);
			Tare_Cnt_1 = (signed int)(Tare_Cnt_1|(rxmsg->data[1]));

			Tare_Cnt_2 = (signed int)(rxmsg->data[2]<<8);
			Tare_Cnt_2 = (signed int)(Tare_Cnt_2|(rxmsg->data[3]));

			Tare_Cnt_3 = (signed int)(rxmsg->data[4]<<8);
			Tare_Cnt_3 = (signed int)(Tare_Cnt_3|(rxmsg->data[5]));

			Tare_Cnt_4 = (signed int)(rxmsg->data[6]<<8);
			Tare_Cnt_4 = (signed int)(Tare_Cnt_4|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				TareCnt = Tare_Cnt_1;
			else if(CAN_SlaveID==2)
				TareCnt = Tare_Cnt_2;
			else if(CAN_SlaveID==3)
				TareCnt = Tare_Cnt_3;
			else if(CAN_SlaveID==4)
				TareCnt = Tare_Cnt_4;
			else if(CAN_SlaveID==5)
				TareCnt = Tare_Cnt_5;
			else if(CAN_SlaveID==6)
				TareCnt = Tare_Cnt_6;
			else if(CAN_SlaveID==7)
				TareCnt = Tare_Cnt_7;
			else if(CAN_SlaveID==8)
				TareCnt = Tare_Cnt_8;
			else if(CAN_SlaveID==9)
				TareCnt = Tare_Cnt_9;
			else if(CAN_SlaveID==10)
				TareCnt = Tare_Cnt_10;
		}
		else if((rxmsg->id) == 0xB5)
		{
			Tare_Cnt_5 = (signed int)(rxmsg->data[0]<<8);
			Tare_Cnt_5 = (signed int)(Tare_Cnt_5|(rxmsg->data[1]));

			Tare_Cnt_6 = (signed int)(rxmsg->data[2]<<8);
			Tare_Cnt_6 = (signed int)(Tare_Cnt_6|(rxmsg->data[3]));

			Tare_Cnt_7 = (signed int)(rxmsg->data[4]<<8);
			Tare_Cnt_7 = (signed int)(Tare_Cnt_7|(rxmsg->data[5]));

			Tare_Cnt_8 = (signed int)(rxmsg->data[6]<<8);
			Tare_Cnt_8 = (signed int)(Tare_Cnt_8|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				TareCnt = Tare_Cnt_1;
			else if(CAN_SlaveID==2)
				TareCnt = Tare_Cnt_2;
			else if(CAN_SlaveID==3)
				TareCnt = Tare_Cnt_3;
			else if(CAN_SlaveID==4)
				TareCnt = Tare_Cnt_4;
			else if(CAN_SlaveID==5)
				TareCnt = Tare_Cnt_5;
			else if(CAN_SlaveID==6)
				TareCnt = Tare_Cnt_6;
			else if(CAN_SlaveID==7)
				TareCnt = Tare_Cnt_7;
			else if(CAN_SlaveID==8)
				TareCnt = Tare_Cnt_8;
			else if(CAN_SlaveID==9)
				TareCnt = Tare_Cnt_9;
			else if(CAN_SlaveID==10)
				TareCnt = Tare_Cnt_10;
		}
		else if((rxmsg->id) == 0xB6)
		{
			Tare_Cnt_9 = (signed int)(rxmsg->data[0]<<8);
			Tare_Cnt_9 = (signed int)(Tare_Cnt_9|(rxmsg->data[1]));

			Tare_Cnt_10 = (signed int)(rxmsg->data[2]<<8);
			Tare_Cnt_10 = (signed int)(Tare_Cnt_10|(rxmsg->data[3]));

			Calib_Cnt_1 = (signed int)(rxmsg->data[4]<<8);
			Calib_Cnt_1 = (signed int)(Calib_Cnt_1|(rxmsg->data[5]));

			Calib_Cnt_2 = (signed int)(rxmsg->data[6]<<8);
			Calib_Cnt_2 = (signed int)(Calib_Cnt_2|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				TareCnt = Tare_Cnt_1;
			else if(CAN_SlaveID==2)
				TareCnt = Tare_Cnt_2;
			else if(CAN_SlaveID==3)
				TareCnt = Tare_Cnt_3;
			else if(CAN_SlaveID==4)
				TareCnt = Tare_Cnt_4;
			else if(CAN_SlaveID==5)
				TareCnt = Tare_Cnt_5;
			else if(CAN_SlaveID==6)
				TareCnt = Tare_Cnt_6;
			else if(CAN_SlaveID==7)
				TareCnt = Tare_Cnt_7;
			else if(CAN_SlaveID==8)
				TareCnt = Tare_Cnt_8;
			else if(CAN_SlaveID==9)
				TareCnt = Tare_Cnt_9;
			else if(CAN_SlaveID==10)
				TareCnt = Tare_Cnt_10;

			if(CAN_SlaveID==1)
				CalibCnt = Calib_Cnt_1;
			else if(CAN_SlaveID==2)
				CalibCnt = Calib_Cnt_2;
			else if(CAN_SlaveID==3)
				CalibCnt = Calib_Cnt_3;
			else if(CAN_SlaveID==4)
				CalibCnt = Calib_Cnt_4;
			else if(CAN_SlaveID==5)
				CalibCnt = Calib_Cnt_5;
			else if(CAN_SlaveID==6)
				CalibCnt = Calib_Cnt_6;
			else if(CAN_SlaveID==7)
				CalibCnt = Calib_Cnt_7;
			else if(CAN_SlaveID==8)
				CalibCnt = Calib_Cnt_8;
			else if(CAN_SlaveID==9)
				CalibCnt = Calib_Cnt_9;
			else if(CAN_SlaveID==10)
				CalibCnt = Calib_Cnt_10;
		}
		else if((rxmsg->id) == 0xB7)
		{
			Calib_Cnt_3 = (signed int)(rxmsg->data[0]<<8);
			Calib_Cnt_3 = (signed int)(Calib_Cnt_3|(rxmsg->data[1]));

			Calib_Cnt_4 = (signed int)(rxmsg->data[2]<<8);
			Calib_Cnt_4 = (signed int)(Calib_Cnt_4|(rxmsg->data[3]));

			Calib_Cnt_5 = (signed int)(rxmsg->data[4]<<8);
			Calib_Cnt_5 = (signed int)(Calib_Cnt_5|(rxmsg->data[5]));

			Calib_Cnt_6 = (signed int)(rxmsg->data[6]<<8);
			Calib_Cnt_6 = (signed int)(Calib_Cnt_6|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				CalibCnt = Calib_Cnt_1;
			else if(CAN_SlaveID==2)
				CalibCnt = Calib_Cnt_2;
			else if(CAN_SlaveID==3)
				CalibCnt = Calib_Cnt_3;
			else if(CAN_SlaveID==4)
				CalibCnt = Calib_Cnt_4;
			else if(CAN_SlaveID==5)
				CalibCnt = Calib_Cnt_5;
			else if(CAN_SlaveID==6)
				CalibCnt = Calib_Cnt_6;
			else if(CAN_SlaveID==7)
				CalibCnt = Calib_Cnt_7;
			else if(CAN_SlaveID==8)
				CalibCnt = Calib_Cnt_8;
			else if(CAN_SlaveID==9)
				CalibCnt = Calib_Cnt_9;
			else if(CAN_SlaveID==10)
				CalibCnt = Calib_Cnt_10;
		}
		else if((rxmsg->id) == 0xB8)
		{
			Calib_Cnt_7 = (signed int)(rxmsg->data[0]<<8);
			Calib_Cnt_7 = (signed int)(Calib_Cnt_7|(rxmsg->data[1]));

			Calib_Cnt_8 = (signed int)(rxmsg->data[2]<<8);
			Calib_Cnt_8 = (signed int)(Calib_Cnt_8|(rxmsg->data[3]));

			Calib_Cnt_9 = (signed int)(rxmsg->data[4]<<8);
			Calib_Cnt_9 = (signed int)(Calib_Cnt_9|(rxmsg->data[5]));

			Calib_Cnt_10 = (signed int)(rxmsg->data[6]<<8);
			Calib_Cnt_10 = (signed int)(Calib_Cnt_10|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				CalibCnt = Calib_Cnt_1;
			else if(CAN_SlaveID==2)
				CalibCnt = Calib_Cnt_2;
			else if(CAN_SlaveID==3)
				CalibCnt = Calib_Cnt_3;
			else if(CAN_SlaveID==4)
				CalibCnt = Calib_Cnt_4;
			else if(CAN_SlaveID==5)
				CalibCnt = Calib_Cnt_5;
			else if(CAN_SlaveID==6)
				CalibCnt = Calib_Cnt_6;
			else if(CAN_SlaveID==7)
				CalibCnt = Calib_Cnt_7;
			else if(CAN_SlaveID==8)
				CalibCnt = Calib_Cnt_8;
			else if(CAN_SlaveID==9)
				CalibCnt = Calib_Cnt_9;
			else if(CAN_SlaveID==10)
				CalibCnt = Calib_Cnt_10;
		}
		else if((rxmsg->id) == 0xB9)
		{
			Calib_Slope_1 = (signed int)(rxmsg->data[0]<<8);
			Calib_Slope_1 = (signed int)(Calib_Slope_1|(rxmsg->data[1]));

			Calib_Slope_2 = (signed int)(rxmsg->data[2]<<8);
			Calib_Slope_2 = (signed int)(Calib_Slope_2|(rxmsg->data[3]));

			Calib_Slope_3 = (signed int)(rxmsg->data[4]<<8);
			Calib_Slope_3 = (signed int)(Calib_Slope_3|(rxmsg->data[5]));

			Calib_Slope_4 = (signed int)(rxmsg->data[6]<<8);
			Calib_Slope_4 = (signed int)(Calib_Slope_4|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				CalibSlope = Calib_Slope_1;
			else if(CAN_SlaveID==2)
				CalibSlope = Calib_Slope_2;
			else if(CAN_SlaveID==3)
				CalibSlope = Calib_Slope_3;
			else if(CAN_SlaveID==4)
				CalibSlope = Calib_Slope_4;
			else if(CAN_SlaveID==5)
				CalibSlope = Calib_Slope_5;
			else if(CAN_SlaveID==6)
				CalibSlope = Calib_Slope_6;
			else if(CAN_SlaveID==7)
				CalibSlope = Calib_Slope_7;
			else if(CAN_SlaveID==8)
				CalibSlope = Calib_Slope_8;
			else if(CAN_SlaveID==9)
				CalibSlope = Calib_Slope_9;
			else if(CAN_SlaveID==10)
				CalibSlope = Calib_Slope_10;
		}
		else if((rxmsg->id) == 0xC1)
		{
			Calib_Slope_5 = (signed int)(rxmsg->data[0]<<8);
			Calib_Slope_5 = (signed int)(Calib_Slope_5|(rxmsg->data[1]));

			Calib_Slope_6 = (signed int)(rxmsg->data[2]<<8);
			Calib_Slope_6 = (signed int)(Calib_Slope_6|(rxmsg->data[3]));

			Calib_Slope_7 = (signed int)(rxmsg->data[4]<<8);
			Calib_Slope_7 = (signed int)(Calib_Slope_7|(rxmsg->data[5]));

			Calib_Slope_8 = (signed int)(rxmsg->data[6]<<8);
			Calib_Slope_8 = (signed int)(Calib_Slope_8|(rxmsg->data[7]));

			if(CAN_SlaveID==1)
				CalibSlope = Calib_Slope_1;
			else if(CAN_SlaveID==2)
				CalibSlope = Calib_Slope_2;
			else if(CAN_SlaveID==3)
				CalibSlope = Calib_Slope_3;
			else if(CAN_SlaveID==4)
				CalibSlope = Calib_Slope_4;
			else if(CAN_SlaveID==5)
				CalibSlope = Calib_Slope_5;
			else if(CAN_SlaveID==6)
				CalibSlope = Calib_Slope_6;
			else if(CAN_SlaveID==7)
				CalibSlope = Calib_Slope_7;
			else if(CAN_SlaveID==8)
				CalibSlope = Calib_Slope_8;
			else if(CAN_SlaveID==9)
				CalibSlope = Calib_Slope_9;
			else if(CAN_SlaveID==10)
				CalibSlope = Calib_Slope_10;
		}

		if(((rxmsg->id) == 0x70) &&((CAN_SlaveID==1)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x71) &&((CAN_SlaveID==2)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x72) &&((CAN_SlaveID==3)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x73) &&((CAN_SlaveID==4)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x74) &&((CAN_SlaveID==5)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x75) &&((CAN_SlaveID==6)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x76) &&((CAN_SlaveID==7)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x77) &&((CAN_SlaveID==8)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x78) &&((CAN_SlaveID==9)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}
		else if(((rxmsg->id) == 0x79) &&((CAN_SlaveID==10)))
		{
			VBRT_Cyl = (unsigned int)(rxmsg->data[0]<<8);
			VBRT_Cyl = (unsigned int)(VBRT_Cyl|(rxmsg->data[1]));
		}

		if(((rxmsg->id) == 0x40) &&((CAN_SlaveID==1)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x41) &&((CAN_SlaveID==2)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x42)&&((CAN_SlaveID==3)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x43)&&((CAN_SlaveID==4)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x44)&&((CAN_SlaveID==5)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x45)&&((CAN_SlaveID==6)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x46)&&((CAN_SlaveID==7)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x47)&&((CAN_SlaveID==8)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x48)&&((CAN_SlaveID==9)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}
		else if(((rxmsg->id) == 0x49) &&((CAN_SlaveID==10)))
		{
			Firing_Angle = (unsigned int)(rxmsg->data[0]<<8);
			Firing_Angle = (unsigned int)(Firing_Angle|(rxmsg->data[1]));
			Firing_Angle = ((-80)*Firing_Angle)+8000;
		}

		if((rxmsg->id) == 0X19 )
		{
			Flag_ManualOperation = 1;
			Flag_CalibOperation  = 0;
			Flag_RunOperation    = 0;

			temp_command         = 0;

			temp_command = (unsigned int)(rxmsg->data[0]<<8);
			temp_command = (unsigned int)(temp_command|(rxmsg->data[1]));

			if(((temp_command>>(CAN_SlaveID+2))&0x1)==1)
			{
				if((temp_command & 0x7)==0x00)
				{
					ManOpr_Vib_Flag = 1;
					ManOpr_FB_Flag  = 0;
					ManOpr_WB_Flag  = 0;
					ManOpr_SS_Flag  = 0;
					ManOpr_CS_Flag  = 0;
				}
				else if((temp_command & 0x7)==0x01)
				{
					ManOpr_Vib_Flag = 0;
					ManOpr_FB_Flag  = 1;
					ManOpr_WB_Flag  = 0;
					ManOpr_SS_Flag  = 0;
					ManOpr_CS_Flag  = 0;
				}
				else if((temp_command & 0x7)==0x02)
				{
					ManOpr_Vib_Flag = 0;
					ManOpr_FB_Flag  = 0;
					ManOpr_WB_Flag  = 1;
					ManOpr_SS_Flag  = 0;
					ManOpr_CS_Flag  = 0;
				}
				else if((temp_command & 0x7)==0x03)
				{
					ManOpr_Vib_Flag = 0;
					ManOpr_FB_Flag  = 0;
					ManOpr_WB_Flag  = 0;
					ManOpr_SS_Flag  = 1;
					ManOpr_CS_Flag  = 0;
				}
				else if((temp_command & 0x7)==0x04)
				{
					ManOpr_Vib_Flag = 0;
					ManOpr_FB_Flag  = 0;
					ManOpr_WB_Flag  = 0;
					ManOpr_SS_Flag  = 0;
					ManOpr_CS_Flag  = 1;
				}
				else
				{
					ManOpr_Vib_Flag = 0;
					ManOpr_FB_Flag  = 0;
					ManOpr_WB_Flag  = 0;
					ManOpr_SS_Flag  = 0;
					ManOpr_CS_Flag  = 0;
				}
			}
			else
			{
				ManOpr_Vib_Flag = 0;
				ManOpr_FB_Flag  = 0;
				ManOpr_WB_Flag  = 0;
				ManOpr_SS_Flag  = 0;
				ManOpr_CS_Flag  = 0;
			}

		}
		else if((rxmsg->id) == CAN_SlaveID)
		{
			Clear_Watchdog();

				if(rxmsg->data[0] == 1)
				{
					calibration_wt_flag = 1;
					Create_Send_CAN_Frame();
				}
				else if(Settle_TMR==0)
				{
					calibration_wt_flag = 0;
					Create_Send_CAN_Frame();
				}
		}

		else if((rxmsg->id)==0x0C)
		{
			Flag_RunOperation    = 0;
			Flag_ManualOperation = 0;

			a = (unsigned int)(rxmsg->data[0]<<8);
			a = (unsigned int)(a|(rxmsg->data[1]));

			if(((a>>(HEADID+1))&0x1)==1)
			{
				if((a&0x3)==0x0)
				{
					Tare_Flag           = FALSE;
					Dump_Flag           = TRUE;
					Calibeate_Flag      = FALSE;
					Flag_CalibOperation = 1;
				}
				else if((a&0x3)==0x1)
				{
					Tare_Flag           = TRUE;
					Dump_Flag           = FALSE;
					Calibeate_Flag      = FALSE;
					Flag_CalibOperation = 1;
				}
				else if((a&0x3)==0x2)
				{
					Tare_Flag           = FALSE;
					Dump_Flag           = FALSE;
					Calibeate_Flag      = TRUE;
					Flag_CalibOperation = 1;
				}
			}
			rxmsg->data[0] = 0;
			rxmsg->data[1] = 0;
	}
		rxmsg->data[0] = 0;
		rxmsg->data[1] = 0;
		rxmsg->data[2] = 0;
		rxmsg->data[3] = 0;
		rxmsg->data[4] = 0;
		rxmsg->data[5] = 0;
		rxmsg->data[6] = 0;
		rxmsg->data[7] = 0;

}


void CAN_SEND_MEG(struct CFrame *CANFrame1)
{
	/*Do not make any changes in the following sequence*/
	g_txmsg.ide 	= 0;
	g_txmsg.rtr 	= CANFrame1->rtr;					// Remote Transmission request byte
	g_txmsg.id		= CANFrame1->id;		// Slave or Frame ID
	g_txmsg.dlc 	= CANFrame1->dlc;					// Frame Size / Must not be more than 8 in Byte
	g_txmsg.data[0] = CANFrame1->data[0];	// Frame Structure in 8 Bytes
	g_txmsg.data[1] = CANFrame1->data[1];
	g_txmsg.data[2] = CANFrame1->data[2];
	g_txmsg.data[3] = CANFrame1->data[3];
	g_txmsg.data[4] = CANFrame1->data[4];
	g_txmsg.data[5] = CANFrame1->data[5];
	g_txmsg.data[6] = CANFrame1->data[6];
	g_txmsg.data[7] = CANFrame1->data[7];
	g_txmsg.one_shot = FALSE;       		// Do normal retries when sending
	g_txmsg.log_history = FALSE;    		// Do not log this message in history FIFO

	R_CAN_SendMsg(CAN_BOX_TXMBX_2, &g_txmsg);		// Send the CAN frame over the Network


}


void Send_Calibcount()
{
	if(CAN_SlaveID == 1)
		CANFrame.id=0x130;
	if(CAN_SlaveID == 2)
		CANFrame.id=0x133;
	if(CAN_SlaveID == 3)
		CANFrame.id=0x135;
	if(CAN_SlaveID == 4)
		CANFrame.id=0x137;
	if(CAN_SlaveID == 5)
		CANFrame.id=0x139;
	if(CAN_SlaveID == 6)
		CANFrame.id=0x141;
	if(CAN_SlaveID == 7)
		CANFrame.id=0x143;
	if(CAN_SlaveID == 8)
		CANFrame.id=0x145;
	if(CAN_SlaveID == 9)
		CANFrame.id=0x147;
	if(CAN_SlaveID == 10)
		CANFrame.id=0x149;

	CANFrame.rtr=0;
	CANFrame.dlc=4;
	CANFrame.data[0]=(CalibCnt>>8)&0xFF;
	CANFrame.data[1]=CalibCnt&0xFF;
	CANFrame.data[2]=(CalibSlope>>8)&0xFF;
	CANFrame.data[3]=CalibSlope&0xFF;

	CAN_SEND_MEG(&CANFrame);
}


void Send_Tarecount()
{
	if(CAN_SlaveID == 1)
		CANFrame.id=0x131;
	if(CAN_SlaveID == 2)
		CANFrame.id=0x132;
	if(CAN_SlaveID == 3)
		CANFrame.id=0x134;
	if(CAN_SlaveID == 4)
		CANFrame.id=0x136;
	if(CAN_SlaveID == 5)
		CANFrame.id=0x138;
	if(CAN_SlaveID == 6)
		CANFrame.id=0x140;
	if(CAN_SlaveID == 7)
		CANFrame.id=0x142;
	if(CAN_SlaveID == 8)
		CANFrame.id=0x144;
	if(CAN_SlaveID == 9)
		CANFrame.id=0x146;
	if(CAN_SlaveID == 10)
		CANFrame.id=0x148;

	CANFrame.rtr=0;
	CANFrame.dlc=2;
	CANFrame.data[0]=(TareCnt>>8)&0xFF;
	CANFrame.data[1]=TareCnt&0xFF;

	CAN_SEND_MEG(&CANFrame);
}

void Create_Send_CAN_Frame(void)
{

		unsigned char D[8];
		unsigned char idx;

		//for(idx=0;idx<8;idx++)
		{
			D[0] = 0;
			D[1] = 0;
			D[2] = 0;
			D[3] = 0;
			D[4] = 0;
			D[5] = 0;
			D[6] = 0;
			D[7] = 0;
		}


		if(Settle_TMR == 0 && wt_calculated_flag == 1)
		{
			D[0] = CurrentWt & 0xff;
			D[1] = (CurrentWt >> 8)& 0xff;
		}
		else
		{
			D[0] = 0;
			D[1] = 0;
		}


	    if(Flag_WtReady)
	    	D[2] |= 0x01;
	    else
	    	D[2] &= ~(0x01);

	   // if((HIN_WB == 0 && ENB_WB == 0)||(wb_at_homing_flag == 0))
	    if((HIN_WB == 0 && ENB_WB == 0))
	    {
	    	D[3] |= 0x01;
	    }
	    else
	    {
	    	D[3] &= ~(0x01);


	    	//if(wb_at_homing_flag == 1 && homing_flag_wb == 0 && Flag_WB_Trig == 0)
	    	if(homing_flag_wb == 0 && Flag_WB_Trig == 0)
	    	{
	    		homing_flag_wb = 1;
	    		Flag_WB_Trig   = 0;
	    	}
	    }

	   // if((HIN_FB == 0 && ENB_FB == 0)||(fb_at_homing_flag == 0))
	    if((HIN_FB == 0 && ENB_FB == 0))
	    {
	    	D[4] |= 0x01;
		}
		else
		{
			D[4] &= ~(0x01);


			//if(fb_at_homing_flag == 1 && homing_flag_fb == 0 && Flag_FB_Trig == 0)
			if(homing_flag_fb == 0 && Flag_FB_Trig == 0)
			{
				homing_flag_fb = 1;
				Flag_FB_Trig   = 0;
			}
		}

	    if(LongTime_Not_In_Combi_flag == 1)
	    {
	    	D[5] |= 0x01;
	    }
	    else
	    {
	    	D[5] &= ~(0x01);
	    }


		CANFrame.id  = HEADID + 0X0D;
		CANFrame.rtr=0;
		CANFrame.dlc = 6;

		for(idx=0;idx<6;idx++)
		{
			CANFrame.data[idx] = D[idx];
		}

		CAN_SEND_MEG(&CANFrame);
}





