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
 *  $Id: taub_pwm.c 122 2015-07-17 01:31:28Z ertl-honda $
 */
/*
 *		モータ・サーボPWM制御用TAUBドライバ
 */
#include "Os.h"
#include "v850_gcc/rh850_f1l.h"
#include "taub_pwm.h"
#include "target_sysmod.h"

/*
 * 使用するユニットとチャネル
 */
#define PWM_TAUB_UNIT        0
#define PWM_TAUB_MASTER_CH   0
#define PWM_TAUB_SERVO_CH    8
#define PWM_TAUB_ESC_CH     10

/*
 *  Port 0 Configration for TAUB0 channel 8 and 10
 *   P0_11 : TAUB0O8 : 第3兼用
 *   P0_12 : TAUB010 : 第3兼用 
 */
#define PWM_P0_MASK			((uint16) 0x1800)
#define PWM_PMC0_INIT		((uint16) 0x1800)
#define PWM_PFCAE0_INIT		((uint16) 0x0000)
#define PWM_PFCE0_INIT		((uint16) 0x1800)
#define PWM_PFC0_INIT		((uint16) 0x0000)
#define PWM_PM0_INIT		((uint16) 0x0000)

/*
 * 出力ポートを有効に
 */
static void
init_pwm_port(void)
{
	uint16 wk;

	/*
	 * PORT0(PWM)
	 */
	/* PFCAE0 設定 */
	wk = sil_reh_mem((void *) PFCAE(0));
	wk &= ~PWM_P0_MASK;
	wk |= (PWM_PFCAE0_INIT & PWM_P0_MASK);
	sil_wrh_mem((void *) PFCAE(0), wk);

	/* PFCE0 設定 */
	wk = sil_reh_mem((void *) PFCE(0));
	wk &= ~PWM_P0_MASK;
	wk |= (PWM_PFCE0_INIT & PWM_P0_MASK);
	sil_wrh_mem((void *) PFCE(0), wk);

	/* PFC0 設定 */
	wk = sil_reh_mem((void *) PFC(0));
	wk &= ~PWM_P0_MASK;
	wk |= (PWM_PFC0_INIT & PWM_P0_MASK);
	sil_wrh_mem((void *) PFC(0), wk);

	/* PMC0 設定 */
	wk = sil_reh_mem((void *) PMC(0));
	wk &= ~PWM_P0_MASK;
	wk |= (PWM_PMC0_INIT & PWM_P0_MASK);
	sil_wrh_mem((void *) PMC(0), wk);

	/* PM0 設定 */
	wk = sil_reh_mem((void *) PM(0));
	wk &= ~PWM_P0_MASK;
	wk |= (PWM_PM0_INIT & PWM_P0_MASK);
	sil_wrh_mem((void *) PM(0), wk);
}

/*
 *  TAUB関連レジスタ
 */
#define TAUB_BASE(n)	((uint32) (0xffe30000U + (n * 0x1000U)))
#define TAUBTPS(n)		(TAUB_BASE(n) + 0x240U)
#define TAUBCDR(n, ch)	(TAUB_BASE(n) + (ch * 0x04U))
#define TAUBCNT(n, ch)	(TAUB_BASE(n) + 0x80U + (ch * 0x04U))
#define TAUBCMOR(n, ch)	(TAUB_BASE(n) + 0x200U + (ch * 0x04U))
#define TAUBCMUR(n, ch)	(TAUB_BASE(n) + 0xC0U + (ch * 0x04U))
#define TAUBTS(n)		(TAUB_BASE(n) + 0x1C4)
#define TAUBTT(n)		(TAUB_BASE(n) + 0x1C8U)
#define TAUBRDE(n)		(TAUB_BASE(n) + 0x260U)
#define TAUBRDM(n)		(TAUB_BASE(n) + 0x264U)

#define TAUBTOE(n)		(TAUB_BASE(n) + 0x5CU)
#define TAUBTOM(n)		(TAUB_BASE(n) + 0x248U)
#define TAUBTOC(n)		(TAUB_BASE(n) + 0x24CU)
#define TAUBTOL(n)		(TAUB_BASE(n) + 0x040U)

#define US_TO_COUNT(usec)	    ((usec * (TIMER_CLOCK_HZ/1000)) / 1000)
#define M100NS_TO_COUNT(usec)	(((usec * (TIMER_CLOCK_HZ/1000)) / 1000) / 10)

/*
 *  クロックはCK1を使用
 */
#define MCU_TAUB_MASK_CK1				((uint16) 0xff0f)
#define MCU_TAUB_CK1					((uint16) 0x0040) /* 40Mhz / 16 = 2.5Mhz */

/*
 *  タイマクロック周波数（Hz）（2.5MHz）
 */
#define TIMER_CLOCK_HZ		((uint32) 2500000)

#define MCU_TAUB_CKS_CK1   (0x01 << 14)
#define MCU_TAUB_MAS       (0x01 << 11)
#define MCU_TAUB_MD0       (0x01 << 0)
#define MCU_TAUB_STS       (0x04 << 8)
#define MCU_TAUB_MD_ONECNT (0x04 << 1)

