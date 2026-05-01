/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2014-2015 by Center for Embedded Computing Systems
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
 *  $Id: DriveManager.c 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		制御系関連
 */

#include "Os.h"
#include "ModelCarControl.h"
#include "t_syslog.h"

/*
 *  ステアリングのニュートラル値
 */
sint8	steer_neutral = STEER_NEUTRAL_INIT;

/*
 *  車速のゲイン
 */
sint8	speed_gain = SPEED_GAIN_INIT;

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
 *  操舵角と車速の決定
 */
void
DrivingControl(void)
{
	/* ステアリングニュートラル調整 */
	switch (g_contcmd.steern) {
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
	switch (g_contcmd.gain) {
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

	/* 操舵角決定 */
	SetSteerAngle(g_contcmd.angle + steer_neutral);

	/* 車速決定 */
	SetDriveSpeed((g_contcmd.speed * speed_gain) / SPEED_GAIN_MID);

	/* ブレーキ状態設定 */
	SetBrake(g_contcmd.brake);
}

/*
 *  初回起動時のボディ系の確認の点滅終了までのカウント値
 */
#define START_CNT_MAX	(INIT_BODY_BLINK_CYCLE_MS*INIT_BODY_BLINK_COUNT*2)/DRIVEMANAGER_TASK_CYCLE_MS

void
BodyControl(void)
{
	static uint8	start_cnt = 0U;
	static boolean	toggle_headlamp = FALSE;
	static boolean	toggle_foglamp = FALSE;
	static boolean	toggle_hazard = FALSE;
	static boolean	toggle_winker_r = FALSE;
	static boolean	toggle_winker_l = FALSE;
	static boolean	pre_headlamp = FALSE;
	static boolean	pre_foglamp = FALSE;
	static boolean	pre_hazard = FALSE;
	static boolean	pre_winker_r = FALSE;
	static boolean	pre_winker_l = FALSE;
	static uint32	canmsg_snd_cycle = 0;

	/* 初回起動時処理 */
	if (start_cnt <= START_CNT_MAX) {
		/* 左右のWinker，全Light点滅(head，fog，brake，back) */
		if ((((start_cnt / (START_CNT_MAX/(INIT_BODY_BLINK_COUNT*2))) % 2U) == 0U)
			&& (start_cnt != START_CNT_MAX)) {
			g_bodycmd.headlamp  = TRUE;
			g_bodycmd.foglamp   = TRUE;
			g_bodycmd.brakelamp = TRUE;
			g_bodycmd.backlamp  = TRUE;
			g_bodycmd.winker    = WINKER_LR_ON;
			g_bodycmd.buzzer    = BUZZER_ON;
			canmsg_snd_cycle = 0;
		}
		else {
			g_bodycmd.headlamp  = FALSE;
			g_bodycmd.foglamp   = FALSE;
			g_bodycmd.brakelamp = FALSE;
			g_bodycmd.backlamp  = FALSE;
			g_bodycmd.winker    = WINKER_OFF;
			g_bodycmd.buzzer    = BUZZER_OFF;
			canmsg_snd_cycle = 0;
		}
		/* 起動カウントインクリメント */
		start_cnt++;
	}
	/* 通常起動時処理 */
	else {
		/* FALSE→TRUE遷移時に各トグル切り替え */
		if ((pre_headlamp == FALSE) && (g_contcmd.headlamp == TRUE)) {
			toggle_headlamp = (toggle_headlamp == FALSE) ? TRUE : FALSE;
		}
		if ((pre_foglamp == FALSE) && (g_contcmd.foglamp == TRUE)) {
			toggle_foglamp = (toggle_foglamp == FALSE) ? TRUE : FALSE;
		}
		if ((pre_hazard == FALSE) && (g_contcmd.hazard == TRUE)) {
			toggle_hazard = (toggle_hazard == FALSE) ? TRUE : FALSE;
		}
		if ((pre_winker_r == FALSE) && (g_contcmd.winker_r == TRUE)) {
			toggle_winker_r = (toggle_winker_r == FALSE) ? TRUE : FALSE;
		}
		if ((pre_winker_l == FALSE) && (g_contcmd.winker_l == TRUE)) {
			toggle_winker_l = (toggle_winker_l == FALSE) ? TRUE : FALSE;
		}

		/*
		 *  ランプ状態判定
		 */
		/* ヘッドランプ状態判定 */
		if (toggle_headlamp == TRUE) {
			g_bodycmd.headlamp = TRUE;
		}
		else {
			g_bodycmd.headlamp = FALSE;
		}
		
		/* フォグランプ状態判定 */
		if (toggle_foglamp == TRUE) {
			g_bodycmd.foglamp = TRUE;
		}
		else {
			g_bodycmd.foglamp = FALSE;
		}
		
		/* ブレーキランプ状態判定 */
		if (g_contcmd.brake == TRUE) {
			g_bodycmd.brakelamp = TRUE;
		}
		else {
			g_bodycmd.brakelamp = FALSE;
		}
		
		/* バックランプ状態判定 */
		if (g_contcmd.speed < 0) {
			g_bodycmd.backlamp = TRUE;
		}
		else {
			g_bodycmd.backlamp = FALSE;
		}

		/*
		 *  ウィンカー状態判定
		 */
		/* ゲイン，ニュートラル調整中は方向指示 */
		if ((acklr_cnt != 0U) || (ackr_cnt != 0U) || (ackl_cnt != 0U)) {
			if (acklr_cnt != 0U) {
				acklr_cnt--;
				g_bodycmd.winker = (acklr_cnt == 0)? WINKER_OFF : WINKER_LR_ON;
			}
			if (ackr_cnt != 0U) {
				ackr_cnt--;
				g_bodycmd.winker = (ackr_cnt == 0)? WINKER_OFF : WINKER_R_ON;
			}
			if (ackl_cnt != 0U) {
				ackl_cnt--;
				g_bodycmd.winker = (ackl_cnt == 0)? WINKER_OFF : WINKER_L_ON;
			}
		}
		else {
			/* 左右点滅 */
			if ((toggle_hazard == TRUE) ||
				((toggle_winker_r == TRUE) && (toggle_winker_l == TRUE))) {
				g_bodycmd.winker = WINKER_LR_BLINK;
			}
			/* 右のみ点滅 */
			else if (toggle_winker_r == TRUE) {
				g_bodycmd.winker = WINKER_R_BLINK;
			}
			/* 左のみ点滅 */
			else if (toggle_winker_l == TRUE) {
				g_bodycmd.winker = WINKER_L_BLINK;
			}
			/* 消灯 */
			else {
				g_bodycmd.winker = WINKER_OFF;
			}
		}

		/*
		 * ブザー状態判定
		 */
		if (g_contcmd.speed < 0) {
			g_bodycmd.buzzer = BUZZER_INTERVAL_ON;
		}
		else {
			g_bodycmd.buzzer = BUZZER_OFF;
		}

		/* 前回値保存 */
		pre_headlamp = g_contcmd.headlamp;
		pre_foglamp  = g_contcmd.foglamp;
		pre_hazard   = g_contcmd.hazard;
		pre_winker_r = g_contcmd.winker_r;
		pre_winker_l = g_contcmd.winker_l;
	}

	/*  ボディ系が別ECUの場合はCANで送信 */
	if (!IsBodyOnECU) {
		if (canmsg_snd_cycle-- == 0) {
			SendBodyCmdCanMag();
			canmsg_snd_cycle = (BODYCMD_CANMSG_SEND_CYCLE_MS/DRIVEMANAGER_TASK_CYCLE_MS);
		}
	}
}

/*
 *  制御タスク
 */
TASK(DriveManagerTask)
{
	static	boolean executed = FALSE;
	uint8 can_rx_data[6];
	uint8 can_rx_size;

	/*  初回起動時のみ実行 */
	if (executed == FALSE) {
		syslog(LOG_INFO, "DriveManagerTask : Start!");
		executed = TRUE;
	}

	/*  操作系が別ECUの場合はCANで受信 */
	if (!IsOperatorOnECU){
		tCanGetRxData(CONTCMD_CANMSG_RXMBOX_ID, CONTCMD_CANMSG_ID,
					  &can_rx_data[0], &can_rx_size);
		if (can_rx_size != 0) {
			UnpackContCmdCanMsg(&can_rx_data[0]);
		}
	}

	/* 駆動系の制御 */
	DrivingControl();

	/*  ボディ系の制御  */
	BodyControl();

	TerminateTask();
}
