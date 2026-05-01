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
 *  $Id: can_if.c 122 2015-07-17 01:31:28Z ertl-honda $
 */
#include "Os.h"
#include "can_if.h"
#include "hsbrh850f1l_device.h"
#include "prc_sil.h"

/*
 *  Port 0 Configration for RSCAN0
 *   P10_1 : RSCAN0_TX : 第2兼用
 *   P10_0 : RSCAN0_RX : 第2兼用
 */
#define RSCAN0_P10_MASK			((uint16) 0x0003)
#define RSCAN0_PMC10_INIT		((uint16) 0x0003)
#define RSCAN0_PFCAE10_INIT		((uint16) 0x0000)
#define RSCAN0_PFCE10_INIT		((uint16) 0x0000)
#define RSCAN0_PFC10_INIT		((uint16) 0x0003)
#define RSCAN0_PM10_INIT		((uint16) 0x0001)
#define RSCAN0_PIBC10_INIT		((uint16) 0x0001)

/*
 *   rscan_port_init();
 */
static void
rscan_port_init(void) {
	uint16 wk;

	/*
	 *  CAN0
	 *   RX : P10_1
	 *   TX : P10_0
	 */
	/* PFCAE0 設定 */
	wk = sil_reh_mem((void *) PFCAE(10));
	wk &= ~RSCAN0_P10_MASK;
	wk |= (RSCAN0_PFCAE10_INIT & RSCAN0_P10_MASK);
	sil_wrh_mem((void *) PFCAE(10), wk);

	/* PFCE0 設定 */
	wk = sil_reh_mem((void *) PFCE(10));
	wk &= ~RSCAN0_P10_MASK;
	wk |= (RSCAN0_PFCE10_INIT & RSCAN0_P10_MASK);
	sil_wrh_mem((void *) PFCE(10), wk);

	/* PFC0 設定 */
	wk = sil_reh_mem((void *) PFC(10));
	wk &= ~RSCAN0_P10_MASK;
	wk |= (RSCAN0_PFC10_INIT & RSCAN0_P10_MASK);
	sil_wrh_mem((void *) PFC(10), wk);

	/* PMC0 設定 */
	wk = sil_reh_mem((void *) PMC(10));
	wk &= ~RSCAN0_P10_MASK;
	wk |= (RSCAN0_PMC10_INIT & RSCAN0_P10_MASK);
	sil_wrh_mem((void *) PMC(10), wk);

	/* PM0 設定 */
	wk = sil_reh_mem((void *) PM(10));
	wk &= ~RSCAN0_P10_MASK;
	wk |= (RSCAN0_PM10_INIT & RSCAN0_P10_MASK);
	sil_wrh_mem((void *) PM(10), wk);

	/* PIBC0 設定 */
	wk = sil_reh_mem((void *) PIBC(10));
	wk &= ~RSCAN0_P10_MASK;
	wk |= (RSCAN0_PIBC10_INIT & RSCAN0_P10_MASK);
	sil_wrh_mem((void *) PIBC(10), wk);

	/* CANトランシーバポート初期化(現状CanTrcvが存在しないため，ここで初期化する) */
	sil_wrh_mem((void *) P(9U), sil_reh_mem((void *) P(9U)) & 0xFFFDU);
	sil_wrh_mem((void *) PM(9U), sil_reh_mem((void *) PM(9U)) & 0xFFFDU);
	sil_wrh_mem((void *) PMC(9U), sil_reh_mem((void *) PMC(9U)) & 0xFFFDU);
}

/* C_ISO_CANOSCクロック分周レジスタ */
#define CKSC_ICANOSCD_CTL			0xFFF8AA00U

static void
rscan_clock_init() {
	/* CANで使用するクロック(C_ISO_CANOSC)に メインクロック(MainOsc)を設定する */
	sil_wrw_mem((void *) PROTCMD1, 0x000000A5U);
	sil_wrw_mem((void *) CKSC_ICANOSCD_CTL, 0x00000001U);
	sil_wrw_mem((void *) CKSC_ICANOSCD_CTL, 0xFFFFFFFEU);
	sil_wrw_mem((void *) CKSC_ICANOSCD_CTL, 0x00000001U);
}

/*
 *  CANの初期化
 */
void
tCanInit(void)
{
	rscan_clock_init();

	rscan_port_init();

	/* CAM初期化 */
	RsCanInit(0);
}

void
tCanSetMailBoxInfo(uint8 mb_id, uint8 direction,
								uint32 id, uint8 mask)
{
	RsCanSetMailBoxInfo(0, mb_id, direction, 0, id, mask, 0);
}

/*
 *  CANの有効化
 */
void
tCanEnable(void)
{
	RsCanEnable(0);
}

/*
 *  CANデータの受信
 */
uint8
tCanGetRxData(uint8 mb_id, uint8 id, uint8 *p_data, uint8 *p_len){
	return RsCanGetRxData(0, mb_id, p_data, p_len);
}

/*
 *  CANデータの送信
 */
uint8
tCanSetTxData(uint8 mb_id, uint8 id, uint8 *p_data, uint8 len){
	return RsCanSetTxData(0, mb_id, id, p_data, len);
}
