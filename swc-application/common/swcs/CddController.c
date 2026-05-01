/*
 *  Copyright (C) 2014-2015 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2014-2015 by FUJI SOFT INCORPORATED, JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *
 *  本ソフトウェアは，AUTOSAR（AUTomotive Open System ARchitecture）仕
 *  様に基づいている．上記の許諾は，AUTOSARの知的財産権を許諾するもので
 *  はない．AUTOSARは，AUTOSAR仕様に基づいたソフトウェアを商用目的で利
 *  用する者に対して，AUTOSARパートナーになることを求めている．
 *  
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  $Id: CddController.c 122 2015-07-17 01:31:28Z ertl-honda $
 */  

/*
 *		CDD SW-C CddController用の記述
 */

#include "Rte_CddController.h"
#include "ModelCarControl.h"
#include "t_syslog.h"
#include "rcb3.h"

/*
 *  CDD SW-C CddController用の初期化
 */
void
CddControllerInit(void)
{
	tInitSBDBTUart();
	rcb3_Init();
}

#ifdef TOPPERS_WITH_DISP
/*
 *  状態表示関連
 */
#define DISP_MSG_LEN			8
#define DISP_MSG_HEADER_1		0xF0

typedef enum disp_st {
	DISP_ST_HEADER_1,
	DISP_ST_CYCLE_1,
	DISP_ST_CYCLE_2,
	DISP_ST_INFO_1,
	DISP_ST_INFO_2,
	DISP_ST_INFO_3,
	DISP_ST_INFO_4,
	DISP_ST_SUM
} disp_st_type;

static disp_st_type	s_disp_st = DISP_ST_HEADER_1;
static uint8		s_disp_rmsg_cnt;
static uint8		g_disp_rmsg[DISP_MSG_LEN];

static boolean disp_AddReceivedByte(uint8 c);
#endif /* TOPPERS_WITH_DISP */


/* 無効値(受信していない) */
#define NOT_RECEIVE_COMMAND		0xFFFFFFFFU

/* 受信モード */
typedef enum {
	RX_MODE_INIT,
	RX_MODE_RCB3,
#ifdef TOPPERS_WITH_DISP
	RX_MODE_DISP
#endif /* TOPPERS_WITH_DISP */
} rx_mode_type;

/* TRUEで初期化することで不定値を取得しないようにする */
static IDT_RCB3		RCB3CommandData;
static boolean		RCB3CommandGotFlg = TRUE;
#ifdef TOPPERS_WITH_DISP
static IDT_Disp		DisplayCommandData;
static boolean		DisplayCommandGotFlg = TRUE;
#endif /* TOPPERS_WITH_DISP */

static rx_mode_type rx_mode = RX_MODE_INIT;

/*
 *  RunnableEntity(オペレーション呼出し)
 */
void
GetRCB3Command(IDT_RCB3 *command)
{
	if (RCB3CommandGotFlg == FALSE) {
		*command = RCB3CommandData;
		RCB3CommandGotFlg = TRUE;
	}
	else {
		*command = NOT_RECEIVE_COMMAND;
	}
}

/*
 *  MessageParserランナブル
 */
