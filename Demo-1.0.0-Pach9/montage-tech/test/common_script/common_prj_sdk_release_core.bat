@REM<-- ****Before use this release bat for sdk products, you need change some broga settings for creating .d files:********************
@REM<-- ****STEP1:load the project                                                                                                                             ********************
@REM<-- ****STEP2:right click poject name and choose "build option->compiler setting->other options"***************************
@REM<-- ****STEP3:add the follwing setting commands:-MMD -MP -MF"$object.d" -MT"$object.d         ***************************
@REM<-- ****STEP4:save                                                                                                                                 ***************************
@REM<-- ***************PLEASE DON'T CHANGE*********************************************************************
@REM<-- ***************Set some variables***************************************************************************
@REM<-- ***************Set some variables**************************************************************************************************************
@REM STEP 1: Set the common environment
@SET ROOT_PATH=%cd%
@setlocal EnableDelayedExpansion

@REM STEP 2: Set the project environment
shift
@SET OBJ_PLATFORM=%0
@SET LIB_PATH=%1
@SET LIB_NAME=%2
@SET LIB_CBP_NAME=%3
@SET WORKSPACE_PATH_LIB=%ROOT_PATH%\%LIB_PATH%

@SET PRJ_PATH=%4
@SET PRJ_NAME=%5
@SET PROJECT_SRC_NAME=%6
@SET PRJ_CONFIG=Release
@SET WORKSPACE_PATH=%PRJ_PATH%\%PRJ_NAME%
@SET MAP_PATH=%7
@SET PRJ_COPY_TXT=%8
@SET PROJECT_PRJ_PATH=%9

if "%1"=="" (
SET PROJECT_SRC_NAME=%PRJ_NAME%
SET LIB_PATH=config_prj\core\core_anil
SET LIB_CBP_NAME=core_anil.workspace
)
if "%2"=="" (
SET PROJECT_SRC_NAME=%PRJ_NAME%
SET LIB_PATH=config_prj\core\core_anil
SET LIB_CBP_NAME=core_anil.workspace
)

@REM STEP 2: Set the core environment
@SET PROJECT_RELEASE=%WORKSPACE_PATH%\%PRJ_CONFIG%
@SET PROJECT_OBJ=%WORKSPACE_PATH%\obj\%PRJ_CONFIG%
@SET PROJECT_PRJ_PATH_ODM=prj\odm\%PROJECT_SRC_NAME%
@if exist %WORKSPACE_PATH%\%PRJ_CONFIG% rd /Q /S %WORKSPACE_PATH%\%PRJ_CONFIG%
@mkdir %WORKSPACE_PATH%\%PRJ_CONFIG%
@REM -->

@REM<-- ****************Clean the environment***********************************************************************************************************

@REM -->

@REM<-- ****************Compile core mt and core ext*******************************************************************************************************************
@setlocal EnableDelayedExpansion
@REM STEP 1: Set the toolchain
CALL toolchain.bat %OBJ_PLATFORM%
@REM <-- Copy lib_drv.a first
@REM STEP 1: Get changeset
hg parent | gawk -F: '/changeset:/ {print $2}' |sed -e 's/ //g'>temp.txt
for /f "tokens=*" %%i in (temp.txt) do (
echo f|xcopy /y /i /s %LIB_PATH%\lib%LIB_NAME%_drv.a %PRJ_NAME%_%%i\%LIB_PATH%\lib%LIB_NAME%_drv.a
)

@REM STEP 4: Import & compile project
@if %OBJ_PLATFORM%==jazz dos2unix %LIB_PATH%/linux_link_%LIB_NAME%.sh
@if exist %WORKSPACE_PATH_LIB%\%LIB_CBP_NAME% codeblocks.exe --no-splash-screen --rebuild %WORKSPACE_PATH_LIB%\%LIB_CBP_NAME% --target=%PRJ_CONFIG%
@REM -->


@REM<-- ****************Compile project(Brolga)******************************************************************************************************************
@REM STEP 1: Set the toolchain

CALL toolchain.bat %OBJ_PLATFORM%

@if %OBJ_PLATFORM%==jazz dos2unix %WORKSPACE_PATH%/post_build.sh

@if exist %ROOT_PATH%\%PROJECT_PRJ_PATH%\binary cp -R %ROOT_PATH%\%PROJECT_PRJ_PATH%\binary %WORKSPACE_PATH%\%PRJ_CONFIG%\

@REM STEP 2: Import & compile project
@if exist %WORKSPACE_PATH%\%PRJ_NAME%.cbp codeblocks.exe  --no-splash-screen  --rebuild %WORKSPACE_PATH%\%PRJ_NAME%.cbp --target=%PRJ_CONFIG%

@REM STEP 3:Clean the toolchain
CALL toolchain.bat clean

