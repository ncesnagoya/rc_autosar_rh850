@echo off

set APP_NAME=ModelCarControl
set ROOT_DIR=../
set OS_DIR=%ROOT_DIR%/atk2-sc1
set ABREX_DIR=%OS_DIR%/utils/abrex

ruby %ABREX_DIR%/abrex.rb ./%APP_NAME%.yaml
move .\%APP_NAME%.arxml . > nul

pause
exit
