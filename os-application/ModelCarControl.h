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
#include "can_if.h"

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
 *  DriveManagerタスクの起動周期
 */
#define DRIVEMANAGER_TASK_CYCLE_MS		20

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
 *  CANに関する定義 
 */

/*
 *  CAN関連 : ボディ系コマンド用CANメッセージID
 */
#define BODYCMD_CANMSG_ID		16

/*
 *  CAN関連 : 制御系コマンド用CANメッセージID
 */
#define CONTCMD_CANMSG_ID		3

/*
 *  各系の動作状況（同一ECUで動作していればTRUEとなる）
 */
extern boolean IsBodyOnECU;
extern boolean IsOperatorOnECU;
extern boolean IsControlerOnECU;

/*
 *  ボディー系への状態指示構造体
 */
typedef struct {
	boolean headlamp;
	boolean foglamp;
	boolean brakelamp;
	boolean backlamp;
	uint8   winker;
	uint8   buzzer;
}BODYCMD;

/*
 *  ボディー系への状態指示用変数
 */
extern volatile BODYCMD g_bodycmd;

/*
 *  ボディ系コマンドをCANメッセージとして送信する
 */
extern void SendBodyCmdCanMag(void);

/*
 *  ボディ系コマンドのCANメッセージからのアンパック
 */
extern void UnpackBodyCmdCanMsg(uint8 *p_rx_data);

/*
 *  制御系への指示構造体
 */
typedef struct {
	sint8	angle;
	sint8	speed;
	sint8	steern;
	sint8	gain;
	boolean brake;
	boolean headlamp;
	boolean foglamp;
	boolean winker_l;
	boolean winker_r;
	boolean hazard;	
}CONTCMD;

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
 *  制御系へのコマンド更新通知用変数(同一ECU時)
 */
extern volatile boolean UpDateContCmd;

/*
 *  制御系への指示用変数
 */
extern volatile CONTCMD g_contcmd;

/*
 *  制御系コマンドをCANメッセージとして送信
 */
extern void SendContCmdCanMag(void);

/*
 *  制御系コマンドのCANメッセージからのアンパック
 */
extern void UnpackContCmdCanMsg(uint8 *p_rx_data);

/*
 * ステアリング値設定用API
 */
extern void SetSteerAngle(sint8 angle);

/*
 *  車速設定用API
 */
extern void SetDriveSpeed(sint8 speed);

/*
 *  ブレーキ値設定用API
 */
extern void SetBrake(boolean brake);

/*
 *  BlinLed用の初期化(blinkled.c)
 */
extern void LedBlinkerInit(void);

/*
 *  ボディ系の初期化(BodyControl.c)
 */
extern void BodyControlInit(void);


/*
 *  制御系の初期化(RcCar.c)
 */
extern void RcCarInit(void);


/*
 *  操作系の初期化(RcCar.c)
 */
extern void OperationInit(void);

#endif /* TOPPERS_MODEL_CAR_CONTROL_H */
