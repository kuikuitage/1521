@REM <-- Set all the variables**************PLEASE CHANGE*******************
@SET TARGET_PLATFORM=jazz
@SET PRJ_NAME=Daphne_Demo
@SET PRJ_CODE=Daphne_Nongwang
@SET PRJ_CUSTOMER=Demo
@SET PRJ_CONFIG=release
@REM -->**************PLEASE CHANGE*******************

@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->

@REM <-- REM Initialize Environment
@REM STEP 1: Config target platform
@CALL toolchain.bat %TARGET_PLATFORM%


@REM STEP 2: Copy binary from binary to release\binary
@mkdir %PRJ_NAME%\release
@mkdir %PRJ_NAME%\release\binary\boot_chip
@mkdir %PRJ_NAME%\release\binary\boot_fpga
@mkdir %PRJ_NAME%\release\binary\ota_binary
@cp -R ../../../../../../prj/dvbc/%PRJ_CODE%/binary %PRJ_NAME%/release/
@cp -R ../../../../../../prj/dvbc/%PRJ_CODE%/customer/%PRJ_CUSTOMER%/Daphne_Demo_dvbca/binary %PRJ_NAME%/release/
@cp -R ../../../../../../prj/dvbc/%PRJ_CODE%/customer/%PRJ_CUSTOMER%/jazz_ota_lzma.img %PRJ_NAME%/release/binary/ota_binary/
@REM -->

@dos2unix %PRJ_NAME%/post_build.sh

@REM <-- Load project
@REM STEP 1: Import project
codeblocks.exe  %PRJ_NAME%\%PRJ_NAME%.cbp

@REM <--  Cleanup environment
@CALL toolchain.bat clean
@REM -->