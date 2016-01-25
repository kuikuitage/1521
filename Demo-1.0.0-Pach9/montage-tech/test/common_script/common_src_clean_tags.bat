@SET OBJ_PATH=%1

@if exist %OBJ_PATH% (
call toolchain.bat mips1
dir /S /B %OBJ_PATH%\*.c > all_src_files.txt
dir /S /B %OBJ_PATH%\*.h >> all_src_files.txt
dir /S /B %OBJ_PATH%\*.ld >> all_src_files.txt
for /f "tokens=1" %%i in (all_src_files.txt) do (
type %%i > %%i.txt
sed '/Copyright.*Montage Tech/d' %%i.txt > %%i
del %%i.txt)
del all_src_files.txt
dir /S /B %OBJ_PATH%\*.a > all_lib_files.txt
for /f "tokens=1" %%i in (all_lib_files.txt) do (
test\lib_tag_clear\lib_tag_clear.exe -p %%i.a %%i
type %%i.a  > %%i
del %%i.a)
del all_lib_files.txt
)
