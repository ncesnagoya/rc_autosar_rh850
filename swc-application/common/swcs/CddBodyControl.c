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
 *  $Id: CddBodyControl.c 122 2015-07-17 01:31:28Z ertl-honda $
 */  

/*
 *  CDD SW-C CddBodyControl用の記述
 */

#include "Rte_CddBodyControl.h"
#include "ModelCarControl.h"
#include "t_syslog.h"

/*
 *  CDD SW-C CddBodyControl用の初期化
 */
void
CddBodyControlInit(void)
{
	tInitBody();
}

/*
 *  LampControlランナブル
 */
void
LampControl(void)
{
	Std_ReturnType	ercd;
	IDT_Lamp		headlamp;
	IDT_Lamp		foglamp;
	IDT_Lamp		brakelamp;
	IDT_Lamp		backlamp;

	/* LampIn状態取得 */
	ercd = Rte_Read_LampIn_headlamp(&headlamp);
	ercd += Rte_Read_LampIn_foglamp(&foglamp);
	ercd += Rte_Read_LampIn_brakelamp(&brakelamp);
	ercd += Rte_Read_LampIn_backlamp(&backlamp);

	/* 1つでも取得に失敗したら後続の処理は実行しない */
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_LampIn()");
	}
	else {
		HEAD_LAMP_SET(headlamp);
		FOG_LAMP_SET(foglamp);
		BRAKE_LAMP_SET(brakelamp);
		BACK_LAMP_SET(backlamp);
	}
}

/*
 *  ウィンカーの点滅周期用カウンタの初期値
 */
#define CNT_BLINK_INIT  (WINKER_INTERVAL_MS/BODY_CYCLE_MS)

/*
 *  WinkerControlランナブル
 */
void
WinkerControl(void)
{
	Std_ReturnType		ercd;
	IDT_Winker			winker;
	static IDT_Winker	pre_winker = 0U;
	static boolean		pre_winker_lr = FALSE;
	static boolean		pre_winker_l = FALSE;
	static boolean		pre_winker_r = FALSE;
	static uint8		cnt_blink = 0U;

	/* Winkerデータ取得 */
	ercd = Rte_Read_WinkerIn_state(&winker);
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_WinkerIn_state()");
		return;
	}

	/* 指示に変化があったタイミングから点滅周期を開始する */
	if (winker != pre_winker) {
		cnt_blink = 0U;
	}

	if (cnt_blink == 0U) {
		switch (winker) {
		case WINKER_OFF: /* 消灯 */
			WINKER_L_SET(FALSE);
			WINKER_R_SET(FALSE);
			break;
		case WINKER_L_ON: /* 左のみ点灯 */
			WINKER_L_SET(TRUE);
			break;
		case WINKER_R_ON: /* 右のみ点灯 */
			WINKER_R_SET(TRUE);
			break;
		case WINKER_LR_ON: /* 左右点灯 */
			WINKER_L_SET(TRUE);
			WINKER_R_SET(TRUE);
			break;
		case WINKER_L_BLINK: /* 左のみ点滅 */
			if (pre_winker_l == FALSE) {
				WINKER_L_SET(TRUE);
				pre_winker_l = TRUE;
			}
			else {
				WINKER_L_SET(FALSE);
				pre_winker_l = FALSE;
			}
			break;
		case WINKER_R_BLINK: /* 右のみ点滅 */
			if (pre_winker_r == FALSE) {
				WINKER_R_SET(TRUE);
				pre_winker_r = TRUE;
			}
			else {
				WINKER_R_SET(FALSE);
				pre_winker_r = FALSE;
			}
			break;
		case WINKER_LR_BLINK: /* 左右点滅 */
			if (pre_winker_lr == FALSE) {
				WINKER_L_SET(TRUE);
				WINKER_R_SET(TRUE);
				pre_winker_lr = TRUE;
			}
			else {
				WINKER_L_SET(FALSE);
				WINKER_R_SET(FALSE);
				pre_winker_lr = FALSE;
			}
			break;
		default:
			break;
		}
		cnt_blink = CNT_BLINK_INIT;
	}

	cnt_blink--;

	pre_winker = winker;
}

/*
 *  BuzzerControlランナブル
 */
void
BuzzerControl(void)
{
	Std_ReturnType	ercd;
	IDT_Buzzer		buzzer;
	static boolean	buzzer_toggle = FALSE;
	static uint16	buzzer_cycle = 0U;
	boolean 		buzzer_on = FALSE;

	/* Buzzerデータ取得 */
	ercd = Rte_Read_BuzzerIn_state(&buzzer);

	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_BuzzerIn_state()");
		return;
	}

	switch (buzzer) {
	  case BUZZER_OFF:
		/* 消音 */
		break;
	  case BUZZER_ON:
		/* 常時発音 */
		buzzer_on = TRUE;
		break;
	  case BUZZER_INTERVAL_ON:
		/* 一定間隔で発音 */
		buzzer_cycle++;
		if (buzzer_cycle < ((BUZZER_INTERVAL_MS * 1000)/BUZZER_CYCLE_NS)) {
			buzzer_on = TRUE;
		}
		if (buzzer_cycle == ((BUZZER_INTERVAL_MS * 1000)/BUZZER_CYCLE_NS)*2) {
			buzzer_cycle = 0U;
		}
		break;
	  default:
		break;
	}

	if (buzzer_on == TRUE) {
		tSetBuzzer(buzzer_toggle);
		buzzer_toggle = (buzzer_toggle == TRUE) ? FALSE : TRUE;
	} else {
		tSetBuzzer(FALSE);
	}
}
