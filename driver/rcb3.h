/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2014-2015 by Center for Embedded Computing Systems
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  $Id: rcb3.h 122 2015-07-17 01:31:28Z ertl-honda $
 */
/*
 *		RCB3電文解析処理外部宣言
 */
#ifndef _RCB3_H_
#define _RCB3_H_

/*
 *  RCB3 Message format
 */
#define RCB3_MSG_LEN 8
#define RCB3_MSG_HEADER_1 0x80

#define RCB3_MSG_OFFSET_BUTTON_H 1
#define RCB3_MSG_OFFSET_BUTTON_L 2
#define RCB3_MSG_OFFSET_L_ANALOG_LR  3
#define RCB3_MSG_OFFSET_L_ANALOG_UD  4
#define RCB3_MSG_OFFSET_R_ANALOG_LR  5
#define RCB3_MSG_OFFSET_R_ANALOG_UD  6

#define RCB3_MSG_BUTTON_UP        0x0001
#define RCB3_MSG_BUTTON_DOWN      0x0002
#define RCB3_MSG_BUTTON_RIGHT     0x0004
#define RCB3_MSG_BUTTON_LEFT      0x0008
#define RCB3_MSG_BUTTON_TRIANGLE  0x0010
#define RCB3_MSG_BUTTON_CROSS     0x0020
#define RCB3_MSG_BUTTON_NOUGHT    0x0040
#define RCB3_MSG_BUTTON_SQUARE    0x0100
#define RCB3_MSG_BUTTON_L1        0x0200
#define RCB3_MSG_BUTTON_L2        0x0400
#define RCB3_MSG_BUTTON_R1        0x0800
#define RCB3_MSG_BUTTON_R2        0x1000
#define RCB3_MSG_BUTTON_START     0x0003
#define RCB3_MSG_BUTTON_SELECT    0x000C

#define RCB3_MSG_ANALOG_MAX      127
#define RCB3_MSG_ANALOG_MIN        1
#define RCB3_MSG_ANALOG_NEUTRAL   64


/*
 * 前状態と現在状態を示すポインタ
 */
extern boolean  *p_g_pre_ps3button;
extern boolean  *p_g_cur_ps3button;
extern sint8    *p_g_pre_ps3analog;
extern sint8    *p_g_cur_ps3analog;
extern uint8     g_rcb3_rmsg[RCB3_MSG_LEN];

/*
 *  各ボタン状態の番号
 */
#define NO_PS3BUTTON_UP        0
#define NO_PS3BUTTON_DOWN      1
#define NO_PS3BUTTON_LEFT      3
#define NO_PS3BUTTON_RIGHT     2
#define NO_PS3BUTTON_TRIANGLE  4
#define NO_PS3BUTTON_CROSS     5
#define NO_PS3BUTTON_NOUGHT    6
#define NO_PS3BUTTON_SQUARE    7
#define NO_PS3BUTTON_L1        8
#define NO_PS3BUTTON_L2        9
#define NO_PS3BUTTON_R1        10
#define NO_PS3BUTTON_R2        11
#define NO_PS3BUTTON_START     12
#define NO_PS3BUTTON_SELECT    13

#define NO_PS3ANALOG_L_LR  0
#define NO_PS3ANALOG_L_UD  1
#define NO_PS3ANALOG_R_LR  2
#define NO_PS3ANALOG_R_UD  3

/*
 *  初期化処理 
 */
void rcb3_Init(void);

/*
 *  RCB3コマンドの解析
 * 
 *  受信した1バイト毎に引数に指定して呼び出す．
 *  電文の最後でTRUEを返す．
 */
extern boolean rcb3_AddReceivedByte(uint8 c);

/*
 *  エラー時のコールバック関数（ユーザーファイルで定義）
 */
extern void rcb3_ErrorCb(uint8 calc_sum, uint8 recv_sum, uint8 *p_rcb3_rmsg);

#endif /* _RCB3_H_ */
