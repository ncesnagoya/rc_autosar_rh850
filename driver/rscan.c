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
 *  $Id: rscan.c 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 * 基本方針
 * ・割込みを使用しない
 * ・チャネル毎のバッファはメールボックスと表現して0からの番号として扱う
 *  
 * 送信
 * ・送信バッファ(16個)を用いて通信
 *
 * 受信 
 * ・受信バッファ(16個)を用いて通信
 * ・受信ルールは16個 
 */
#include "Os.h"
#include "rscan.h"
#include "t_syslog.h"
#include "t_stdlib.h"
#include "sysmod/serial.h"
#include "sysmod/syslog.h"


/*
 *  定数定義
 */
#define TNUM_RX_BUFF		16U

/* 送信バッファ番号 */
#define CAN_TX_BUFF_NO(ch, id)	((ch) * 16U + id)

/* 受信バッファ番号 */
#define CAN_RX_BUFF_NO(ch, id)	((ch) * TNUM_RX_BUFF + id)

/* 受信ルール数 */
#define TNUM_RXRULE			16U


/* ビット操作用 */
#define CAN_BIT0	0x00000001U
#define CAN_BIT1	0x00000002U
#define CAN_BIT2	0x00000004U
#define CAN_BIT3	0x00000008U
#define CAN_BIT4	0x00000010U
#define CAN_BIT5	0x00000020U
#define CAN_BIT6	0x00000040U
#define CAN_BIT7	0x00000080U
#define CAN_BIT8	0x00000100U
#define CAN_BIT9	0x00000200U
#define CAN_BIT10	0x00000400U
#define CAN_BIT11	0x00000800U
#define CAN_BIT12	0x00001000U
#define CAN_BIT13	0x00002000U
#define CAN_BIT14	0x00004000U
#define CAN_BIT15	0x00008000U
#define CAN_BIT16	0x00010000U
#define CAN_BIT17	0x00020000U
#define CAN_BIT18	0x00040000U
#define CAN_BIT19	0x00080000U
#define CAN_BIT20	0x00100000U
#define CAN_BIT21	0x00200000U
#define CAN_BIT22	0x00400000U
#define CAN_BIT23	0x00800000U
#define CAN_BIT24	0x01000000U
#define CAN_BIT25	0x02000000U
#define CAN_BIT26	0x04000000U
#define CAN_BIT27	0x08000000U
#define CAN_BIT28	0x10000000U
#define CAN_BIT29	0x20000000U
#define CAN_BIT30	0x40000000U
#define CAN_BIT31	0x80000000U

/* グローバル */
#define RSCAN0GCFG					(*(volatile uint32 *) (RSCAN0_BASE + 0x0084U))
#define RSCAN0GCTR					(*(volatile uint32 *) (RSCAN0_BASE + 0x0088U))
#define RSCAN0GSTS					(*(volatile uint32 *) (RSCAN0_BASE + 0x008CU))
#define RSCAN0GAFLCFG0				(*(volatile uint32 *) (RSCAN0_BASE + 0x009CU))
#define RSCAN0GAFLCFG1				(*(volatile uint32 *) (RSCAN0_BASE + 0x00A0U))
#define RSCAN0GAFLECTR				(*(volatile uint32 *) (RSCAN0_BASE + 0x0098U))

/* 受信バッファ */
#define RSCAN0RMNB					(*(volatile uint32 *) (RSCAN0_BASE + 0x00A4U))
#define RSCAN0RMND(y)				(*(volatile uint32 *) (RSCAN0_BASE + 0x00A8U + ((y) * 0x04)))
#define RSCAN0RMID(id)				(*(volatile uint32 *) (RSCAN0_BASE + 0x0600U + ((id) * 0x10)))
#define RSCAN0RMPTQ(id)				(*(volatile uint32 *) (RSCAN0_BASE + 0x0604U + ((id) * 0x10)))
#define RSCAN0RMDF0(id, byte)		(*(volatile uint8 *) (RSCAN0_BASE + 0x0608U + ((id) * 0x10) + (byte)))
#define RSCAN0RMDF1(id, byte)		(*(volatile uint8 *) (RSCAN0_BASE + 0x060CU + ((id) * 0x10) + (byte)))

