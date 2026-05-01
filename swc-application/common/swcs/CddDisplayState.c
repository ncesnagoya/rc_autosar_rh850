/*
 *  Copyright (C) 2013-2014 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2014-2015 by FUJI SOFT INCORPORATED, JAPAN
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
 *  $Id: CddDisplayState.c 122 2015-07-17 01:31:28Z ertl-honda $
 */
#include "Rte_CddDisplayState.h"
#include "ModelCarControl.h"
#include "t_syslog.h"

/* 取得した表示情報 */
static IDT_Angle	cur_angle = 0;
static IDT_Speed	cur_speed = 0;
static IDT_PWM		cur_pwm_angle = 0;
static IDT_PWM		cur_pwm_speed = 0;

/* 差分有無情報 */
static boolean		changed_angle = FALSE;
static boolean		changed_speed = FALSE;
static boolean		changed_pwm_angle = FALSE;
static boolean		changed_pwm_speed = FALSE;

/* 取得した表示状態 */
static IDT_Switch	display_angle = FALSE;
static IDT_Switch	display_speed = FALSE;
static IDT_PWM		display_pwm_angle = FALSE;
static IDT_PWM		display_pwm_speed = FALSE;

/* 全情報更新処理 */
void
update_info(void)
{
	Std_ReturnType	ercd;
	IDT_Angle		angle;
	IDT_Speed		speed;
	IDT_PWM			pwm_angle;
	IDT_PWM			pwm_speed;

	/* 表示情報更新 */
	ercd = Rte_Read_VehicleInfoIn_angle(&angle);
	ercd += Rte_Read_VehicleInfoIn_speed(&speed);
	ercd += Rte_Read_VehicleInfoIn_pwm_angle(&pwm_angle);
	ercd += Rte_Read_VehicleInfoIn_pwm_speed(&pwm_speed);

	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_VehicleInfoIn()");
		return;
	}

	/* 前回取得値との差分を確認しながら情報を更新する */
	if (cur_angle != angle) {
		cur_angle = angle;
		changed_angle = TRUE;
	}
	if (cur_speed != speed) {
		cur_speed = speed;
		changed_speed = TRUE;
	}
	if (cur_pwm_angle != pwm_angle) {
		cur_pwm_angle = pwm_angle;
		changed_pwm_angle = TRUE;
	}
	if (cur_pwm_speed != pwm_speed) {
		cur_pwm_speed = pwm_speed;
		changed_pwm_speed = TRUE;
	}
}

/* 情報表示処理 */
void
display_info(IDT_Switch forced)
{
	uint8 str[64];
	Std_ReturnType	ercd;

	/* 表示状態更新 */
	ercd = Rte_Read_DisplayModeIn_angle(&display_angle);
	ercd += Rte_Read_DisplayModeIn_speed(&display_speed);
	ercd += Rte_Read_DisplayModeIn_pwm_angle(&display_pwm_angle);
	ercd += Rte_Read_DisplayModeIn_pwm_speed(&display_pwm_speed);
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_DisplayModeIn()");
		return;
	}

	/* デリミタ */
	tSendSBDBTUart("===============================\r\n");

	/* 操舵角 */
	if ((display_angle == TRUE) &&
		((forced == TRUE) || (changed_angle == TRUE))) {
		sprintf(str, "SteeringAngle: %d\r\n", cur_angle);
		tSendSBDBTUart(str);
	}

	/* 駆動速度 */
	if ((display_speed == TRUE) &&
		((forced == TRUE) || (changed_speed == TRUE))) {
		sprintf(str, "DrivingSpeed: %d\r\n", cur_speed);
		tSendSBDBTUart(str);
	}

	/* 操舵角PWM */
	if ((display_pwm_angle == TRUE) &&
		((forced == TRUE) || (changed_pwm_angle == TRUE))) {
		sprintf(str, "ServoPwmAngle: %d\r\n", cur_pwm_angle);
		tSendSBDBTUart(str);
	}

	/* 駆動速度PWM */
	if ((display_pwm_speed == TRUE) &&
		((forced == TRUE) || (changed_pwm_speed == TRUE))) {
		sprintf(str, "EscPwmSpeed: %d\r\n", cur_pwm_speed);
		tSendSBDBTUart(str);
	}
}

void
DisplayState(void)
{
	Std_ReturnType		ercd;
	IDT_Cycle			cycle = 0U;
	IDT_Switch			differ;
	static uint8		cnt = 0U;
	static uint8		max = 0U;
	static IDT_Cycle	pre_cycle = 0U;
	static boolean		one_shot = FALSE;

	/* 表示周期読み込み */
	ercd = Rte_Read_DisplayModeIn_cycle(&cycle);
	if (ercd != RTE_E_OK) {
		syslog(LOG_INFO, "## Error: Rte_Read_DisplayModeIn_cycle()");
	}

	/* 表示しない */
	if (cycle == 0U) {
		/* ワンショット済みフラグOFF */
		one_shot = FALSE;
	}
	/* ワンショット */
	else if (cycle == 1U) {
		/* 情報を更新して強制的に1回だけ表示 */
		if (one_shot == FALSE) {
			update_info();
			display_info(TRUE);
			/* 他の表示周期に切り替わるまで表示しない */
			one_shot = TRUE;
		}
	}
	else if (((cycle % 100U) == 0U) && (cycle <= 10000U)) {
		/* 前回から表示周期が変わっている場合カウンタ，最大値をリセット */
		if (cycle != pre_cycle) {
			cnt = 0U;
			max = cycle / 100U;
		}

		/* 周期に合わせて状態表示 */
		cnt++;
		if (max < cnt) {
			ercd = Rte_Read_DisplayModeIn_differ(&differ);
			if (ercd != RTE_E_OK) {
				syslog(LOG_INFO, "## Error: Rte_Read_DisplayModeIn_cycle()");
			}
			else {
				update_info();
				if (differ == TRUE) {
					display_info(FALSE);
				}
				else {
					display_info(TRUE);
				}
			}
			cnt = 0U;
		}

		/* 前回値更新 */
		pre_cycle = cycle;

		/* ワンショット済みフラグOFF */
		one_shot = FALSE;
	}
	else {
		syslog(LOG_INFO, "## Error: Invalid cycle value: %d", cycle);
	}
}