@REM -->

@REM<-- ***************Set the toolchain to use some commands********************************************************************************************
CALL toolchain.bat %OBJ_PLATFORM%
@REM -->

@REM<-- ****************Get the .d file of core***********************************************************************************************************
for %%i in (.\%LIB_PATH%) do (
for %%j in (.d) do (
dir /S /B %%i\*%%j>>core_d_file.txt
)
)
@REM -->

@REM<-- ****************Get the .d file of project********************************************************************************************************
for %%i in (.\%PROJECT_OBJ%) do (
for %%j in (.d) do (
dir /S /B %%i\*%%j>>project_d_file.txt
)
)
@REM -->
@REM<-- ****************Get the .o file of project********************************************************************************************************
@REM STEP 1: Delete the line used libgcc.a, libc.a, libm.a from out.map
sed '/libgcc\.a/d' %MAP_PATH% > out_1.map
type out_1.map>out.map
sed '/libc\.a/d' out.map>out_1.map
type out_1.map>out.map
sed '/libm\.a/d' out.map>out_1.map
type out_1.map>out.map
gawk '/Linker script and memory map/,/debug_aranges/' out.map>out_1.map
type out_1.map>out.map

@REM STEP 2: Delete other items in .o line
sed 's/\(.*\)\W\(.*\.o$\)/\2/g' out.map>out_1.map
type out_1.map>out.map
sed 's/\(.*\)\W\(.*\.o\)\W\(.*\)/\2/g' out.map>out_1.map
type out_1.map>out.map

@REM STEP 3: Delete the lines without .o item
unix2dos out.map
findstr "\.o$" out.map>out_1.map
type out_1.map>out.map

@REM STEP 4: Delete the same lines
  @REM STEP 4.1: Define the items already exist
del out_1.map
for /f "tokens=*" %%x in (out.map) do (
if not defined %%x set %%x=A & echo %%x>>out_1.map
)
type out_1.map>out.map
del out_1.map
  @REM STEP 4.2: Undefine the defined items
for /f "tokens=*" %%x in (out.map) do (
if defined %%x set %%x=
)
@REM -->
@REM<-- ****************Get the .o file relateve path of core*******************************************************************************************

@for %%x in (.\%LIB_PATH%) do (
@for %%y in (.o) do (
dir /S /B %%x\*%%y>core_src_files.txt
)
)
@sed 's/\\\/\\\\\\\\\\\/g' core_src_files.txt > temp_core_src_files1.txt
@sed 's/\(.*\)%PRJ_CONFIG%\\\\\\\\\\\\\(.*\)/\2/g' temp_core_src_files1.txt>core_src_files.txt
@sed 's/\\\\\\\\\\\/\\\/g'  core_src_files.txt > temp_core_src_files1.txt
@type temp_core_src_files1.txt > core_src_files.txt
@REM STEP 2: Filter .o files
unix2dos out.map
unix2dos core_src_files.txt
for /f "tokens=*" %%x in (out.map) do (
findstr "%%x$" core_src_files.txt >> temp_core_src_files.txt
)
@sed 's/\.o/\.c/g' temp_core_src_files.txt >  core_src_files.txt
@sed 's/\.o/\.c/g' temp_core_src_files1.txt >  core_src_files_org.txt
@del temp_core_src_files*.txt

@REM -->

@REM<-- ****************Get the .o file relateve path of project********************************************************************************************************
@for %%x in (.\%WORKSPACE_PATH%) do (
@for %%y in (.o) do (
dir /S /B %%x\*%%y>project_src_files.txt
)
)
@sed 's/\\\/\\\\\\\\\\\/g' project_src_files.txt > temp_project_src_files.txt
@sed 's/\(.*\)%PRJ_CONFIG%\\\\\\\\\\\\\(.*\)/\2/g' temp_project_src_files.txt>project_src_files.txt
@sed 's/\.o/\.c/g' project_src_files.txt > temp_project_src_files.txt
@sed 's/\\\\\\\\\\\/\\\/g'  temp_project_src_files.txt>project_src_files.txt
@del temp_project_src_files.txt
@REM -->

@REM<-- ****************get the relative path .d file**************************************************************************************************************
@REM STEP 1: Merge the .d file to one file
type core_d_file.txt>all_d_file.txt
type project_d_file.txt>>all_d_file.txt
del core_d_file.txt
del project_d_file.txt

@REM STEP 2: delete the root path of the file path
set cd=%cd:\=\\\%
sed 's/%cd%\\\//g' all_d_file.txt>temp_d_file.txt
set cd=%cd:\\\=\%
type temp_d_file.txt>all_d_file.txt
del temp_d_file.txt
@REM -->

@REM STEP 1: Change the items in out.map to .d for find .d files
@sed 's/\.o/\.o\.d/g' out.map>out_o_d_file.map

