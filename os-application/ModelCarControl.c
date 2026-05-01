/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 *
 *  Copyright (C) 2015 by Center for Embedded Computing Systems
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
 *  $Id: ModelCarControl.c 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		RC-CAR制御プログラム
 */

#include "Os.h"
#include "t_syslog.h"
#include "sysmod/serial.h"
#include "sysmod/banner.h"
#include "ModelCarControl.h"

/*

○仕様
  
・概要  
  PS3コントローラでRCカーを制御する．
  
・PS3コントローラからのコマンド  
  PS3コントローラからの操作要求は，SBDBTのPS3コントローラ/USBゲームパッド
  UART変換ファームウェア(http://www.runele.com/) により，RCB3形式に変換され，
  シリアルプロトコルで送られる．
  
・起動時
  全てのランプとブザーを0.5秒周期で3秒点滅させる．

・ボタン操舵
  左・右 ボタン
   ・ステアリングのニュートラルを調整．
   ・ボタンを押すと方向指示器がON，離すとOFF
  上ボタン
   ・ドライブにする．
  下ボタン
   ・リバースににする．
  L1/R1
   ・方向指示器をON/OFFする．
  L2/R2
   ・モータ制御のゲインを変更
  ×
   ・ブレーキ
  △
   ・ハザード
  ○
   ・メインヘッドライトON/OFF
  □
   ・サブメインヘッドライトON/OFF

  
○CANメッセージ仕様
  
 ●ボディ系コマンドCANメッセージ仕様

 送信周期     : 50ms
 メッセージID : 16 
 メッセージ長 : 11bit
 メッセージ   :
                  offset  bits   bit assign
  ヘッドランプ   :  0  :   1   : 0:OFF 1:ON 
  フォグランプ   :  1  :   1   : 0:OFF 1:ON 
  ブレーキランプ :  2  :   1   : 0:OFF 1:ON 
  バックランプ   :  3  :   1   : 0:OFF 1:ON 
  ブザー         :  5  :   2   : 0:OFF 1:ON 2:TOGGLE
  ウィンカー     :  8  :   3   : 0:OFF 1:L_ON 2:R_ON 3:LR_ON 4:L_BLINK 5:R_BLINK 6:LR_BLINK

 ●制御系コマンドCANメッセージ仕様

 送信周期     : メッセージ到達毎
 メッセージID : 3
 メッセージ長 : 40bit
 メッセージ   : 
  
                     offset  bits
   ステアリング角度 :  0  :   8  : 数値
   車速             :  8  :   8  : 数値
   ステアリング     
    ニュートラル    : 16  :   8  : 数値
   車速ゲイン       : 24  :   8  : 数値
   ブレーキ         : 32  :   1  : 1:ON 0:OFF
   ヘッドランプ     : 33  :   1  : 1:ON 0:OFF
   フォグランプ     : 34  :   1  : 1:ON 0:OFF
   Lウィンカー      : 35  :   1  : 1:ON 0:OFF
   Rウィンカー      : 36  :   1  : 1:ON 0:OFF
   ハザード         : 37  :   1  : 1:ON 0:OFF
 */

/*
 *  各系の動作状況保持変数
 */
boolean IsBodyOnECU      = FALSE;
boolean IsOperatorOnECU  = FALSE;
boolean IsControlerOnECU = FALSE;

/*
 *  アプリケーションモードが指定されていない場合の通知処理
 */
void
DoAppModeNone(void)
{
	volatile int i;

	led_init();
	while(1){
		for(i = 0; i < 1000000; i++);
		led_out(LED1|LED2|LED3|LED4);
		for(i = 0; i < 1000000; i++);
		led_out(LED_OFF);
	}
}

/*
 *  アプリケーションモードの取得
 */
boolean
GetAppMode(AppModeType *p_appmode)
{
	uint8 dipsw;
	boolean ret = TRUE;

	switch_dip_init();
	dipsw = switch_dip_sense();

	switch (dipsw) {
	  case DSW1:
		*p_appmode = AppMode_Body;
		break;
	  case DSW2:
		*p_appmode = AppMode_Cont;
		break;
	  case DSW3:
		*p_appmode = AppMode_Ope;
		break;
	  case DSW1|DSW2:
		*p_appmode = AppMode_Body_Cont;
		break;
	  case DSW1|DSW3:
		*p_appmode = AppMode_Body_Ope;
		break;
	  case DSW2|DSW3:
		*p_appmode = AppMode_Cont_Ope;
		break;
	  case DSW1|DSW2|DSW3:
		*p_appmode = AppMode_ALL;
		break;
	  default:
		ret = FALSE;
		break;
	}

	return ret;
}

/*
 *  ユーザメイン関数
 *
 *  アプリケーションモードの判断とカーネル起動
 */
