@echo off
rem #
rem #  TOPPERS/A-RTEGEN
rem #      Automotive Runtime Environment Generator
rem #
rem #  Copyright (C) 2015 by Embedded and Real-Time Systems Laboratory
rem #                             Toyohashi Univ. of Technology, JAPAN
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
rem #  $Id: ecu_extract.bat 122 2015-07-17 01:31:28Z ertl-honda $
rem #

set ROOT_DIR=../../
set RTE_DIR=%ROOT_DIR%/a-rtegen

echo execute ECU extractor
ruby %RTE_DIR%/ecu_extractor/ecu_extractor.rb ./System.arxml ../common/arxmls/RcCar.arxml 

pause
exit
