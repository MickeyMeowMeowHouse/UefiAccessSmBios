@echo off

title Project: UefiAccessSmBios, Cleanup, UEFI (AMD64 Architecture)
echo Project: UefiAccessSmBios
echo Platform: Unified Extensible Firmware Interface
echo Preset: Cleanup
echo Powered by zero.tangptr@gmail.com

del ..\bin\compchk_uefix64 /q /s
del ..\bin\compfre_uefix64 /q /s

echo Completed!
pause.