/* チャネル */
#define RSCAN0CmCFG(ch)				(*(volatile uint32 *) (RSCAN0_BASE + 0x0000U + ((ch) * 0x10U)))
#define RSCAN0CmCTR(ch)				(*(volatile uint32 *) (RSCAN0_BASE + 0x0004U + ((ch) * 0x10U)))
#define RSCAN0CmSTS(ch)				(*(volatile uint32 *) (RSCAN0_BASE + 0x0008U + ((ch) * 0x10U)))
#define RSCAN0THLCC(ch)				(*(volatile uint32 *) (RSCAN0_BASE + 0x0400U + ((ch) * 0x04U)))

/* 受信ルール */
#define RSCAN0GAFLID(rule)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0500U + ((rule) * 0x10U)))
#define RSCAN0GAFLM(rule)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0504U + ((rule) * 0x10U)))
#define RSCAN0GAFLP0(rule)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0508U + ((rule) * 0x10U)))
#define RSCAN0GAFLP1(rule)			(*(volatile uint32 *) (RSCAN0_BASE + 0x050CU + ((rule) * 0x10U)))

/* 送受信FIFOバッファ(受信用) */
#define RSCAN0CFCC(buf_no)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0118U + ((buf_no) * 0x04U)))
#define RSCAN0CFSTS(buf_no)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0178U + ((buf_no) * 0x04U)))
#define RSCAN0CFPCTR(buf_no)		(*(volatile uint32 *) (RSCAN0_BASE + 0x01D8U + ((buf_no) * 0x04U)))
#define RSCAN0CFID(buf_no)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0E80U + ((buf_no) * 0x10U)))
#define RSCAN0CFPTR(buf_no)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0E84U + ((buf_no) * 0x10U)))
#define RSCAN0CFDF0(buf_no, byte)	(*(volatile uint8 *) (RSCAN0_BASE + 0x0E88U + ((buf_no) * 0x10U) + (byte)))
#define RSCAN0CFDF1(buf_no, byte)	(*(volatile uint8 *) (RSCAN0_BASE + 0x0E8CU + ((buf_no) * 0x10U) + (byte)))

/* 送信バッファ */
#define RSCAN0TMC(buf_no)			(*(volatile uint8 *)  (RSCAN0_BASE + 0x0250U + (buf_no)))
#define RSCAN0TMSTS(buf_no)			(*(volatile uint8  *) (RSCAN0_BASE + 0x02D0U + ((buf_no) * 0x01U)))
#define RSCAN0TMID(buf_no)			(*(volatile uint32 *) (RSCAN0_BASE + 0x1000U + ((buf_no) * 0x10U)))
#define RSCAN0TMPTR(buf_no)			(*(volatile uint32 *) (RSCAN0_BASE + 0x1004U + ((buf_no) * 0x10U)))
#define RSCAN0TMDF0(buf_no, byte)	(*(volatile uint8 *) (RSCAN0_BASE + 0x1008U + ((buf_no) * 0x10U) + (byte)))
#define RSCAN0TMDF1(buf_no, byte)	(*(volatile uint8 *) (RSCAN0_BASE + 0x100CU + ((buf_no) * 0x10U) + (byte)))


/* 送信キュー */
#define RSCAN0TXQCC(ch)				(*(volatile uint32 *) (RSCAN0_BASE + 0x03A0U + ((ch) * 0x04U)))
#define RSCAN0TXQSTS(ch)			(*(volatile uint32 *) (RSCAN0_BASE + 0x03C0U + ((ch) * 0x04U)))
#define RSCAN0TXQPCTR(ch)			(*(volatile uint32 *) (RSCAN0_BASE + 0x03E0U + ((ch) * 0x04U)))

/* 送信履歴 */
#define RSCAN0THLSTS(ch)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0420U + ((ch) * 0x04U)))
#define RSCAN0THLACC(ch)			(*(volatile uint32 *) (RSCAN0_BASE + 0x1800U + ((ch) * 0x04U)))
#define RSCAN0THLPCTR(ch)			(*(volatile uint32 *) (RSCAN0_BASE + 0x0440U + ((ch) * 0x04U)))

