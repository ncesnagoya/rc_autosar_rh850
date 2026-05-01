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
 *  $Id: DriveManager.c 122 2015-07-17 01:31:28Z ertl-honda $
 */  

/*
 *		CDD SW-C DriveManager用の記述
 */

#include "Rte_DriveManager.h"
#include "ModelCarControl.h"
#include "t_syslog.h"

/*
 *  ステアリングのニュートラル値
 */
static IDT_Angle	steer_neutral = STEER_NEUTRAL_INIT;

/*
 *  車速のゲイン
 */
static uint8		speed_gain = SPEED_GAIN_INIT;

/*
 *  ステアリングニュートラルや車速ゲイン調整時のACK通知時間
 */
#define ACK_CNT_MAX		8U

/*
 *  ステアリングニュートラルや車速ゲイン調整時のACK通知用変数
 */
static uint8	acklr_cnt = 0U;
static uint8	ackr_cnt = 0U;
static uint8	ackl_cnt = 0U;

/*
 * 現在値
 * ランナブル間で共有するために共有変数として宣言 
 */
static IDT_Angle	cur_angle;
static IDT_Speed	cur_speed;
static IDT_Adjust	cur_steern;
static IDT_Adjust	cur_gain;
static IDT_Brake	cur_brake;

/*
 *  DrivingControlランナブル
 */
void
DrivingControl(void)
{
	Std_ReturnType	ercd;

	/* IfDriving情報取得 */
	ercd = Rte_Read_DrivingIn_angle(&cur_angle);
	ercd += Rte_Read_DrivingIn_speed(&cur_speed);
	ercd += Rte_Read_DrivingIn_steern(&cur_steern);
	ercd += Rte_Read_DrivingIn_gain(&cur_gain);
	ercd += Rte_Read_DrivingIn_brake(&cur_brake);

	/* 1つでも取得に失敗したら後続の処理は実行しない */
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_DrivingIn()");
		return;
	}

	/* ステアリングニュートラル調整 */
	switch (cur_steern) {
	  case CONTCMD_STEERN_I:
		steer_neutral = STEER_NEUTRAL_INIT;
		acklr_cnt = ACK_CNT_MAX;
		break;
	  case CONTCMD_STEERN_R:
		/* 右(-)へ調整 */
		if (steer_neutral > STEER_NEUTRAL_MIN) {
			steer_neutral--;
		}
		ackr_cnt = ACK_CNT_MAX;
		break;
	  case CONTCMD_STEERN_L:
		/* 左(+)へ調整 */
		if (steer_neutral < STEER_NEUTRAL_MAX) {
			steer_neutral++;
		}
		ackl_cnt = ACK_CNT_MAX;
		break;
	  default:
		/* 要求なし */
		break;
	}

	/* ゲイン調整 */
	switch (cur_gain) {
	  case CONTCMD_GAIN_I:
		speed_gain = SPEED_GAIN_INIT;
		acklr_cnt = ACK_CNT_MAX;
		break;
	  case CONTCMD_GAIN_U:
		/* ゲインを上げる */
		if (speed_gain < SPEED_GAIN_MAX) {
			speed_gain++;
		}
		ackr_cnt = ACK_CNT_MAX;
		break;
	  case CONTCMD_GAIN_D:
		/* ゲインを下げる */
		if (speed_gain > SPEED_GAIN_MIN) {
			speed_gain--;
		}
		ackl_cnt = ACK_CNT_MAX;
		break;
	  default:
		/* 要求なし */
		break;
	}

	/* 操舵角送信(-63〜64) */
	Rte_Call_AngleClt_OpSetSteerAngle(cur_angle + steer_neutral);

	/* 車速送信(-63〜64) */
	Rte_Call_SpeedClt_OpSetDriveSpeed((cur_speed * speed_gain) / SPEED_GAIN_MID);

	/* ブレーキ送信 */
	Rte_Call_BrakeClt_OpSetBrake(cur_brake);
}

/*
 *  初回起動時のボディ系の確認の点滅終了までのカウント値
 */
#define START_CNT_MAX	((INIT_BODY_BLINK_CYCLE_MS*INIT_BODY_BLINK_COUNT*2)/BODYCONTROL_RUNNABLE_CYCLE_MS)

/*
 *  BodyControlランナブル
 */
