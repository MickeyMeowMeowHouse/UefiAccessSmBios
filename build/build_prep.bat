@echo off

title Project: UefiAccessSmBios, Preparation, UEFI (AMD64 Architecture)
echo Project: UefiAccessSmBios
echo Platform: Unified Extensible Firmware Interface
echo Preset: Directory Build
echo Powered by zero.tangptr@gmail.com
pause.

echo Starting Compilation Preparations
mkdir ..\bin

echo Making Directories for UefiAccessSmBios Checked Build, 64-Bit UEFI
mkdir ..\bin\compchk_uefix64
mkdir ..\bin\compchk_uefix64\Intermediate

echo Making Directories for UefiAccessSmBios Free Build, 64-Bit UEFI
mkdir ..\bin\compfre_uefix64
mkdir ..\bin\compfre_uefix64\Intermediate

echo Preparation Completed!
pause.