/* C_ISO_CANOSCクロック分周レジスタ */
#define CKSC_ICANOSCD_CTL			0xFFF8AA00U

/* 受信ルール数設定(ch0～3)用 */
#define GAFLCFG0_REG_BIT_SHIFT(ch)	(24U - ((ch) * 8U))

/* 受信ルール数設定(ch4～5)用 */
#define GAFLCFG1_REG_BIT_SHIFT(ch)	(24U - (((ch) - 4U) * 8U))


STATUS
RsCanInit(uint8 ctrl_id) {
	/*
	 *  グローバルな初期化
	 */
	/* CAN用RAMクリアステータスフラグチェック */
	if ((RSCAN0GSTS & CAN_BIT3) != 0U) {
		return STATUS_ERROR;
	}

	/* グローバルストップモードを解除 */
	RSCAN0GCTR &= ~CAN_BIT2;

	/* グローバルリセットモードに遷移 */
	RSCAN0GCTR |= CAN_BIT0;

	/* CANで使用するクロックをCKSCLK_ICANOSC(clk_xincan)に設定 */
	RSCAN0GCFG |= CAN_BIT4;

	/*
	 *  チャネル毎の初期化
	 */
	/* チャネルストップモードを解除 */
	RSCAN0CmCTR(ctrl_id) &= ~CAN_BIT2;

	/* [CAN259] チャネルリセットモードに遷移 */
	RSCAN0CmCTR(ctrl_id) |= CAN_BIT0;

	/*
	 * 送信設定
	 */
	/* 送信キューは使用しない */
	RSCAN0TXQCC(ctrl_id) = 0x00;

	/* 送信履歴バッファは使用しない */
	RSCAN0THLCC(ctrl_id) = 0x00;

	/*
	 *  受信設定
	 */
	/* コントローラが4ch以上か判定*/
	if (ctrl_id < 4U) {
		/* 受信ルール数設定(ch0～3) */
		RSCAN0GAFLCFG0 |= TNUM_RXRULE << GAFLCFG0_REG_BIT_SHIFT(ctrl_id);
	}
	else {
		/* 受信ルール数設定(ch4～5) */
		RSCAN0GAFLCFG1 |= TNUM_RXRULE << GAFLCFG1_REG_BIT_SHIFT(ctrl_id);
	}

	/*
	 *  ボーレートの設定
	 */
	/* チャネルコンフィグレーション設定(ボーレート:500kbps) */
	RSCAN0CmCFG(ctrl_id) = RSCAN_BRP_500;

	return STATUS_OK;
}


void
RsCanEnable(uint8 ctrl_id){
	/* グローバルリセットモードからグローバル動作モードに遷移 */
	RSCAN0GCTR &= ~(CAN_BIT1 | CAN_BIT0);

	/* [CAN261] チャネル通信モードに遷移 */
	RSCAN0CmCTR(ctrl_id) &= ~(CAN_BIT1 | CAN_BIT0);

	/* チャネル通信モードになるまでループ(#67) */
	while ((RSCAN0CmSTS(ctrl_id) &CAN_BIT7) == 0U) {
	}

	/* 受信バッファ数設定 */
	RSCAN0RMNB = TNUM_RX_BUFF * TNUM_CH;
}

void
RsCanDisable(uint8 ctrl_id) {
	/* チャネルリセットモードに遷移 */
	RSCAN0CmCTR(ctrl_id) &= 0xFFFFFFFDU;
}