void
BodyControl(void)
{
	Std_ReturnType	ercd;
	IDT_Lamp		headlamp;
	IDT_Lamp		foglamp;
	IDT_Lamp		hazard;
	IDT_Lamp		winker_r;
	IDT_Lamp		winker_l;
	static uint8	start_cnt = 0U;
	static IDT_Lamp	toggle_headlamp = FALSE;
	static IDT_Lamp	toggle_foglamp = FALSE;
	static IDT_Lamp	toggle_winker_r = FALSE;
	static IDT_Lamp	toggle_winker_l = FALSE;
	static IDT_Lamp	toggle_hazard = FALSE;
	static IDT_Lamp	pre_headlamp = FALSE;
	static IDT_Lamp	pre_foglamp = FALSE;
	static IDT_Lamp	pre_hazard = FALSE;
	static IDT_Lamp	pre_winker_r = FALSE;
	static IDT_Lamp	pre_winker_l = FALSE;

	/* 初回起動時処理 */
	if (start_cnt <= START_CNT_MAX) {
		/* 左右のWinker，全Lamp点滅(head，fog，brake，back) */
		if ((((start_cnt / (START_CNT_MAX/(INIT_BODY_BLINK_COUNT*2))) % 2U) == 0U)) {
			Rte_Write_LampOut_headlamp(TRUE);
			Rte_Write_LampOut_foglamp(TRUE);
			Rte_Write_LampOut_backlamp(TRUE);
			Rte_Write_LampOut_brakelamp(TRUE);
			Rte_Write_WinkerOut_state(WINKER_LR_ON);
			Rte_Write_BuzzerOut_state(BUZZER_ON);
		}
		else {
			Rte_Write_LampOut_headlamp(FALSE);
			Rte_Write_LampOut_foglamp(FALSE);
			Rte_Write_LampOut_backlamp(FALSE);
			Rte_Write_LampOut_brakelamp(FALSE);
			Rte_Write_WinkerOut_state(WINKER_OFF);
			Rte_Write_BuzzerOut_state(BUZZER_OFF);
		}
		/* 起動カウントインクリメント */
		start_cnt++;
	}
	/* 通常起動時処理 */
	else {
		/* IfBody情報取得 */
		ercd = Rte_Read_BodyIn_headlamp(&headlamp);
		ercd += Rte_Read_BodyIn_foglamp(&foglamp);
		ercd += Rte_Read_BodyIn_hazard(&hazard);
		ercd += Rte_Read_BodyIn_winker_r(&winker_r);
		ercd += Rte_Read_BodyIn_winker_l(&winker_l);

		/* 1つでも取得に失敗したら後続の処理は実行しない */
		if (ercd != RTE_E_OK) {
			syslog(LOG_INFO, "## Error: Rte_Read_BodyIn()");
			return;
		}

		/* FALSE→TRUE遷移時に各トグル切り替え */
		if ((pre_headlamp == FALSE) && (headlamp == TRUE)) {
			toggle_headlamp = (toggle_headlamp == FALSE) ? TRUE : FALSE;
		}
		if ((pre_foglamp == FALSE) && (foglamp == TRUE)) {
			toggle_foglamp = (toggle_foglamp == FALSE) ? TRUE : FALSE;
		}
		if ((pre_hazard == FALSE) && (hazard == TRUE)) {
			toggle_hazard = (toggle_hazard == FALSE) ? TRUE : FALSE;
		}
		if ((pre_winker_r == FALSE) && (winker_r == TRUE)) {
			toggle_winker_r = (toggle_winker_r == FALSE) ? TRUE : FALSE;
		}
		if ((pre_winker_l == FALSE) && (winker_l == TRUE)) {
			toggle_winker_l = (toggle_winker_l == FALSE) ? TRUE : FALSE;
		}

		/*
		 *  ランプ状態判定
		 */
		/* ヘッドランプ状態判定 */
		if (toggle_headlamp == TRUE) {
			Rte_Write_LampOut_headlamp(TRUE);
		}
		else {
			Rte_Write_LampOut_headlamp(FALSE);
		}

		/* フォグランプ状態判定 */
		if (toggle_foglamp == TRUE) {
			ercd += Rte_Write_LampOut_foglamp(TRUE);
		}
		else {
			ercd += Rte_Write_LampOut_foglamp(FALSE);
		}

		/* ブレーキランプ状態判定 */
		if (cur_brake == TRUE) {
			Rte_Write_LampOut_brakelamp(TRUE);
		}
		else {
			Rte_Write_LampOut_brakelamp(FALSE);
		}

		/* バックランプ状態判定 */
		if (cur_speed < 0) {
			Rte_Write_LampOut_backlamp(TRUE);
		}
		else {
			Rte_Write_LampOut_backlamp(FALSE);
		}

		/*
		 *  ウィンカー情報解析
		 */
		/* ゲイン，ニュートラル調整中は方向指示 */
		if ((acklr_cnt != 0U) || (ackr_cnt != 0U) || (ackl_cnt != 0U)) {
			if (acklr_cnt != 0U) {
				acklr_cnt--;
				Rte_Write_WinkerOut_state((acklr_cnt == 0)? WINKER_OFF : WINKER_LR_ON);
			}
			if (ackr_cnt != 0U) {
				ackr_cnt--;
				Rte_Write_WinkerOut_state((ackr_cnt == 0)? WINKER_OFF : WINKER_R_ON);
			}
			if (ackl_cnt != 0U) {
				ackl_cnt--;
				Rte_Write_WinkerOut_state((ackl_cnt == 0)? WINKER_OFF : WINKER_L_ON);
			}
		}
		else {
			/* 左右点滅 */
			if ((toggle_hazard == TRUE) ||
				((toggle_winker_r == TRUE) && (toggle_winker_l == TRUE))) {
				Rte_Write_WinkerOut_state(WINKER_LR_BLINK);
			}
			/* 右のみ点滅 */
			else if (toggle_winker_r == TRUE) {
				Rte_Write_WinkerOut_state(WINKER_R_BLINK);
			}
			/* 左のみ点滅 */
			else if (toggle_winker_l == TRUE) {
				Rte_Write_WinkerOut_state(WINKER_L_BLINK);
			}
			/* 消灯 */
			else {
				Rte_Write_WinkerOut_state(WINKER_OFF);
			}
		}

		/*
		 *  Buzzer情報解析
		 */
		/* 車速がマイナスか判定 */
		if (cur_speed < 0) {
			Rte_Write_BuzzerOut_state(BUZZER_INTERVAL_ON);
		}
		else {
			Rte_Write_BuzzerOut_state(BUZZER_OFF);
		}

		/* 前回値保存 */
		pre_headlamp = headlamp;
		pre_foglamp = foglamp;
		pre_hazard = hazard;
		pre_winker_r = winker_r;
		pre_winker_l = winker_l;
	}
}
