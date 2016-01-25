@REM <-- Set all the variables

@SET OBJ_PLATFORM=jazz
@SET WORKSPACE_NAME=core_canna.workspace

@CALL toolchain.bat %OBJ_PLATFORM%
@dos2unix .\linux_link_core_canna.sh
@CALL toolchain.bat clean

@REM <-- Create shell & Run background
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->

@REM <-- Load project
codeblocks.exe  %WORKSPACE_NAME%
@REM -->
