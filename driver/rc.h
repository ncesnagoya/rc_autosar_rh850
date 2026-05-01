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
 *  $Id: rc.h 122 2015-07-17 01:31:28Z ertl-honda $
 */

/*
 *		ESC(スピードコントローラ)及びサーボのパラメータ定義
 */

#ifndef ESC_H
#define ESC_H

/*
 *  ESCのパラメータ
 */

/*
 *  使用するESCの選択
 */
#define ESC_TLU104BK
//#define ESC_TLU105BK


/*
 *  各ESCのパラメータ
 */

/* TLU104BK用のパラメータ */
#ifdef ESC_TLU104BK
#define ESC_N_100NS				15000
#define ESC_DMAX_100NS			14000
#define ESC_RMAX_100NS			17000
#define ESC_DTH_100NS			14500 
#define ESC_RTH_100NS			15300
#define ESC_D2R_100NS			16500
#define ESC_BREAK_100NS			18000
#define ESC_WAIT_N2R_WAIT_10MS	4  /* ニュートラルからリバースまでの待ち時間(10MS単位) */
#define ESC_WAIT_D2B_WAIT_10MS	10 /* ドライブからブレーキまでの待ち時間(10MS単位)     */
#define ESC_WAIT_D2NR_WAIT_10MS	8  /* ドライブからリバースする際のニュートラルまでの待ち時間(10MS単位) */
#endif /* ESC_TLU104BK */


/* TLU105BK用のパラメータ */
#ifdef ESC_TLU105BK
#define ESC_N_US				1550
#define ESC_DMAX_100NS			14000
#define ESC_RMAX_100NS			17000
#define ESC_DTH_100NS			14900 
#define ESC_RTH_100NS			15500
#define ESC_D2R_100NS			16500
#define ESC_BREAK_100NS			20000
#define ESC_WAIT_N2R_WAIT_10MS	4  /* ニュートラルからリバースまでの待ち時間(10MS単位) */
#define ESC_WAIT_D2B_WAIT_10MS	8  /* ドライブからブレーキまでの待ち時間(10MS単位)     */
#define ESC_WAIT_D2NR_WAIT_10MS	8  /* ドライブからリバースする際のニュートラルまでの待ち時間(10MS単位) */
#endif /* ESC_TLU105BK */


/*
 *  パワトレ関連 : サーボのパラメータ
 */
#define SERVO_N_US		1570
#define SERVO_RMAX_US	1870 
#define SERVO_LMAX_US	1200

#endif /* ESC_H */
