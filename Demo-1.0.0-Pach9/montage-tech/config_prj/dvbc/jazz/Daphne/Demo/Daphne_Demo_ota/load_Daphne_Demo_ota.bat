@REM <-- Set all the variables**************PLEASE CHANGE*******************
@SET TARGET_PLATFORM=jazz
@SET PRJ_NAME=Daphne_Demo_ota
@SET PRJ_CONFIG=release
@REM -->**************PLEASE CHANGE*******************

@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->

@REM <-- REM Initialize Environment
@REM STEP 1: Config target platform
@CALL toolchain.bat %TARGET_PLATFORM%
rd /s /q .metadata

@REM STEP 2: create release dir
@mkdir %PRJ_NAME%\release
@REM -->

@dos2unix %PRJ_NAME%/post_build.sh

@REM <-- Load project
@REM STEP 1: Import project
codeblocks.exe  %PRJ_NAME%\%PRJ_NAME%.cbp


@REM <--  Cleanup environment
@CALL toolchain.bat clean
@REM -->
