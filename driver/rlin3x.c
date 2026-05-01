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
 *  $Id: rlin3x.c 122 2015-07-17 01:31:28Z ertl-honda $
 */
/*
 *		通信モジュール用UART(RLIN)ドライバ
 */
#include "Os.h"
#include "t_stdlib.h"
#include "target_sysmod.h"
#include "rlin3x.h"

#undef RLIN3x_BASE
#define RLIN3x_BASE	RLIN32_BASE

#define RLIN3xLWBR_B		0x00000001
#define RLIN3xLBRP01_H		0x00000002
#define RLIN3xLBRP0_B		0x00000002
#define RLIN3xLBRP1_B		0x00000003
#define RLIN3xLSTC_B		0x00000004
#define RLIN3xLMD_B			0x00000008
#define RLIN3xLBFC_B		0x00000009
#define RLIN3xLSC_B			0x0000000a
#define RLIN3xLWUP_B		0x0000000b
#define RLIN3xLIE_B			0x0000000c
#define RLIN3xLEDE_B		0x0000000d
#define RLIN3xLCUC_B		0x0000000e
#define RLIN3xLTRC_B		0x00000010
#define RLIN3xLMST_B		0x00000011
#define RLIN3xLST_B			0x00000012
#define RLIN3xLEST_B		0x00000013
#define RLIN3xLDFC_B		0x00000014
#define RLIN3xLIDB_B		0x00000015
#define RLIN3xLCBR_B		0x00000016
#define RLIN3xLUDB0_B		0x00000017
#define RLIN3xLDBR1_B		0x00000018
#define RLIN3xLDBR2_B		0x00000019
#define RLIN3xLDBR3_B		0x0000001a
#define RLIN3xLDBR4_B		0x0000001b
#define RLIN3xLDBR5_B		0x0000001c
#define RLIN3xLDBR6_B		0x0000001d
#define RLIN3xLDBR7_B		0x0000001e
#define RLIN3xLDBR8_B		0x0000001f
#define RLIN3xLUOER_B		0x00000020
#define RLIN3xLUOR1_B		0x00000021
#define RLIN3xLUTDR_H		0x00000024
#define RLIN3xLUTDRL_B		0x00000024
#define RLIN3xLUTDRH_B		0x00000025
#define RLIN3xLURDR_H		0x00000026
#define RLIN3xLURDRL_B		0x00000026
#define RLIN3xLURDRH_B		0x00000027
#define RLIN3xLUWTDR_H		0x00000028
#define RLIN3xLUWTDRL_B		0x00000028
#define RLIN3xLUWTDRH_B		0x00000029

/*
 *  7 : TX
 *  8 : RX
 */

/*
 *  Port 0 Configration for RLIN32
 *   P10_13 : RLIN32RX : 第2兼用
 *   P10_14 : RLIN32TX : 第2兼用 
 */
#define RLIN32_P10_MASK			((uint16) 0x6000)
#define RLIN32_PMC10_INIT		((uint16) 0x6000)
#define RLIN32_PFCAE10_INIT		((uint16) 0x0000)
#define RLIN32_PFCE10_INIT		((uint16) 0x0000)
#define RLIN32_PFC10_INIT		((uint16) 0x6000)
#define RLIN32_PM10_INIT		((uint16) 0x2000)
#define RLIN32_PIBC10_INIT		((uint16) 0x2000)

/*
 *  RLIN32用ポートの初期化
 */
void
rlin3x_portinit(void)
{
	uint16 wk;

	/*
	 * PORT10(RLIN32)
	 */
	/* PFCAE10 設定 */
	wk = sil_reh_mem((void *) PFCAE(10));
	wk &= ~RLIN32_P10_MASK;
	wk |= (RLIN32_PFCAE10_INIT & RLIN32_P10_MASK);
	sil_wrh_mem((void *) PFCAE(10), wk);

	/* PFCE10 設定 */
	wk = sil_reh_mem((void *) PFCE(10));
	wk &= ~RLIN32_P10_MASK;
	wk |= (RLIN32_PFCE10_INIT & RLIN32_P10_MASK);
	sil_wrh_mem((void *) PFCE(10), wk);

	/* PFC10 設定 */
	wk = sil_reh_mem((void *) PFC(10));
	wk &= ~RLIN32_P10_MASK;
	wk |= (RLIN32_PFC10_INIT & RLIN32_P10_MASK);
	sil_wrh_mem((void *) PFC(10), wk);

	/* PMC10 設定 */
	wk = sil_reh_mem((void *) PMC(10));
	wk &= ~RLIN32_P10_MASK;
	wk |= (RLIN32_PMC10_INIT & RLIN32_P10_MASK);
	sil_wrh_mem((void *) PMC(10), wk);

	/* PM10 設定 */
	wk = sil_reh_mem((void *) PM(10));
	wk &= ~RLIN32_P10_MASK;
	wk |= (RLIN32_PM10_INIT & RLIN32_P10_MASK);
	sil_wrh_mem((void *) PM(10), wk);

	/* PIBC10 設定 */
	wk = sil_reh_mem((void *) PIBC(10));
	wk &= ~RLIN32_P10_MASK;
	wk |= (RLIN32_PIBC10_INIT & RLIN32_P10_MASK);
	sil_wrh_mem((void *) PIBC(10), wk);
}

