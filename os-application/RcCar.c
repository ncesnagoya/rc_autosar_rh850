/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2014-2015 by Center for Embedded Computing Systems
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
 *  $Id: RcCar.c 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		RC-CAR関連
 */

#include "Os.h"
#include "ModelCarControl.h"
#include "t_syslog.h"
#include "rc.h"

/*
 *  PWMの周期
 */
#define PWM_CYCLE_US	20000

/*
 * ステアリング値/車速/ブレーキの指令値
 */
volatile boolean	brake_val = FALSE;
volatile int		speed_val = 0;
volatile float		angle_val = 0;

/*
 * ステアリング値と車速のPWM値
 */
static sint32	servo_pwm = SERVO_N_US;
static sint32	esc_pwm_100nsec = ESC_N_100NS;

/*
 *  RC-CAR用の初期化
 */
void
RcCarInit(void)
{
	tInitPWM(PWM_CYCLE_US, SERVO_N_US, ESC_N_100NS/10);
}

/*
 * ステアリング値設定用API
 */
void
SetSteerAngle(sint8 angle)
{
	if ((angle <= STEER_ANGLE_MAX) && (angle >= STEER_ANGLE_MIN)) {
		angle_val = angle;
	}
}

/*
 * 車速設定用API
 */
void
SetDriveSpeed(sint8 speed)
{
	if ((speed <= DRIVE_SPEED_MAX) && (speed >= DRIVE_SPEED_MIN)) {
		speed_val = speed;
	}
}

/*
 * ブレーキ値設定用API
 */
void
SetBrake(boolean brake)
{
	brake_val = brake;
}

/*
 *  ESCの制御状態
 */
typedef enum {
	DRIVE_STATE_DRIVE,
	DRIVE_STATE_REVERSE,
	DRIVE_STATE_DNEUTRAL,
	DRIVE_STATE_NEUTRAL,
	DRIVE_STATE_DRIVE2REVERSE,
	DRIVE_STATE_DRIVE2BRAKE,
	DRIVE_STATE_REVERSE2BRAKE,
}DRIVE_STATE;

/*
 *  パワートレインタスク
 */
TASK(PowerTrainTask)
{
	static	boolean		executed = FALSE;
	static uint8		adjust_cnt = 0U;
	static boolean		pre_brake_val = FALSE;
	static DRIVE_STATE	drive_state = DRIVE_STATE_NEUTRAL;

	/* 初回起動時のみ実行 */
	if (executed == FALSE) {
		syslog(LOG_INFO, "PowerTrainTask : Start!");
		executed = TRUE;
	}

	/*
	 * ステアリング制御
	 */
	if ((angle_val <= STEER_ANGLE_MAX) && (angle_val >= STEER_ANGLE_MIN)) {
		if (angle_val >= 0) {
			servo_pwm = SERVO_N_US - ((((SERVO_N_US - SERVO_LMAX_US) * 100) / (64 * 100)) * angle_val);
		}
		else {
			servo_pwm = SERVO_N_US + ((((SERVO_RMAX_US - SERVO_N_US) * 100) / (64 * 100)) * (-angle_val));
		}
		/* ステアリング値設定 */
		tChangeServoDuty(servo_pwm);
	}

	/*
	 * 車速制御
	 */
	if (adjust_cnt != 0U) {
		adjust_cnt--;
		if(adjust_cnt == 0){
			if(drive_state  == DRIVE_STATE_REVERSE2BRAKE) {
				tChangeESCDuty100NS(ESC_BREAK_100NS);
			}
			if(drive_state == DRIVE_STATE_DRIVE2REVERSE){
				drive_state = DRIVE_STATE_REVERSE;
				adjust_cnt = ESC_WAIT_N2R_WAIT_10MS;
				tChangeESCDuty100NS(ESC_N_100NS);
			}
		}
	}
	else if (pre_brake_val != brake_val) {
		/* ブレーキ状態が変化 */
		pre_brake_val = brake_val;
		/* ブレーキ開放 */
		if (brake_val == FALSE) {
			if (speed_val < 0) {
				drive_state = DRIVE_STATE_REVERSE;
				adjust_cnt = ESC_WAIT_N2R_WAIT_10MS;
				tChangeESCDuty100NS(ESC_N_100NS);
			}
		}
		else {
			/* ブレーキ */
			if((drive_state == DRIVE_STATE_DRIVE) || (drive_state == DRIVE_STATE_DNEUTRAL)) {
				drive_state = DRIVE_STATE_DRIVE2BRAKE;
				tChangeESCDuty100NS(ESC_BREAK_100NS);
			}
			else if((drive_state == DRIVE_STATE_REVERSE) || (drive_state == DRIVE_STATE_NEUTRAL)) {
				drive_state = DRIVE_STATE_REVERSE2BRAKE;
				adjust_cnt = ESC_WAIT_D2B_WAIT_10MS;
				tChangeESCDuty100NS(ESC_DTH_100NS);
			}
		}
	}
	else if (brake_val == FALSE) {
		/* 車速判定 */
		if ((speed_val <= DRIVE_SPEED_MAX) && (speed_val >= DRIVE_SPEED_MIN)) {
			/* 前進から後退に切り替わった場合 */
			if ((drive_state == DRIVE_STATE_DNEUTRAL)  && (speed_val < 0)) {
				drive_state = DRIVE_STATE_DRIVE2REVERSE;
				adjust_cnt = ESC_WAIT_D2NR_WAIT_10MS;
				tChangeESCDuty100NS(ESC_D2R_100NS);
			}
			else {
				/* 前進 */
				if (speed_val > 0) {
					esc_pwm_100nsec = ESC_DTH_100NS + (((ESC_DTH_100NS - ESC_DMAX_100NS) / 64) * (-1) * speed_val);
					drive_state = DRIVE_STATE_DRIVE;
				}
				/* ニュートラル */
				if (speed_val == 0) {
					esc_pwm_100nsec = ESC_N_100NS;
					if (drive_state == DRIVE_STATE_DRIVE) {
						drive_state = DRIVE_STATE_DNEUTRAL;
					} else if (drive_state != DRIVE_STATE_DNEUTRAL){
						drive_state = DRIVE_STATE_NEUTRAL;
					}
				}
				/* 後退 */
				if (speed_val < 0) {
					esc_pwm_100nsec = ESC_RTH_100NS + (((ESC_RMAX_100NS - ESC_RTH_100NS) / 64) * (-1) * speed_val);
					drive_state = DRIVE_STATE_REVERSE;
				}
				/* 車速値設定 */
				tChangeESCDuty100NS(esc_pwm_100nsec);
			}
		}
	}

	TerminateTask();
}
