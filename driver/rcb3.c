/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 *
 *  Copyright (C) 2015 by Center for Embedded Computing Systems
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  $Id: rcb3.c 122 2015-07-17 01:31:28Z ertl-honda $
 */
/*
 *		RCB3電文解析処理
 */
#include "Os.h"
#include "rcb3.h"

/*
 *  PS3コントローラのボタンの数
 */
#define PS3_NUM_OF_BUTTON 14
#define PS3_NUM_OF_ANALOG 4

/*
 *  電文フォーマットのタイプ
 */
enum rcb3_st {
	RCB3_ST_HEADER_1,
	RCB3_ST_BUTTON_1,
	RCB3_ST_BUTTON_2, 
	RCB3_ST_ANALOG_1,
	RCB3_ST_ANALOG_2,
	RCB3_ST_ANALOG_3,
	RCB3_ST_ANALOG_4,
	RCB3_ST_SUM, 
};

/*
 *  受信メッセージ解析用グローバル変数
 */
static enum rcb3_st s_rcb3_st = RCB3_ST_HEADER_1;
static int			s_rcb3_rmsg_cnt;
uint8				g_rcb3_rmsg[RCB3_MSG_LEN];

/*
 *  ボタンメッセージ判別用グローバル変数
 */
static const uint16 msg_button_pattern[PS3_NUM_OF_BUTTON] = {
	RCB3_MSG_BUTTON_UP,
	RCB3_MSG_BUTTON_DOWN,
	RCB3_MSG_BUTTON_LEFT,
	RCB3_MSG_BUTTON_RIGHT,
	RCB3_MSG_BUTTON_TRIANGLE,
	RCB3_MSG_BUTTON_CROSS,
	RCB3_MSG_BUTTON_NOUGHT,
	RCB3_MSG_BUTTON_SQUARE,
	RCB3_MSG_BUTTON_L1,
	RCB3_MSG_BUTTON_L2,
	RCB3_MSG_BUTTON_R1,
	RCB3_MSG_BUTTON_R2,
	RCB3_MSG_BUTTON_START,
	RCB3_MSG_BUTTON_SELECT
};

/*
 *  ボタン状態保持用グローバル変数
 *   ダブルバッファとして前状態と現在状態を保存する
 */
static boolean	s_ps3button[2][PS3_NUM_OF_BUTTON];
static sint8	 s_ps3analog[2][PS3_NUM_OF_ANALOG];

/*
 * 前状態と現在状態を示すポインタ
 */
boolean	*p_g_pre_ps3button;
boolean	*p_g_cur_ps3button;
sint8	*p_g_pre_ps3analog;
sint8	*p_g_cur_ps3analog;


void
rcb3_Init(void)
{
	/* Init button */
	p_g_pre_ps3button = s_ps3button[0];
	p_g_cur_ps3button = s_ps3button[1];
	p_g_pre_ps3analog = s_ps3analog[0];
	p_g_cur_ps3analog = s_ps3analog[1];
}

/*
 *  ボタン情報は2バイトとなっているため2バイトデータとして戻す
 */
static uint16
rcb3_GetReceivedButton()
{
	uint16 button_cmd;

	button_cmd = (g_rcb3_rmsg[RCB3_MSG_OFFSET_BUTTON_H] << 8) | g_rcb3_rmsg[RCB3_MSG_OFFSET_BUTTON_L];

	return button_cmd;
}

/*
 *  ボタン状態の更新
 */
static void
rcb3_UpdateStatus(void)
{
	boolean *tmp_button;
	sint8 *tmp_analog;
	int i;
	uint16 button = rcb3_GetReceivedButton();

	/*
	 *  前状態と現在状態のポインタを更新
	 */
	tmp_button = p_g_pre_ps3button;
	tmp_analog = p_g_pre_ps3analog;

	p_g_pre_ps3button = p_g_cur_ps3button;
	p_g_pre_ps3analog = p_g_cur_ps3analog;

	p_g_cur_ps3button = tmp_button;
	p_g_cur_ps3analog = tmp_analog;

	/*
	 *  現在のデジタルボタンの状態を更新
	 */
	for(i = 0; i < PS3_NUM_OF_BUTTON; i++) {
		if ((button & msg_button_pattern[i])) {
			p_g_cur_ps3button[i] = TRUE;
		}
		else {
			p_g_cur_ps3button[i] = FALSE;
		}
	}

	/*
	 *  現在のアナログボタンの状態を更新
	 */
	for(i = 0; i < PS3_NUM_OF_ANALOG; i++) {
		uint8 val = g_rcb3_rmsg[RCB3_MSG_OFFSET_L_ANALOG_LR + i];
		p_g_cur_ps3analog[i] = ((sint8)val - RCB3_MSG_ANALOG_NEUTRAL);
	}
}

/*
 *  RCB3コマンドの解析
 * 
 *  受信した1バイト毎に引数に指定して呼び出す．
 *  電文の最後でTRUEを返す．
 */
boolean
rcb3_AddReceivedByte(uint8 c)
{
	boolean rval = FALSE;
	int i;
	uint8 sum = 0;

	switch (s_rcb3_st) {
	case RCB3_ST_HEADER_1:
		if (c == RCB3_MSG_HEADER_1) {
			s_rcb3_rmsg_cnt = 0;
			g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
			s_rcb3_st = RCB3_ST_BUTTON_1;
		}
		break;
	case RCB3_ST_BUTTON_1:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		s_rcb3_st = RCB3_ST_BUTTON_2;
		break;
	case RCB3_ST_BUTTON_2:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		s_rcb3_st = RCB3_ST_ANALOG_1;
		break;
	case RCB3_ST_ANALOG_1:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		s_rcb3_st = RCB3_ST_ANALOG_2;
		break;
	case RCB3_ST_ANALOG_2:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		s_rcb3_st = RCB3_ST_ANALOG_3;
		break;
	case RCB3_ST_ANALOG_3:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		s_rcb3_st = RCB3_ST_ANALOG_4;
		break;
	case RCB3_ST_ANALOG_4:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		s_rcb3_st = RCB3_ST_SUM;
		break;
	case RCB3_ST_SUM:
		g_rcb3_rmsg[s_rcb3_rmsg_cnt++] = c;
		for(i = 1; i < (RCB3_MSG_LEN - 1); i++) {
			sum += g_rcb3_rmsg[i];
		}
		sum &= 0x7f;
		if (c == sum) {
			rcb3_UpdateStatus();
			rval = TRUE;
		}
		else{
			rcb3_ErrorCb(sum, c, g_rcb3_rmsg);
		}
		s_rcb3_st = RCB3_ST_HEADER_1;
		break;
	}
	return rval;
}
