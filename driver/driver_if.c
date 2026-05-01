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
 *  $Id: driver_if.c 122 2015-07-17 01:31:28Z ertl-honda $
 */
#include "Os.h"
#include "driver_if.h"
#include "rlin3x.h"
#include "hsbrh850f1l_device.h"
#include "taub_pwm.h"
#include "prc_sil.h"

/*
 *  IOポート経由で接続されたTLU01の制御
 */

/*
 * TLU01-1 : P10_11
 * TLU01-2 : P10_10
 * TLU01-3 : P10_9
 * TLU01-4 : P10_8
 * TLU01-5 : P10_7
 * TLU01-6 : P10_6
 * TLU01-7 : P9_6
 * TLU01-8 : P9_5
 */

#define TLU01_P10  0x0FC0 /* P10_11, P10_10, P10_9, P10_8, P10_7, P10_6 */
#define TLU01_P9   0x0060 /* P9_6, P9_5 */

/*
 *  ボディー系の初期化
 */
void
tInitBody(void)
{
	uint16 wk;

	/* PMC10 設定 */
	wk = sil_reh_mem((void *) PMC(10));
	wk &= ~TLU01_P10;
	sil_wrh_mem((void *) PMC(10), wk);

	/* PM10 設定 */
	wk = sil_reh_mem((void *) PM(10));
	wk &= ~TLU01_P10;
	sil_wrh_mem((void *) PM(10), wk);

	wk = sil_reh_mem((void *) P(10));
	wk &= ~TLU01_P10;
	sil_wrh_mem((void *) P(10), wk);

	/* PMC9 設定 */
	wk = sil_reh_mem((void *) PMC(9));
	wk &= ~TLU01_P9;
	sil_wrh_mem((void *) PMC(9), wk);

	/* PM9 設定 */
	wk = sil_reh_mem((void *) PM(9));
	wk &= ~TLU01_P9;
	sil_wrh_mem((void *) PM(9), wk);

	wk = sil_reh_mem((void *) P(9));
	wk &= ~TLU01_P9;
	sil_wrh_mem((void *) P(9), wk);
}

#define TLU01_PORT_MIN 0
#define TLU01_PORT_MAX 7

static const uint32 tlu01_pinfo[8] ={
	0x0A0200, /* WINKER_L   */
	0x0A0400, /* WINKER_R   */
	0x0A0080, /* BREAK_LAMP */
	0x0A0800, /* HEAD_LAMP  */
	0x0A0100, /* FOG_LAMP   */
	0x0A0040, /* BACK_LAMP  */
	0x090040, /* BUZZER     */
	0x090020, /* Reserve    */
};

void
tlu01_set(uint8 no, boolean on)
{
	uint16 wk;
	uint16 pattern;
	uint32 pinfo;
	uint8  portno;

	if (!(no <= TLU01_PORT_MAX)) {
		return;
	}

	pinfo = tlu01_pinfo[no];
	pattern =  pinfo & 0xffff;
	portno  =  (tlu01_pinfo[no] >> 16);

	wk = sil_reh_mem((void *) P(portno));
	if (on) {
		wk |=  pattern;
	}
	else {
		wk &= ~pattern;
	}
	sil_wrh_mem((void *) P(portno), wk);
}

/*
 *  個別のLEDの設定
 */
void
tSetEachLED(uint8 no, boolean on)
{
	tlu01_set(no, on);
}

/*
 *  接続LEDの最大数
 */
#define MAX_LED_NO		6

/*
 *  LEDの設定
 */
void
tSetLED(uint32 pattern)
{
	int loop;

	for(loop = 0; loop <= MAX_LED_NO; loop++){
		if((pattern & (1 << loop)) != 0){
			tlu01_set(loop, TRUE);
		}
		else{
			tlu01_set(loop, FALSE);
		}
	}
}

void
tSetBuzzer(boolean onoff){
	uint16 wk;

	wk = sil_reh_mem((void *) P(9));
	if(onoff == TRUE){
		wk |= 1 << 6;
	}
	else {
		wk &= ~(1 << 6);
	}
	sil_wrh_mem((void *) P(9), wk);
}

/*
 *  SDBDT接続のUARTの初期化
 */
void
tInitSBDBTUart(void)
{
	rlin3x_init();
}

/*
 *  SDBDT接続のUARTからの受信
 */
boolean
tReceiveSBDBTUart(unsigned char *p_str, uint32 len)
{
	return rlin3x_receive(p_str, len);
}

/*
 *  SDBDT接続のUARTへ送信
 */
boolean
tSendSBDBTUart(unsigned char *p_str)
{
	uint16	len = strlen(p_str);
	return rlin3x_send(p_str, len);
}

/*
 *  PWM関連の初期化
 */
void
tInitPWM(uint32 cycle_us, uint32 servo_duty_us, uint32 esc_duty_us)
{
	taub_init_pwm(cycle_us, servo_duty_us, esc_duty_us);
}

/*
 *  サーボのデューティ比の変更
 */
void
tChangeServoDuty(uint32 servo_duty_us)
{
	taub_change_servo_duty(servo_duty_us);
}

/*
 *  ESCのデューティ比の変更
 */
void
tChangeESCDuty(uint32 esc_duty_us)
{
	taub_change_esc_duty(esc_duty_us);
}

/*
 *  ESCのデューティ比の変更
 */
void
tChangeESCDuty100NS(uint32 esc_duty_100ns)
{
	taub_change_esc_duty_100ns(esc_duty_100ns);
}
