@echo off
rem #
rem #  TOPPERS/A-RTEGEN
rem #      Automotive Runtime Environment Generator
rem #
rem #  Copyright (C) 2015 by Center for Embedded Computing Systems
rem #              Graduate School of Information Science, Nagoya Univ., JAPAN
rem #  Copyright (C) 2014-2015 by FUJI SOFT INCORPORATED, JAPAN
rem #
rem #  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
rem #  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
rem #  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
rem #  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
rem #      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
rem #      スコード中に含まれていること．
rem #  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
rem #      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
rem #      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
rem #      の無保証規定を掲載すること．
rem #  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
rem #      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
rem #      と．
rem #    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
rem #        作権表示，この利用条件および下記の無保証規定を掲載すること．
rem #    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
rem #        報告すること．
rem #  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
rem #      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
rem #      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
rem #      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
rem #      免責すること．
rem #
rem #  本ソフトウェアは，AUTOSAR（AUTomotive Open System ARchitecture）仕
rem #  様に基づいている．上記の許諾は，AUTOSARの知的財産権を許諾するもので
rem #  はない．AUTOSARは，AUTOSAR仕様に基づいたソフトウェアを商用目的で利
rem #  用する者に対して，AUTOSARパートナーになることを求めている．
rem #
rem #  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
rem #  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
rem #  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
rem #  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
rem #  の責任を負わない．
rem #
rem #  $Id: generator.bat 122 2015-07-17 01:31:28Z ertl-honda $
rem #

set PATH=%PATH%;C:\Program Files\Renesas Electronics\CS+\CC
set PATH=%PATH%;C:\Program Files (x86)\Renesas Electronics\CS+\CC

set ROOT_DIR=../../../
set OS_DIR=%ROOT_DIR%/atk2-sc1
set CFG_DIR=%OS_DIR%/cfg/cfg
set ABREX_DIR=%OS_DIR%/utils/abrex
set TARGET_DIR=%OS_DIR%/target/hsbrh850f1l_gcc
set COMSTACK_PATH=%ROOT_DIR%/a-comstack/
set RTE_DIR=%ROOT_DIR%/a-rtegen

if not exist %CFG_DIR%/cfg.exe (
	echo "cfg.exe is not found."
	pause
	exit
)

set APP_NAME=Ecuc_C

echo execute RTE generator
call %RTE_DIR%/bin/bin/rtegen.bat ^
	%TARGET_DIR%/target_hw_counter.arxml ^
	../../common/arxmls/DataTypes.arxml ^
	../../common/arxmls/BswMD_TxRx.arxml ^
	../System_EcuInstance_C.arxml ^
	%APP_NAME%.arxml

echo execute cfg Com
start /b %CFG_DIR%/cfg.exe --omit-symbol --pass 2 --kernel atk2 --ini-file %COMSTACK_PATH%/com/com.ini --api-table %COMSTACK_PATH%/com/com.csv -T %COMSTACK_PATH%/com/com.tf %APP_NAME%.arxml

echo execute cfg PduR
start /b %CFG_DIR%/cfg.exe --omit-symbol --pass 2 --kernel atk2 --ini-file %COMSTACK_PATH%/pdur/pdur.ini --api-table %COMSTACK_PATH%/pdur/pdur.csv -T %COMSTACK_PATH%/pdur/pdur.tf %APP_NAME%.arxml

echo execute cfg CanIf
start /b %CFG_DIR%/cfg.exe --omit-symbol --pass 2 --kernel atk2 --ini-file %COMSTACK_PATH%/canif/canif.ini --api-table %COMSTACK_PATH%/canif/canif.csv -T %COMSTACK_PATH%/canif/canif.tf %APP_NAME%.arxml

echo execute cfg Can

if exist cfg1_out.* del /F cfg1_out.*
if exist cancfg rmdir /S /Q cancfg
echo /* this is dummy */ > Can_Cfg.h
copy configure.mtpj cancfg.mtpj > nul
copy ..\..\common\tools\cancfg.py .  > nul
CubeSuite+.exe /ps cancfg.py cancfg.mtpj
move .\cancfg\cfg1_out.srec . > nul

start /b %CFG_DIR%/cfg.exe --cfg1-def-table %COMSTACK_PATH%/can/arch/rs_can/prc_def.csv --pass 2 --kernel atk2 --ini-file %COMSTACK_PATH%/can/can.ini --api-table %COMSTACK_PATH%/can/can.csv -I %COMSTACK_PATH%/can -I %COMSTACK_PATH%/can/arch/rs_can -T %COMSTACK_PATH%/can/target/hsbrh850f1l_ccrh/Can_Target.tf %APP_NAME%.arxml %TARGET_DIR%/target_hw_counter.arxml

pause
exit