void
MessageParser(void)
{
	uint8 c;

	while (tReceiveSBDBTUart(&c, 1)) {
		switch (rx_mode) {
		case RX_MODE_INIT:
			if (c == RCB3_MSG_HEADER_1) {
				/* RCB3データ受信開始 */
				(void) rcb3_AddReceivedByte(c);
				rx_mode = RX_MODE_RCB3;
			}
#ifdef TOPPERS_WITH_DISP
			else if (c == DISP_MSG_HEADER_1) {
				/* 状態表示データ受信開始 */
				(void) disp_AddReceivedByte(c);
				rx_mode = RX_MODE_DISP;
			}
#endif /* TOPPERS_WITH_DISP */
			else {
				syslog(LOG_INFO, "## Header Error (0x%x)", c);
			}
			break;
		case RX_MODE_RCB3:
			if (rcb3_AddReceivedByte(c)) {
				/* RCB3データ受信完了 */
				RCB3CommandData = (IDT_RCB3) g_rcb3_rmsg[RCB3_MSG_OFFSET_BUTTON_L];         /* ボタン情報1 */
				RCB3CommandData += ((IDT_RCB3) g_rcb3_rmsg[RCB3_MSG_OFFSET_BUTTON_H]) << 8U;/* ボタン情報2 */
				RCB3CommandData += ((IDT_RCB3) g_rcb3_rmsg[RCB3_MSG_OFFSET_L_ANALOG_LR]) << 16U;  /* アナログ1 */
				RCB3CommandData += ((IDT_RCB3) g_rcb3_rmsg[RCB3_MSG_OFFSET_R_ANALOG_UD]) << 24U;  /* アナログ4 */
				RCB3CommandGotFlg = FALSE;
				rx_mode = RX_MODE_INIT;
			}
			break;
#ifdef TOPPERS_WITH_DISP
		case RX_MODE_DISP:
			if (disp_AddReceivedByte(c)) {
				/* 状態表示データ受信完了 */
				DisplayCommandData = (IDT_Disp) g_disp_rmsg[1];             /* 周期表示1 */
				DisplayCommandData += ((IDT_Disp) g_disp_rmsg[2]) << 8U;    /* 周期表示2 */
				DisplayCommandData += ((IDT_Disp) g_disp_rmsg[3]) << 16U;   /* 状態表示 */
				DisplayCommandGotFlg = FALSE;
				rx_mode = RX_MODE_INIT;
			}
			break;
#endif /* TOPPERS_WITH_DISP */
		}
	}
}

/*
 *  rcb3_AddReceivedByteからのエラーコールバック
 */
void
rcb3_ErrorCb(uint8 sum, uint8 recv_sum, uint8 *p_rcb3_rmsg)
{
	uint32 i;

	rx_mode = RX_MODE_INIT;
	syslog(LOG_INFO, "## RCB3 Sum Error: 0x%x != 0x%x", sum, recv_sum);
	for (i = 0; i < RCB3_MSG_LEN; i++) {
		syslog(LOG_INFO, "## rcb3_rmsg[%d]: 0x%x", i, p_rcb3_rmsg[i]);
	}
}

#ifdef TOPPERS_WITH_DISP
void
GetDisplayCommand(IDT_Disp *command)
{
	if (DisplayCommandGotFlg == FALSE) {
		*command = DisplayCommandData;
		DisplayCommandGotFlg = TRUE;
	}
	else {
		*command = NOT_RECEIVE_COMMAND;
	}
}

boolean
disp_AddReceivedByte(uint8 c)
{
	boolean rval = FALSE;
	uint8	i;
	uint8	sum = 0;

	switch (s_disp_st) {
	case DISP_ST_HEADER_1:
		if (c == DISP_MSG_HEADER_1) {
			s_disp_rmsg_cnt = 0;
			g_disp_rmsg[s_disp_rmsg_cnt++] = c;
			s_disp_st = DISP_ST_CYCLE_1;
		}
		break;
	case DISP_ST_CYCLE_1:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		s_disp_st = DISP_ST_CYCLE_2;
		break;
	case DISP_ST_CYCLE_2:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		s_disp_st = DISP_ST_INFO_1;
		break;
	case DISP_ST_INFO_1:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		s_disp_st = DISP_ST_INFO_2;
		break;
	case DISP_ST_INFO_2:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		s_disp_st = DISP_ST_INFO_3;
		break;
	case DISP_ST_INFO_3:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		s_disp_st = DISP_ST_INFO_4;
		break;
	case DISP_ST_INFO_4:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		s_disp_st = DISP_ST_SUM;
		break;
	case DISP_ST_SUM:
		g_disp_rmsg[s_disp_rmsg_cnt++] = c;
		for (i = 1; i < (DISP_MSG_LEN - 1); i++) {
			sum += g_disp_rmsg[i];
		}
		sum &= 0x7FU;
		if (c == sum) {
			rval = TRUE;
		}
		else {
			rx_mode = RX_MODE_INIT;
			syslog(LOG_INFO, "## Disp Sum Error: 0x%x != 0x%x", sum, c);
			for (i = 0; i < DISP_MSG_LEN; i++) {
				syslog(LOG_INFO, "## g_disp_rmsg[%d]: 0x%x", i, g_disp_rmsg[i]);
			}
		}
		s_disp_st = DISP_ST_HEADER_1;
		break;
	}
	return(rval);
}
#endif /* TOPPERS_WITH_DISP */
