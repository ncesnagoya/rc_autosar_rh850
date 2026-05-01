/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2014-2015 by Center for Embedded Computing Systems
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2014-2015 by FUJI SOFT INCORPORATED, JAPAN  
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
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  $Id: ModelCarControl.h 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		モデルーカー制御プログラムヘッダファイル
 */

#ifndef TOPPERS_MODEL_CAR_CONTROL_H
#define TOPPERS_MODEL_CAR_CONTROL_H

/*
 *  共通ヘッダファイル
 */
#include "driver_if.h"
#include "rc.h"

/*
 *  パワトレ関連 : ニュートラル時のステアリング角度の初期値
 */
#define STEER_NEUTRAL_INIT	0


/*
 * 車速，ステアリング最大/最小値
 */
#define DRIVE_SPEED_MAX		64
#define DRIVE_SPEED_MIN		-63
#define STEER_ANGLE_MAX		64
#define STEER_ANGLE_MIN		-63

/*
 *  ステアリングニュートラル調整値の最大/最小値
 */
#define STEER_NEUTRAL_MIN	-32
#define STEER_NEUTRAL_MAX	32

/*
 *  ゲイン調整値
 */
#define SPEED_GAIN_INIT		6
#define SPEED_GAIN_MID		10
#define SPEED_GAIN_MIN		3
#define SPEED_GAIN_MAX		17

/*
 *  ボディ系の起動周期
 */
#define BODY_CYCLE_MS		100U

/*
 *  BodyControlランナブルの起動周期
 */
#define BODYCONTROL_RUNNABLE_CYCLE_MS		20U

/*
 *  制御系 : 起動時のボディー系の接続テスト用設定
 *           点滅回数，点滅周期  
 */
#define INIT_BODY_BLINK_COUNT		3 
#define INIT_BODY_BLINK_CYCLE_MS	500

/*
 *  制御系 : CAN送信周期
 */
#define BODYCMD_CANMSG_SEND_CYCLE_MS	100

/*
 *  ウィンカーの点滅周期
 */
#define WINKER_INTERVAL_MS	500U

/*
 *  ブザー用アラームコールバックの周期
 */
#define BUZZER_CYCLE_NS		125U

/*
 *  ブザーの周期
 */
#define BUZZER_INTERVAL_MS	500U

/*
 *  ブザーの操作定義
 */
#define BUZZER_OFF			0U
#define BUZZER_ON			1U
#define BUZZER_INTERVAL_ON	2U

/*
 *  ウィンカーの操作定義
 */
#define WINKER_OFF		0U
#define WINKER_L_ON		1U
#define WINKER_R_ON		2U
#define WINKER_LR_ON	3U
#define WINKER_L_BLINK	4U
#define WINKER_R_BLINK	5U
#define WINKER_LR_BLINK	6U

/*
 *  制御系へのステアリングニュートラル関連の指示
 */
#define CONTCMD_STEERN_I	1U
#define CONTCMD_STEERN_R	2U
#define CONTCMD_STEERN_L	3U

/*
 *  制御系へのモータゲイン関連の指示
 */
#define CONTCMD_GAIN_I	1U
#define CONTCMD_GAIN_U	2U
#define CONTCMD_GAIN_D	3U

/*
 *  SW-C の初期化コード
 */
extern void CddLedBlinkerInit(void);
extern void CddBodyControlInit(void);
extern void CddControllerInit(void);
extern void CddRcCarInit(void);

/*
 * 無効値(受信していない)
 */
#define NOT_RECEIVE_COMMAND		0xFFFFFFFFU

#endif /* TOPPERS_MODEL_CAR_CONTROL_H */
