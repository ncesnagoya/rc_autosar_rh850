/*
 *  Copyright (C) 2014-2015 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2014-2015 by FUJI SOFT INCORPORATED, JAPAN
 *
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
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
 *  $Id: OperationManager.c 122 2015-07-17 01:31:28Z ertl-honda $
 */  

/*
 *		CDD SW-C OperationManager用の記述
 */

#include "Rte_OperationManager.h"
#include "ModelCarControl.h"
#include "rcb3.h"
#include "t_syslog.h"


/* ボディ系情報を送信する回数(20ms * 6 = 120ms) */
#define BODY_SEND_COUNT			6U

/*
 *  OperationControlランナブル
 */
void
OperationControl(void)
{
	Std_ReturnType		ercd;
	IDT_RCB3			rcb3_bstate;

	IDT_Angle	angle;
	IDT_Speed	speed;
	IDT_Adjust	steern;
	IDT_Adjust	gain;
	IDT_Brake	brake = FALSE;
	IDT_Lamp	headlamp = FALSE;
	IDT_Lamp	foglamp = FALSE;
	IDT_Lamp	hazard = FALSE;
	IDT_Lamp	winker_r = FALSE;
	IDT_Lamp	winker_l = FALSE;


#ifdef TOPPERS_WITH_DISP
	IDT_Disp	disp;
	IDT_Cycle	cycle;
	IDT_Switch	differ = FALSE;
	IDT_Switch	swich_angle = FALSE;
	IDT_Switch	swich_speed = FALSE;
	IDT_Switch	pwm_angle = FALSE;
	IDT_Switch	pwm_speed = FALSE;
#endif /* TOPPERS_WITH_DISP */

	/* RCB3コントローラから送られて来たRCB3コマンドの取得 */
	ercd = Rte_Call_RCB3CommandClt_OpGetIfRCB3Command(&rcb3_bstate);
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Call_RCB3CommandClt_OpGetIfRCB3Command()");
	}
	else if (rcb3_bstate != NOT_RECEIVE_COMMAND) {
		/* 各ON/OFF切り替え情報 */
		if ((rcb3_bstate & RCB3_MSG_BUTTON_NOUGHT) != 0U) {
			headlamp = TRUE;
		}
		if ((rcb3_bstate & RCB3_MSG_BUTTON_SQUARE) != 0U) {
			foglamp = TRUE;
		}
		if ((rcb3_bstate & RCB3_MSG_BUTTON_TRIANGLE) != 0U) {
			hazard = TRUE;
		}
		if ((rcb3_bstate & RCB3_MSG_BUTTON_R1) != 0U) {
			winker_r = TRUE;
		}
		if ((rcb3_bstate & RCB3_MSG_BUTTON_L1) != 0U) {
			winker_l = TRUE;
		}

		/* ステアリング(-63～64に調整) */
		angle = (-1) * ((IDT_Angle) ((rcb3_bstate >> 16U) & 0xFFU) - 64);

		/* 前進，後退(-63～64に調整) */
		speed = (-1) * ((IDT_Speed) ((rcb3_bstate >> 24U) & 0xFFU) - 64);

		/* ステアリング調整 */
		if ((rcb3_bstate & RCB3_MSG_BUTTON_UP) != 0U) {
			steern = CONTCMD_STEERN_I; /* ステアリングニュートラルを初期値に */
		}
		else if ((rcb3_bstate & RCB3_MSG_BUTTON_RIGHT) != 0U) {
			steern = CONTCMD_STEERN_R; /* ステアリングのニュートラルを右に調整 */
		}
		else if ((rcb3_bstate & RCB3_MSG_BUTTON_LEFT) != 0U) {
			steern = CONTCMD_STEERN_L; /* ステアリングのニュートラルを左に調整 */
		}
		else {
			steern = 0U; /* 要求なし */
		}

		/* ゲイン調整 */
		if ((rcb3_bstate & RCB3_MSG_BUTTON_DOWN) != 0U) {
			gain = CONTCMD_GAIN_I;  /* モータゲインを初期値にする */
		}
		else if ((rcb3_bstate & RCB3_MSG_BUTTON_R2) != 0U) {
			gain = CONTCMD_GAIN_U;  /* モータゲインを1段階上げる */
		}
		else if ((rcb3_bstate & RCB3_MSG_BUTTON_L2) != 0U) {
			gain = CONTCMD_GAIN_D;  /* モータゲインを1段階下げる */
		}
		else {
			gain = 0U;  /* 要求なし */
		}

		/* ブレーキ */
		if ((rcb3_bstate & RCB3_MSG_BUTTON_CROSS) != 0U) {
			brake = TRUE;
		}

		/* IfDrivingへ送信 */
		Rte_Write_DrivingOut_angle(angle);
		Rte_Write_DrivingOut_speed(speed);
		Rte_Write_DrivingOut_steern(steern);
		Rte_Write_DrivingOut_gain(gain);
		Rte_Write_DrivingOut_brake(brake);

		/* IfBodyへの送信 */
		Rte_Write_BodyOut_headlamp(headlamp);
		Rte_Write_BodyOut_foglamp(foglamp);
		Rte_Write_BodyOut_hazard(hazard);
		Rte_Write_BodyOut_winker_r(winker_r);
		Rte_Write_BodyOut_winker_l(winker_l);
	}


#ifdef TOPPERS_WITH_DISP
	/* 状態表示要求 */
	ercd = Rte_Call_DisplayCommandClt_OpGetDisplayCommand(&disp);
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Call_DisplayCommandClt_OpGetDisplayCommand()");
	}
	else if (disp != NOT_RECEIVE_COMMAND) {
		/* 表示周期 */
		cycle = disp & 0xFFFFU;

		/* 差分がある場合のみ表示するかどうか */
		if ((disp & 0x00010000U) != 0U) {
			differ = TRUE;
		}

		/* 各ON/OFF切り替え情報 */
		if ((disp & 0x00020000U) != 0U) {
			swich_angle = TRUE;
		}
		if ((disp & 0x00040000U) != 0U) {
			swich_speed = TRUE;
		}
		if ((disp & 0x00080000U) != 0U) {
			pwm_angle = TRUE;
		}
		if ((disp & 0x00100000U) != 0U) {
			pwm_speed = TRUE;
		}

		/* IfDisplayModeへ送信 */
		Rte_Write_DisplayModeOut_cycle(cycle);
		Rte_Write_DisplayModeOut_differ(differ);
		Rte_Write_DisplayModeOut_angle(swich_angle);
		Rte_Write_DisplayModeOut_speed(swich_speed);
		Rte_Write_DisplayModeOut_pwm_angle(pwm_angle);
		Rte_Write_DisplayModeOut_pwm_speed(pwm_speed);
	}
#endif /* TOPPERS_WITH_DISP */
}