@REM STEP 2: Filter .d files
@unix2dos out_o_d_file.map
@unix2dos all_d_file.txt
@for /f "tokens=*" %%x in (out_o_d_file.map) do (
findstr "%%x$" all_d_file.txt>>temp_d_file.txt
)
@type temp_d_file.txt>all_d_file.txt
@del temp_d_file.txt
@del out_o_d_file.map
@REM -->

@REM<-- ****************Get the .h file  from .d file****************************************************************************************************
@REM STEP 1: Read the the path of .h from .d files
for /f "tokens=*" %%i in (all_d_file.txt) do (
type %%i>>all_h_files.txt
)
del all_d_file.txt

@REM STEP 2: Filter the .h file
unix2dos all_h_files.txt
findstr "\.h:$" all_h_files.txt>tmp_all_h_files.txt
type tmp_all_h_files.txt>all_h_files.txt

@REM STEP 3: Filter the .h file
sed 's/:$//g' all_h_files.txt>tmp_all_h_files.txt
type tmp_all_h_files.txt>all_h_files.txt

@REM STEP 4: Get the relative path of the .h files
sed 's/\//\\\/g' all_h_files.txt>tmp_all_h_files.txt
type tmp_all_h_files.txt>all_h_files.txt
set cd=%cd:\=\\\%
sed 's/%cd%\\\//g' all_h_files.txt>tmp_all_h_files.txt
set cd=%cd:\\\=\%
type tmp_all_h_files.txt>all_h_files.txt

@REM STEP 5: Delete the same items
del tmp_all_h_files.txt
for /f "tokens=*" %%x in (all_h_files.txt) do (
if not defined %%x set %%x=A & echo %%x>>tmp_all_h_files.txt
)
type tmp_all_h_files.txt>all_h_files.txt

@REM STEP 6: Clean the environment
for /f "tokens=*" %%x in (all_h_files.txt) do (
if defined %%x set %%x=
)
del tmp_all_h_files.txt

@REM STEP 7: // Change // to /
@sed 's/\\\/\\\\\\\\\\\/g'  all_h_files.txt>1.txt
@sed 's/\\\\\\\\\\\\\\\\\\\\\\\/\\\\\\\\\\\/g'  1.txt>1.1.txt
@sed 's/\\\\\\\\\\\/\\\/g'  1.1.txt>all_h_files.txt
@del 1.txt
@del 1.1.txt 

@REM -->
@REM<-- ****************Get the list of .c files****************************************************************************************************
@sed 's/\\\/\\\\\\\/g' core_src_files.txt > src_need.txt
@findstr /B /v /g:src_need.txt core_src_files_org.txt > src_ex_temp.txt
@sed 's/\\\/\\\\\\\/g' %PRJ_PATH%\%PRJ_COPY_TXT% > src_need.txt
@findstr /B /v /g:src_need.txt src_ex_temp.txt > src_ex.txt
@type core_src_files.txt>c_file_list.txt
@type project_src_files.txt>>c_file_list.txt

@del core_src_files.txt
@del project_src_files.txt

@del src_need.txt
@del core_src_files.txt
@del project_src_files.txt

@REM<-- ****************Create new core project not contains unneeded .c*****************************************************************************
@dir /S /B %LIB_PATH%\*.cbp > cbp_files.txt
@sed 's/.*montage-tech\\\//g' cbp_files.txt > cbp_files1.txt
@for /f "tokens=1" %%i in (cbp_files1.txt) do ( 
type %%i > %%i_out.txt
)
@sed 's/\\\/\\\\\\\\\\\/g' src_ex.txt > src_ex3.txt
@for /f "tokens=1" %%i in (src_ex3.txt) do (
for /f "tokens=1" %%j in (cbp_files1.txt) do ( 
sed '/\Unit filename=.*\.\.\\\%%i/,/\/\Unit/d' %%j_out.txt > %%j_temp.txt
type %%j_temp.txt > %%j_out.txt)
)
@del src_ex*.txt