/*
 *   ボーレートレジスタへの設定値 PCLK=40MHz, 115200bps
 */
#define RLIN3xLWBR_VAL		0xf2
#define RLIN3xLBRP01_VAL	10

#define RxBUFF_SIZE 256
#define TxBUFF_SIZE 256

static uint8 s_rxbuff[RxBUFF_SIZE];
static uint8 s_txbuff[TxBUFF_SIZE];

static int s_rxbuff_tail;
static int s_rxbuff_head;
static int s_rxbuff_cnt;
static int s_txbuff_tail;
static int s_txbuff_head;
static int s_txbuff_cnt;
static boolean s_do_tx;

/*
 * 初期化
 */
void
rlin3x_init(void)
{
	s_rxbuff_tail = 0;
	s_rxbuff_head = 0;
	s_rxbuff_cnt  = 0;
	s_txbuff_tail = 0;
	s_txbuff_head = 0;
	s_txbuff_cnt  = 0;
	s_do_tx = FALSE;

	/* Uart Mode を有効(ノイズフィルタも有効) */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLMD_B), 0x31);

	/* ボーレート設定 */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLWBR_B), RLIN3xLWBR_VAL);
	sil_wrh_mem((void *) (RLIN3x_BASE + RLIN3xLBRP01_H), RLIN3xLBRP01_VAL);

	/* エラー検出許可 */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLEDE_B), 0x0d);

	/* データ フォーマット */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLBFC_B), 0x00);

	/* リセット解除 */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLCUC_B), 0x01);

	/* リセット解除待ち */
	while (sil_reb_mem((void *) (RLIN3x_BASE + RLIN3xLMST_B)) == 0x00) {
	}

	/* 送受信動作許可 */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLUOER_B), 0x03);

	/* 送受信割込み許可 */
	sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLIE_B), 0x03);

	/* ポートの初期化 */
	rlin3x_portinit();
}


/*
 * 受信したデータ長 
 */
int
rlin3x_get_rxcnt(void)
{
	return s_rxbuff_cnt;
}

/*
 *  RLIN32受信 
 */
boolean
rlin3x_receive(unsigned char *data, int len)
{
	boolean result = TRUE;
	int loop;

	SuspendOSInterrupts();
	if (s_rxbuff_cnt < len) {
		result = FALSE;
	}else {
		for(loop = 0; loop < len; loop++) {
			*data++ = s_rxbuff[s_rxbuff_head++];
			s_rxbuff_head = s_rxbuff_head % RxBUFF_SIZE;
			s_rxbuff_cnt--;
		}
	}
	ResumeOSInterrupts();

	return result;
}

/*
 *  RLIN32送信
 */
boolean
rlin3x_send(const unsigned char *data, int len)
{
	int loop;
	char c;

	/* 空きが無ければエラー */
	if (s_txbuff_cnt + len > TxBUFF_SIZE) {
		return FALSE;
	}

	/* リングバッファにコピー */
	for(loop = 0; loop < len; loop++){
		s_txbuff[s_txbuff_tail++] = *data++;
		s_txbuff_tail = s_txbuff_tail % TxBUFF_SIZE;
		s_txbuff_cnt++;
	}

	if (!(s_do_tx)) {
		SuspendOSInterrupts();
		/* リングバッファから取り出し送信 */
		c = s_txbuff[s_txbuff_head++];
		s_txbuff_head = s_txbuff_head % TxBUFF_SIZE;
		s_txbuff_cnt--;
		s_do_tx = TRUE;
		sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLUTDRL_B), c);
		ResumeOSInterrupts();
	}

	return TRUE;
}

void
rlin3x_tx_interrupt(void)
{
	char c;

	if (s_txbuff_cnt == 0) {
		/* バッファが空 */
		s_do_tx = FALSE;
	}
	else {
		/* バッファからデータを取り出し出力 */
		c = s_txbuff[s_txbuff_head++];
		s_txbuff_head = s_txbuff_head % TxBUFF_SIZE;
		s_txbuff_cnt--;
		sil_wrb_mem((void *) (RLIN3x_BASE + RLIN3xLUTDRL_B), c);
		s_do_tx = TRUE;
	}
}

ISR(RLIN3x_TX_ISR)
{
	rlin3x_tx_interrupt();
}

void
rlin3x_rx_interrupt(void)
{
	char c;

	c = sil_reb_mem((void *) (RLIN3x_BASE + RLIN3xLURDRL_B));

	if (s_rxbuff_cnt < RxBUFF_SIZE) {
		s_rxbuff[s_rxbuff_tail++] = c;
		s_rxbuff_tail = s_rxbuff_tail % RxBUFF_SIZE;
		s_rxbuff_cnt++;
	}
}

ISR(RLIN3x_RX_ISR)
{
	rlin3x_rx_interrupt();
}