/*
 *  PSM用TAUBの初期化
 */
void
taub_init_pwm(uint32 cycle_us, uint32 servo_duty_us, uint32 esc_duty_us)
{
	uint16 wk;

	/* IPERI2 : TAUB0 CPUCLK2 / 1 = 40MHz */

	/* 各タイマの停止 */
	sil_wrh_mem((void *) TAUBTT(PWM_TAUB_UNIT), (1 << PWM_TAUB_MASTER_CH));
	sil_wrh_mem((void *) TAUBTT(PWM_TAUB_UNIT), (1 << PWM_TAUB_SERVO_CH));
	sil_wrh_mem((void *) TAUBTT(PWM_TAUB_UNIT), (1 << PWM_TAUB_ESC_CH));

	/* プリスケーラを設定 */
	wk = sil_reh_mem((void *) TAUBTPS(PWM_TAUB_UNIT));
	wk &= MCU_TAUB_MASK_CK1;
	wk |= MCU_TAUB_CK1;
	sil_wrh_mem((void *) TAUBTPS(PWM_TAUB_UNIT), wk);

	/* マスタチャネルの設定 */
	sil_wrh_mem((void *) TAUBCMOR(PWM_TAUB_UNIT, PWM_TAUB_MASTER_CH),
				(MCU_TAUB_CKS_CK1|MCU_TAUB_MAS|MCU_TAUB_MD0));
	/* スレーブチャネル : サーボ用 */
	sil_wrh_mem((void *) TAUBCMOR(PWM_TAUB_UNIT, PWM_TAUB_SERVO_CH),
				(MCU_TAUB_CKS_CK1|MCU_TAUB_STS|MCU_TAUB_MD_ONECNT|MCU_TAUB_MD0));
	/* スレーブチャネル : ESC用 */
	sil_wrh_mem((void *) TAUBCMOR(PWM_TAUB_UNIT, PWM_TAUB_ESC_CH),
				(MCU_TAUB_CKS_CK1|MCU_TAUB_STS|MCU_TAUB_MD_ONECNT|MCU_TAUB_MD0));

	/* ユーザーモードの設定 */
	sil_wrb_mem((void *) TAUBCMUR(PWM_TAUB_UNIT, PWM_TAUB_MASTER_CH), 0x00);
	sil_wrb_mem((void *) TAUBCMUR(PWM_TAUB_UNIT, PWM_TAUB_SERVO_CH),  0x00);
	sil_wrb_mem((void *) TAUBCMUR(PWM_TAUB_UNIT, PWM_TAUB_ESC_CH),    0x00);

	/* 一斉書き換え禁止 */
	sil_wrb_mem((void *) TAUBRDE(PWM_TAUB_UNIT),0x00);

	/* チャネル出力モード(スレーブのみ) */
	sil_wrh_mem((void *) TAUBTOE(PWM_TAUB_UNIT), (1 << PWM_TAUB_SERVO_CH)|(1 << PWM_TAUB_ESC_CH));
	sil_wrh_mem((void *) TAUBTOM(PWM_TAUB_UNIT), (1 << PWM_TAUB_SERVO_CH)|(1 << PWM_TAUB_ESC_CH));
	sil_wrh_mem((void *) TAUBTOC(PWM_TAUB_UNIT), 0x00);
	sil_wrh_mem((void *) TAUBTOL(PWM_TAUB_UNIT), 0x00);

	/* カウンタの初期値を設定 */
	sil_wrh_mem((void *) TAUBCDR(PWM_TAUB_UNIT, PWM_TAUB_MASTER_CH), US_TO_COUNT(cycle_us));
	sil_wrh_mem((void *) TAUBCDR(PWM_TAUB_UNIT, PWM_TAUB_SERVO_CH),  US_TO_COUNT(servo_duty_us));
	sil_wrh_mem((void *) TAUBCDR(PWM_TAUB_UNIT, PWM_TAUB_ESC_CH),    US_TO_COUNT(esc_duty_us));

	/* PWM動作開始 */
	sil_wrh_mem((void *) TAUBTS(PWM_TAUB_UNIT),
				(1 << PWM_TAUB_MASTER_CH)|(1 << PWM_TAUB_SERVO_CH)|(1 << PWM_TAUB_ESC_CH));

	init_pwm_port();
}

/*
 *  サーボのデューティー比を変更
 */
void
taub_change_servo_duty(uint32 usec)
{
	sil_wrw_mem((void *) TAUBCDR(PWM_TAUB_UNIT, PWM_TAUB_SERVO_CH), US_TO_COUNT(usec));
}

/*
 *  ESCのデューティー比を変更
 */
void
taub_change_esc_duty(uint32 usec)
{
	sil_wrw_mem((void *) TAUBCDR(PWM_TAUB_UNIT, PWM_TAUB_ESC_CH), US_TO_COUNT(usec));
}

/*
 *  ESCのデューティー比を変更(100nsec精度)
 */
void
taub_change_esc_duty_100ns(uint32 nsec)
{
	sil_wrw_mem((void *) TAUBCDR(PWM_TAUB_UNIT, PWM_TAUB_ESC_CH), M100NS_TO_COUNT(nsec));
}
