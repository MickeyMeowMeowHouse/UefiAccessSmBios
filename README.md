# UefiAccessSmBios
 Simple demo of accessing System Management BIOS in UEFI environment.

## Introduction
This project is a simple demo of accessing SMBIOS in UEFI environment.

## Requirement
To run this project, a working computer with UEFI firmware is required.

## Build
If it is your first time to build this project, you should run `build_prep.bat` to prepare for compilation. <br>
The required compiler is Win64 LLVM. Download it from GitHub: https://github.com/llvm/llvm-project/releases <br>
The required library is TianoCore EDK II. Download it from GitHub: https://github.com/tianocore/edk2/releases <br>
Extract the TianoCore EDK II source code to `C:\UefiDKII` directory. <br>
In addition, you are required to pre-compile the EDK II library. The compilation is unofficial. Use the script from [EDK II Library](https://github.com/MickeyMeowMeowHouse/EDK-II-Library) to build EDK II library. For details of pre-compilation, consult that repository to proceed.

## Test
Setup a USB flash stick with GUID Partition Table (GPT). Construct a partition and format it into FAT32 file system. <br>
Copy the compiled `bootx64.efi` to `\EFI\BOOT\bootx64.efi` in the flash stick. <br>
Enter your firmware settings. Set the device option prior to the operating system. Disable Secure Boot unless you can sign the executable.

## License
This repository is under MIT license.