@REM <-- Set all the variables********************PLEASE CHANGE***************

@SET OBJ_PLATFORM=jazz
@SET PRJ_PATH=config_prj\dvbc\jazz\Daphne\Zhongda\Daphne_Zhongda_SY
@SET PRJ_NAME=Daphne_Zhongda_SY
@SET ROOT_PATH=%cd%
@SET PRJ_BINARY_PATH=prj\dvbc\Daphne_Nongwang\binary
@REM <-- **************************call common script*************************************

@call %ROOT_PATH%\test\common_script\common_prj_compile_check.bat %OBJ_PLATFORM% %PRJ_PATH% %PRJ_NAME% %PRJ_BINARY_PATH%