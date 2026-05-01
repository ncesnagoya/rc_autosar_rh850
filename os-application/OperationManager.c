/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2014-2015 by Center for Embedded Computing Systems
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
 *  $Id: OperationManager.c 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		操作系関連
 */

#include "Os.h"
#include "ModelCarControl.h"
#include "t_syslog.h"
#include "rcb3.h"

/*
 *  操作系用の初期化
 */
void
OperationInit(void)
{
	tInitSBDBTUart();
	rcb3_Init();
}

/*
 *  RCB3コマンドから制御系への指示に変換して送る
 */
void
OperationControl(void)
{
	uint16 rcb3_bstate;
	sint8		angle;
	sint8		speed;
	sint8		steern;
	sint8		gain;
	boolean		brake    = FALSE;
	boolean		headlamp = FALSE;
	boolean		foglamp  = FALSE;
	boolean		winker_l = FALSE;
	boolean		winker_r = FALSE;
	boolean		hazard   = FALSE;

	/* ボタン状態を生成 */
	rcb3_bstate = (g_rcb3_rmsg[RCB3_MSG_OFFSET_BUTTON_H] << 8U)
	                          | (g_rcb3_rmsg[RCB3_MSG_OFFSET_BUTTON_L]);

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

	/* 操舵角(-63〜64に調整) */
	angle = (-1) * (g_rcb3_rmsg[RCB3_MSG_OFFSET_L_ANALOG_LR] - 64);

	/* 車速(-63〜64に調整) */
	speed = (-1) * (g_rcb3_rmsg[RCB3_MSG_OFFSET_R_ANALOG_UD] - 64);

	/* ステアリングニュートラル調整 */
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

	/* 車速ゲイン調整 */
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

	/* g_contcmd の更新 */
	g_contcmd.angle    = angle;
	g_contcmd.speed    = speed;
	g_contcmd.gain     = gain;
	g_contcmd.steern   = steern;
	g_contcmd.brake    = brake;
	g_contcmd.headlamp = headlamp;
	g_contcmd.foglamp  = foglamp;
	g_contcmd.hazard   = hazard;	
	g_contcmd.winker_l = winker_l;
	g_contcmd.winker_r = winker_r;

	/* 制御系へ更新があったことを通知 */
	UpDateContCmd = TRUE;

	/* 制御系が別ECUで実行されているの場合はCANで送信 */
	if (!IsControlerOnECU) {
		SendContCmdCanMag();
	}
}

/*
 *  操作マネージャタスク
 */
TASK(OperationManagerTask)
{
	unsigned char	c;
	static	boolean executed = FALSE;

	/* 初回起動時のみ実行 */
	if (executed == FALSE) {
		syslog(LOG_INFO, "OperationManagerTask : Start!");
		executed = TRUE;
	}

	while(tReceiveSBDBTUart(&c, 1)) {
		if (rcb3_AddReceivedByte(c)) {
			/* RCB3コマンドから制御系コマンドへ変換して送信 */
			OperationControl();
		}
	}

	TerminateTask();
}

/*
 *  rcb3_AddReceivedByteからのエラーコールバック
 */
void
rcb3_ErrorCb(uint8 sum, uint8 recv_sum, uint8 *p_rcb3_rmsg)
{
	uint32 i;

	syslog(LOG_INFO, "## RCB3 Sum Error: 0x%x != 0x%x", sum, recv_sum);
	for (i = 0; i < RCB3_MSG_LEN; i++) {
		syslog(LOG_INFO, "## rcb3_rmsg[%d]: 0x%x", i, p_rcb3_rmsg[i]);
	}
}