sint32
main(void)
{
	AppModeType	crt_app_mode;

	if(!GetAppMode(&crt_app_mode)){
		/*
		 *  有効なアプリケーションモードが指定されてない場合
		 */
		DoAppModeNone();
	}

	if (crt_app_mode == AppMode_Body) {
		IsBodyOnECU = TRUE;
	}
	else if (crt_app_mode == AppMode_Cont) {
		IsControlerOnECU = TRUE;
	}
	else if (crt_app_mode == AppMode_Ope) {
		IsOperatorOnECU = TRUE;
	}
	else if (crt_app_mode == AppMode_Body_Cont) {
		IsBodyOnECU = TRUE;
		IsControlerOnECU = TRUE;
	}
	else if (crt_app_mode == AppMode_Body_Ope) {
		IsBodyOnECU = TRUE;
		IsOperatorOnECU = TRUE;
	}
	else if (crt_app_mode == AppMode_Cont_Ope) {
		IsControlerOnECU = TRUE;
		IsOperatorOnECU = TRUE;
	}
	else if (crt_app_mode == AppMode_ALL) {
		IsBodyOnECU = TRUE;
		IsControlerOnECU  = TRUE;
		IsOperatorOnECU  = TRUE;
	}

	/*
	 *  OS起動
	 */
	StartOS(crt_app_mode);

	while (1) {
	}
}

/*
 *  スタートアップフックルーチン
 */
void
StartupHook(void)
{
	/*
	 *  共通の初期化
	 */ 
	syslog_initialize();
	syslog_msk_log(LOG_UPTO(LOG_INFO));
	InitSerial();
	print_banner();
	syslog(LOG_INFO, "Model Car Control System Start");

	tCanInit();
	tCanSetMailBoxInfo(BODYCMD_CANMSG_RXMBOX_ID, 0, BODYCMD_CANMSG_ID, 0xff);
	tCanSetMailBoxInfo(CONTCMD_CANMSG_RXMBOX_ID, 0, CONTCMD_CANMSG_ID, 0xff);
	tCanEnable();

	/* LedBlinker関連の初期化 */
	LedBlinkerInit();

	/* ボディ系に関する初期化 */
	if(IsBodyOnECU) {
		BodyControlInit();
	}

	/* 操作系に関する初期化 */
	if(IsOperatorOnECU) {
		OperationInit();
	}

	/* 制御系に関する初期化 */
	if(IsControlerOnECU) {
		RcCarInit();
	}
}

/*
 *  シャットダウンフックルーチン
 */
void
ShutdownHook(StatusType Error)
{
	/* 終了ログ出力 */
	syslog(LOG_INFO, "");
	syslog(LOG_INFO, "Model Car Control System ShutDown");
	syslog(LOG_INFO, "ShutDownCode:%s", atk2_strerror(Error));
	syslog(LOG_INFO, "");
	TermSerial();
}

/*
 *  ボディ系への状態指示用変数
 */
volatile BODYCMD g_bodycmd;

/*
 *  周期送信とするか
 */
boolean g_bodycmd_canmsg_cyclic = FALSE;

/*
 *  ボディ系コマンドをCANメッセージとして送信する
 */
void
SendBodyCmdCanMag(void)
{
	uint8 tx_data[2];
	/* 前回送付の状態 */
	static BODYCMD g_pre_bodycmd;

	/* 状態指示に変化が無ければ送信しない */
	if (!g_bodycmd_canmsg_cyclic){
		if((g_bodycmd.headlamp == g_pre_bodycmd.headlamp)  &&
		   (g_bodycmd.foglamp   == g_pre_bodycmd.foglamp)  &&
		   (g_bodycmd.brakelamp == g_pre_bodycmd.brakelamp)&&
		   (g_bodycmd.backlamp  == g_pre_bodycmd.backlamp) &&
		   (g_bodycmd.buzzer    == g_pre_bodycmd.buzzer)   && 
		   (g_bodycmd.winker    == g_pre_bodycmd.winker)) {
			return;
		}
	}

	/* 送信メッセージを生成 */
	tx_data[0] = ((g_bodycmd.headlamp  << 7) &  0x80) | 
				 ((g_bodycmd.foglamp   << 6) &  0x40) |
				 ((g_bodycmd.brakelamp << 5) &  0x20) |
				 ((g_bodycmd.backlamp  << 4) &  0x10) |
				 ((g_bodycmd.buzzer    << 2) &  0x0c);
	tx_data[1] = ((g_bodycmd.winker    << 5) &  0xe0);

	tCanSetTxData(BODYCMD_CANMSG_TXMBOX_ID, BODYCMD_CANMSG_ID, tx_data, 2);

	g_pre_bodycmd = g_bodycmd;
}

