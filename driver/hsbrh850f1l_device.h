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
 *  $Id: hsbrh850f1l_device.h 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 * HSBRH850F1Lボード上のデバイス操作関数群の外部宣言
 */

#ifndef _RH850F1L_DEVICE_H_
#define _RH850F1L_DEVICE_H_

#include "Os.h"

/*
 * LEDのON/OFFパターン
 */
#define LED1		0x01
#define LED2		0x02
#define LED3		0x04
#define LED4		0x08
#define LED_OFF		0x00

/*
 *  LED接続ポート初期化
 */ 
extern void led_init(void);

/*
 *  LED接続ポート書き込み
 */ 
extern void led_out(uint8 led_data);

/*
 * DIPスイッチ接続ビット
 */
#define DSW1		0x01
#define DSW2		0x02
#define DSW3		0x04
#define DSW4		0x08

/*
 * DIPスイッチ接続ポート初期化
 */
extern void switch_dip_init(void);

/*
 * DIPスイッチ状態の読み込み
 */
extern uint8 switch_dip_sense(void);

/*
 * ポート1のPUSHスイッチ接続ビット
 */
#define PSW1		0x02
#define PSW2		0x01

/*
 * PUSHスイッチ接続ポート初期化
 */
extern void switch_push_init(void);

/*
 * PUSHスイッチ状態の読み込み
 */
extern uint8 switch_push_sense(void);

/*
 * LEDとスイッチの状態マクロ
 */
#define ON		1	/* LEDやスイッチON状態  */
#define OFF		0	/* LEDやスイッチOFF状態 */

#endif /* _RH850F1L_DEVICE_H_ */