STATUS
RsCanSetTxData(uint8 ctrl_id, uint8 mb_id, uint32 id, uint8 *p_data, uint8 len){
	uint32			tm_bit;
	uint8			i;

	/* 送信中ならリターン  */
	if((RSCAN0TMSTS(CAN_TX_BUFF_NO(ctrl_id, mb_id)) & CAN_BIT0) == CAN_BIT0) {
		return STATUS_SENDING;
	}

	RSCAN0TMSTS(CAN_TX_BUFF_NO(ctrl_id, mb_id)) = 0;

	/* CAN-ID設定 */
	RSCAN0TMID(CAN_TX_BUFF_NO(ctrl_id, mb_id)) = id;

	/* DLCと送信ラベル情報取得 */
	tm_bit = (uint32) len << 28U;
	tm_bit |= (uint32) 2 << 16U;
	
	/* DLCと送信ラベル設定 */
	RSCAN0TMPTR(CAN_TX_BUFF_NO(ctrl_id, mb_id)) = tm_bit;

	/* [CAN059][CAN427] データ格納 */
	/* データ設定(0～3バイト) */
	for (i = 0U; i < 4U; i++) {
		RSCAN0TMDF0(CAN_TX_BUFF_NO(ctrl_id, mb_id), i) = p_data[i];
	}
	
	/* 4バイト以降のメッセージ有無判定 */
	if (len > 4U) {
		/* データ設定(4～7バイト) */
		for (i = 4U; i < len; i++) {
			RSCAN0TMDF1(CAN_TX_BUFF_NO(ctrl_id, mb_id), (i - 4U)) = p_data[i];
		}
	}

	/* 送信要求 */
	RSCAN0TMC(CAN_TX_BUFF_NO(ctrl_id, mb_id)) = CAN_BIT0;

	return STATUS_OK;
}


STATUS
RsCanSetMailBoxInfo(uint8 ctrl_id, uint8 mb_id, uint8 direction,
					uint8 ide, uint32 id, uint8 mask, uint32 remote){
	static uint8	rx_rule_index_cnt = 0U;
	uint8			rx_rule_index;

	if(direction == 1) {
		return STATUS_ERROR;
	}

	/* 受信ルールテーブル書き込み許可 */
	RSCAN0GAFLECTR |= CAN_BIT8;

	/* ルール番号の抽出 */
	rx_rule_index = rx_rule_index_cnt % 16U;

	/* ページ番号が切り替わるか判定 */
	if (rx_rule_index == 0U) {
		/* ページ番号を設定 */
		RSCAN0GAFLECTR = (CAN_BIT8 | (uint32) (rx_rule_index_cnt / 16U));
	}

	/* フィルタマスク設定 */
	RSCAN0GAFLM(rx_rule_index) = (CAN_BIT31 | mask);

	/* 受信ルールID設定(CAN-IDレジスタ) */
	RSCAN0GAFLID(rx_rule_index) = (uint32) id;

	/* 受信ルールを使用する受信バッファの指定 */
	RSCAN0GAFLP0(rx_rule_index) = (1 << 15U) | (1 << 16U) | (mb_id << 8U);

	/* 受信ルールインデックスをインクリメント */
	rx_rule_index_cnt++;

	/* 受信ルールテーブル書き込み禁止 */
	RSCAN0GAFLECTR &= ~CAN_BIT8;
	
	return STATUS_OK;
 }



STATUS
RsCanGetRxData(uint8 ctrl_id, uint8 mb_id, uint8 *p_data, uint8 *p_len){
	uint8	local_dlc;
	uint8	i;
	uint8	bufid;

	bufid = CAN_RX_BUFF_NO(ctrl_id, mb_id);

	/* 受信しているか確認 ToDo 現状は決めうち */
	if((RSCAN0RMND(bufid / 32) & (1U << (bufid % 32))) == 0) {
		*p_len = 0;
		return STATUS_EMPTY;
	}

	/* フラグクリア */
	RSCAN0RMND(bufid / 32) = ~(1U << (bufid % 32));

	local_dlc = (RSCAN0RMPTQ(mb_id) >> 28);
	*p_len = local_dlc;

	/* データ格納(0～3バイト) */
	for (i = 0U; i < 4U; i++) {
		p_data[i] = RSCAN0RMDF0(mb_id, i);
	}

	/* データが4byte以上か判定 */
	if (local_dlc > 4U) {
		/* データ格納(4～7バイト) */
		for (i = 4U; i < local_dlc; i++) {
			p_data[i] = RSCAN0RMDF1(mb_id, (i - 4U));
		}
	}

	return STATUS_OK;
}