/*
 *  ボディ系コマンドのCANメッセージからのアンパック
 */
void
UnpackBodyCmdCanMsg(uint8 *p_rx_data)
{
	g_bodycmd.headlamp  = (p_rx_data[0] & 0x80)? TRUE : FALSE;
	g_bodycmd.foglamp   = (p_rx_data[0] & 0x40)? TRUE : FALSE;
	g_bodycmd.brakelamp = (p_rx_data[0] & 0x20)? TRUE : FALSE;
	g_bodycmd.backlamp  = (p_rx_data[0] & 0x10)? TRUE : FALSE;
	g_bodycmd.buzzer    = (p_rx_data[0] & 0x0c) >> 2;
	g_bodycmd.winker    = (p_rx_data[1] & 0xe0) >> 5;
}

/*
 *  制御系へのコマンド更新通知用変数(同一ECU時)
 */
volatile boolean UpDateContCmd = FALSE;

/*
 *  制御系への指示用構造体
 */
volatile CONTCMD g_contcmd;

/*
 *  制御系への指示をCANメッセージとして送信
 */
void
SendContCmdCanMag(void)
{
	uint8 tx_data[5];

	/* 送信メッセージを生成 */
	tx_data[0] = g_contcmd.angle;
	tx_data[1] = g_contcmd.speed;
	tx_data[2] = g_contcmd.steern;
	tx_data[3] = g_contcmd.gain;
	tx_data[4] = ((g_contcmd.brake    << 7) &  0x80) | 
				 ((g_contcmd.headlamp << 6) &  0x40) |
				 ((g_contcmd.foglamp  << 5) &  0x20) |
				 ((g_contcmd.winker_l << 4) &  0x10) |
				 ((g_contcmd.winker_r << 3) &  0x08) |
				 ((g_contcmd.hazard   << 2) &  0x04);

	tCanSetTxData(CONTCMD_CANMSG_TXMBOX_ID, CONTCMD_CANMSG_ID, tx_data, 5);
}

/*
 *  制御系コマンドのCANメッセージからのアンパック
 */
void
UnpackContCmdCanMsg(uint8 *p_rx_data)
{
	g_contcmd.angle  = p_rx_data[0];
	g_contcmd.speed  = p_rx_data[1];
	g_contcmd.steern = p_rx_data[2];
	g_contcmd.gain   = p_rx_data[3];

	g_contcmd.brake    = (p_rx_data[4] & 0x80)? TRUE : FALSE;
	g_contcmd.headlamp = (p_rx_data[4] & 0x40)? TRUE : FALSE;
	g_contcmd.foglamp  = (p_rx_data[4] & 0x20)? TRUE : FALSE;
	g_contcmd.winker_l = (p_rx_data[4] & 0x10)? TRUE : FALSE;
	g_contcmd.winker_r = (p_rx_data[4] & 0x08)? TRUE : FALSE;
	g_contcmd.hazard   = (p_rx_data[4] & 0x04)? TRUE : FALSE;
}

/*
 *  エラーフックルーチン
 */
void
ErrorHook(StatusType Error)
{
	TaskType	tskid;
	StatusType	ercd;

	/* 多重起動要求エラーは無視 */
	if ((OSErrorGetServiceId() == OSServiceId_ActivateTask) && (Error == E_OS_LIMIT)) {
		return;
	}

	syslog(LOG_NOTICE, "## ErrorHook is called !! (%d)", Error);

	ercd = GetTaskID(&tskid);
	if (ercd == E_OK) {
		syslog(LOG_NOTICE, "GetTaskID: %d", (sintptr) tskid);
	}
	else {
		syslog(LOG_NOTICE, "GetTaskID is unavailable.");
	}
	syslog(LOG_NOTICE, "GetISRID: %d", (sintptr) GetISRID());
	syslog(LOG_NOTICE, "OSErrorGetServiceId(): %d", OSErrorGetServiceId());

	ShutdownOS(E_OK);

	return;
}

/*
 *  プロテクションフックルーチン
 */
ProtectionReturnType
ProtectionHook(StatusType FatalError)
{
	StatusType ercd;

	syslog(LOG_INFO, "");
	syslog(LOG_INFO, "ProtectionHook");

	if (FatalError == E_OS_STACKFAULT) {
		syslog(LOG_INFO, "E_OS_STACKFAULT");
		ercd = PRO_SHUTDOWN;
	}
	else if (FatalError == E_OS_PROTECTION_EXCEPTION) {
		syslog(LOG_INFO, "E_OS_PROTECTION_EXCEPTION");
		ercd = PRO_IGNORE;
	}
	else {
		ercd = PRO_SHUTDOWN;
	}

	return(ercd);
}
