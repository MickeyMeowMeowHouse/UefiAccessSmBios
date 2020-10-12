@echo off
set edkpath=C:\UefiDKII
set mdepath=C:\UefiDKII\MdePkg
set libpath=C:\UefiDKII\Bin\MdePkg
set binpath=..\bin\compfre_uefix64
set objpath=..\bin\compfre_uefix64\Intermediate

title Compiling UefiAccessSmBios, Free Build, UEFI (AMD64 Architecture)
echo Project: UefiAccessSmBios
echo Platform: Unified Extensible Firmware Interface
echo Preset: Release/Free Build
echo Powered by zero.tangptr@gmail.com
clang-cl --version
lld-link --version
pause

echo ============Start Compiling============
clang-cl ..\src\efimain.c /I"%mdepath%\Include" /I"%mdepath%\Include\X64" /D"_EFI_BOOT" /Zi /W3 /WX /O2 /Fa"%objpath%\efimain.cod" /Fo"%objpath%\efimain.obj" /GS- /Gr /TC /c -Wno-int-to-pointer-cast -Wno-incompatible-pointer-types

echo ============Start Linking============
lld-link "%objpath%\efimain.obj" /NODEFAULTLIB /LIBPATH:"%libpath%\compfre_uefix64" "BaseLib.lib" "BaseDebugPrintErrorLevelLib.lib" "BaseMemoryLib.lib" "BasePrintLib.lib" "UefiLib.lib" "UefiDebugLibConOut.lib" "UefiMemoryAllocationLib.lib" "UefiDevicePathLib.Lib" "UefiBootServicesTableLib.Lib" "UefiRuntimeServicesTableLib.Lib" /OUT:"%binpath%\bootx64.efi" /OPT:REF /SUBSYSTEM:EFI_APPLICATION /ENTRY:"EfiMain" /DEBUG /PDB:"%objpath%\bootx64.pdb" /Machine:X64

echo Completed!
pause.