@REM<-- ****************Clean the environment**************************************************************************************************
@del /s /q %WORKSPACE_PATH_LIB%\*.layout
@del /s /q %WORKSPACE_PATH_LIB%\*.depend
@if exist %WORKSPACE_PATH_LIB%\*.a del %WORKSPACE_PATH_LIB%\*.a
@if exist %WORKSPACE_PATH%\bin rd /Q /S %WORKSPACE_PATH%\bin
@if exist %WORKSPACE_PATH%\.objs rd /Q /S %WORKSPACE_PATH%\.objs
@if exist %WORKSPACE_PATH%\.s rd /Q /S %WORKSPACE_PATH%\.s
@if exist %WORKSPACE_PATH%\obj rd /Q /S %WORKSPACE_PATH%\obj
@if exist %PROJECT_RELEASE% rd /Q /S %PROJECT_RELEASE%
@if exist %WORKSPACE_PATH%\*.layout del %WORKSPACE_PATH%\*.layout
@if exist %WORKSPACE_PATH%\*.depend del %WORKSPACE_PATH%\*.depend
@if exist %WORKSPACE_PATH%\*.cbTemp del %WORKSPACE_PATH%\*.cbTemp
@if exist %WORKSPACE_PATH%\out.map del %WORKSPACE_PATH%\out.map
@if exist %WORKSPACE_PATH_LIB% (
@pushd %WORKSPACE_PATH_LIB%
@set fn=bin
@set m0=0
:loop0
@set /a m0+=1
@if not "!fn:~%m0%,1!" equ "" goto loop0
@for /f %%k in ('dir /s/b/ad') do (
@set aa=%%k
@if "!aa:~-%m0%!" equ "%fn%" rd /s/q %%k 2>nul)
@set fn=obj
@set m1=0
:loop1
@set /a m1+=1
@if not "!fn:~%m1%,1!" equ "" goto loop1 
@for /f %%k in ('dir /s/b/ad') do (
@set aa=%%k
@if "!aa:~-%m1%!" equ "%fn%" rd /s/q %%k 2>nul)
@popd
)

@REM -->


@REM<-- ****************Copy the file list**************************************************************************************************

@REM STEP 2: New the folder to store products
for /f "tokens=*" %%i in (temp.txt) do (
mkdir %RELEASE_PATH%\%PRJ_NAME%\%PRJ_NAME%_%%i\
)
@REM STEP 3: Copy the c and h files
for /f "tokens=*" %%i in (temp.txt) do (
for /f "tokens=*" %%x in (c_file_list.txt) do (
echo f|xcopy /y /i %%x %PRJ_NAME%_%%i\%%x
)

for /f "tokens=*" %%x in (all_h_files.txt) do (
echo f|xcopy /y /i %%x %PRJ_NAME%_%%i\%%x
)

@REM STEP 3.1: delete the private files
del %PRJ_NAME%_%%i\src\drv\avsync\wizard\avsync_wizards.c
del %PRJ_NAME%_%%i\src\drv\audio\wizard\audio_wizards.c
del %PRJ_NAME%_%%i\src\drv\video\wizard\video_wizards.c

@REM STEP 4: Copy the readme.txt
copy config_prj\%PRJ_NAME%\README.txt %PRJ_NAME%_%%i\README.txt


@REM STEP 5<-- Copy the config files project related
echo d|xcopy /y /i /s %LIB_PATH% %PRJ_NAME%_%%i\%LIB_PATH%

for /f "tokens=1" %%j in (cbp_files1.txt) do (
copy /Y %%j_out.txt %PRJ_NAME%_%%i\%%j
del %%j_out.txt
del %%j_temp.txt
del %PRJ_NAME%_%%i\%%j_out.txt
del %PRJ_NAME%_%%i\%%j_temp.txt)

del %PRJ_NAME%_%%i\%LIB_PATH%\release_%LIB_NAME%.bat

echo d|xcopy /y /i /s %PRJ_PATH% %PRJ_NAME%_%%i\%PRJ_PATH%
del %PRJ_NAME%_%%i\%PRJ_PATH%\release_%PRJ_NAME%.bat
del %PRJ_NAME%_%%i\%PRJ_PATH%\README.txt

 @REM STEP 6: delete the .bak and org file
for %%x in (.\%PRJ_NAME%_%%i) do (
for %%y in (.bak) do (
dir /S /B %%x\*%%y>unusedfiles.txt
)
)
for %%x in (.\%PRJ_NAME%_%%i) do (
for %%y in (.orig) do (
dir /S /B %%x\*%%y>>unusedfiles.txt
)
)
for %%x in (.\%PRJ_NAME%_%%i) do (
for %%y in (.cbTemp) do (
dir /S /B %%x\*%%y>>unusedfiles.txt
)
)
for /f "tokens=*" %%x in (unusedfiles.txt) do (
del %%x
)
del unusedfiles.txt


@REM STEP 7: Copy the anchor tool and link_mips.ld
echo f|xcopy /y /i link_mips.ld %PRJ_NAME%_%%i\link_mips.ld
echo f|xcopy /y /i /s config_prj\other\anchor\bin\anchor.exe %PRJ_NAME%_%%i\config_prj\other\anchor\bin\anchor.exe
)
del all_h_files.txt
del c_file_list.txt
del core_src_files_org.txt
del cbp_files*.txt
del out.map

@REM -->

@REM<-- ***************Clean the environment********************************************************************************************
CALL toolchain.bat clean
@REM -->