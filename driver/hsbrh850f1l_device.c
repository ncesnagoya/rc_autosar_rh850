/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 *
 *  Copyright (C) 2015 by Center for Embedded Computing Systems
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
 *  $Id: hsbrh850f1l_device.c 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 * HSBRH850F1Lボード上のデバイス操作関数群
 */
#include "Os.h"
#include "hsbrh850f1l_device.h"
#include "prc_sil.h"

/*
 * LED接続ポート
 * 
 * 拡張I/OボードのLED1-3はプログラマブル入出力ポート4に
 * 次のように接続されている．
 *  D1 : P8_4
 *  D2 : P8_5
 *  D3 : P8_6
 *  D4 : P8_7
 */

#define LED1_PORT  0x10
#define LED2_PORT  0x20
#define LED3_PORT  0x40
#define LED4_PORT  0x80

#define LED_PORT_MASK (LED4_PORT|LED3_PORT|LED2_PORT|LED1_PORT)

/*
 *  LED接続ポート初期化
 */ 
void
led_init(void){
	uint16 wk;

	/* PMC8 設定 */
	wk = sil_reh_mem((void *) PMC(8));
	wk &= ~LED_PORT_MASK;
	sil_wrh_mem((void *) PMC(8), wk);

	/* PM8 設定 */
	wk = sil_reh_mem((void *) PM(8));
	wk &= ~LED_PORT_MASK;
	sil_wrh_mem((void *) PM(8), wk);

	/* All Off */
	led_out(LED_OFF);
}

/*
 *  LED接続ポート書き込み
 */ 
void
led_out(uint8 led_data){
	uint16 wk;
	uint16 ptn = 0;

	if (led_data & LED1){
		ptn |= LED1_PORT;
	}
	if (led_data & LED2){
		ptn |= LED2_PORT;
	}
	if (led_data & LED3){
		ptn |= LED3_PORT;
	}
	if (led_data & LED4){
		ptn |= LED4_PORT;
	}

	ptn = ~ptn & LED_PORT_MASK;
	wk = sil_reh_mem((void *) P(8));
	wk &= ~LED_PORT_MASK;
	wk |= ptn;
	sil_wrh_mem((void *) P(8), wk);
}

/*
 * DIPスイッチ接続ポート
 * 
 * 拡張I/OボードのSW1-4はプログラマブル入出力ポート
 * にインバータを介して接続されている．
 *  DSW1 : P0_7
 *  DSW2 : P0_8
 *  DSW3 : P0_9
 *  DSW4 : P0_10
 */

#define DSW1_PORT  0x0080
#define DSW2_PORT  0x0100
#define DSW3_PORT  0x0200
#define DSW4_PORT  0x0400

#define DSW_PORT_MASK (DSW4_PORT|DSW3_PORT|DSW2_PORT|DSW1_PORT)

/*
 * DIPスイッチ接続ポート初期化
 */
void
switch_dip_init(void)
{
	uint16 wk;

	/* PMC0 設定 */
	wk = sil_reh_mem((void *) PMC(0));
	wk &= ~DSW_PORT_MASK;
	sil_wrh_mem((void *) PMC(0), wk);

	/* PM0 設定 */
	wk = sil_reh_mem((void *) PM(0));
	wk |= DSW_PORT_MASK;
	sil_wrh_mem((void *) PM(0), wk);

	/* PIBC0 設定 */
	wk = sil_reh_mem((void *) PIBC(0));
	wk |= DSW_PORT_MASK;
	sil_wrh_mem((void *) PIBC(0), wk);
}

/*
 * DIPスイッチ状態の読み込み
 */
uint8
switch_dip_sense(void)
{
	uint16 wk;
	uint16 ptn = 0;

	wk = sil_reh_mem((void *) PPR(0));

	if (!(wk & DSW1_PORT)){
		ptn |= DSW1;
	}
	if (!(wk & DSW2_PORT)){
		ptn |= DSW2;
	}
	if (!(wk & DSW3_PORT)){
		ptn |= DSW3;
	}
	if (!(wk & DSW4_PORT)){
		ptn |= DSW4;
	}

	return(ptn);
}

/*
 * PUSHスイッチ接続ポート
 * 
 *  PSW2 : P8_0 (INTP4) : 第3兼用
 *  PSW1 : P8_1 (INTP5) : 第3兼用
 */
#define PSW2_PORT  0x0002
#define PSW1_PORT  0x0001
#define PSW_PORT_MASK  0x0003

#define PSW_PORT_PFCAE_INIT  0x0000
#define PSW_PORT_PFCE_INIT   0x0003
#define PSW_PORT_PFC_INIT    0x0000
#define PSW_PORT_PMC_INIT    0x0003
#define PSW_PORT_PM_INIT     0x0003
#define PSW_PORT_PIBC_INIT   0x0003

#define FCLA0CTL4_INTPL 0xFFC34030
#define FCLA0CTL5_INTPL 0xFFC34034

/*
 * PUSHスイッチ接続ポート初期化
 */
void
switch_push_init(void)
{
	uint16 wk;

	/* PFCAE8 設定 */
	wk = sil_reh_mem((void *) PFCAE(8));
	wk &= ~PSW_PORT_MASK;
	wk |= (PSW_PORT_PFCAE_INIT & PSW_PORT_MASK);
	sil_wrh_mem((void *) PFCAE(8), wk);

	/* PFCE8 設定 */
	wk = sil_reh_mem((void *) PFCE(8));
	wk &= ~PSW_PORT_MASK;
	wk |= (PSW_PORT_PFCE_INIT & PSW_PORT_MASK);
	sil_wrh_mem((void *) PFCE(8), wk);

	/* PFC8 設定 */
	wk = sil_reh_mem((void *) PFC(8));
	wk &= ~PSW_PORT_MASK;
	wk |= (PSW_PORT_PFC_INIT & PSW_PORT_MASK);
	sil_wrh_mem((void *) PFC(8), wk);

	/* PMC8 設定 */
	wk = sil_reh_mem((void *) PMC(8));
	wk &= ~PSW_PORT_MASK;
	wk |= (PSW_PORT_PMC_INIT & PSW_PORT_MASK);
	sil_wrh_mem((void *) PMC(8), wk);

	/* PM8 設定 */
	wk = sil_reh_mem((void *) PM(8));
	wk &= ~PSW_PORT_MASK;
	wk |= (PSW_PORT_PM_INIT & PSW_PORT_MASK);
	sil_wrh_mem((void *) PM(8), wk);

	/* PIBC8 設定 */
	wk = sil_reh_mem((void *) PIBC(8));
	wk &= ~PSW_PORT_MASK;
	wk |= (PSW_PORT_PIBC_INIT & PSW_PORT_MASK);
	sil_wrh_mem((void *) PIBC(8), wk);

	/* エッジ検出立ち下がり */
	sil_wrb_mem((void *)FCLA0CTL4_INTPL, 0x02);
	sil_wrb_mem((void *)FCLA0CTL5_INTPL, 0x02);
}

/*
 * PUSHスイッチ状態の読み込み
 */
uint8
switch_push_sense(void)
{
	uint16 wk;
	uint16 mask;

	wk = sil_reh_mem((void *) PPR(8));
	mask =0;
	if(!(wk & PSW1_PORT)) {
		mask |= PSW1;
	}
	if(!(wk & PSW2_PORT)) {
		mask |= PSW2;
	}

	return(mask);
}
