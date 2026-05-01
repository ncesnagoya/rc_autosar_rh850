/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2014-2015 by Center for Embedded Computing Systems
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
 *  $Id: driver_if.h 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		ドライバへのインタフェース
 */

#ifndef TARGET_DRIVER_IF_H
#define TARGET_DRIVER_IF_H

#include "hsbrh850f1l_device.h"

/*
 *  ボディー系の初期化
 */
extern void tInitBody(void);

/*
 *  tSetEachLEDで指示する値
 */
#define WINKER_L_NO		0
#define WINKER_R_NO		1
#define BREAK_LAMP_NO	2
#define HEAD_LAMP_NO	3
#define FOG_LAMP_NO		4
#define BACK_LAMP_NO	5

/*
 *  個別のLEDの設定用マクロ
 */
#define WINKER_L_SET(on)		tSetEachLED(WINKER_L_NO, (on))
#define WINKER_R_SET(on)		tSetEachLED(WINKER_R_NO, (on))
#define HEAD_LAMP_SET(on)		tSetEachLED(HEAD_LAMP_NO,  (on))
#define FOG_LAMP_SET(on)		tSetEachLED(FOG_LAMP_NO, (on))
#define BRAKE_LAMP_SET(on)		tSetEachLED(BREAK_LAMP_NO, (on))
#define BACK_LAMP_SET(on)		tSetEachLED(BACK_LAMP_NO, (on))

/*
 *  個別のLEDの設定
 */
extern void tSetEachLED(uint8 no, boolean on);

/*
 *  LEDの設定
 */
extern void tSetLED(uint32 pattern);

/*
 *  ブザーのセット
 */
extern void tSetBuzzer(boolean onoff);

/*
 *  SDBDT接続のUARTの初期化
 */
extern void tInitSBDBTUart(void);

/*
 *  SDBDT接続のUARTからの受信
 */
extern boolean tReceiveSBDBTUart(unsigned char *p_str, uint32 len);

/*
 *  SDBDT接続のUARTへの送信
 */
extern boolean tSendSBDBTUart(unsigned char *p_str);

/*
 *  PWM関連の初期化
 */
extern void tInitPWM(uint32 cycle_us, uint32 servo_duty_us, uint32 esc_duty_us);

/*
 *  サーボのデューティ比の変更
 */
extern void tChangeServoDuty(uint32 servo_duty_us);

/*
 *  ESCのデューティ比の変更
 */
extern void tChangeESCDuty(uint32 esc_duty_us);

/*
 *  ESCのデューティ比の変更
 */
extern void tChangeESCDuty100NS(uint32 esc_duty_100ns);

#endif /* TARGET_DRIVER_IF_H */
