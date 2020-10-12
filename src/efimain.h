/*
  UefiAccessSmBios - Simple demo of Accessing SMBIOS

  Copyright 2020, Zero Tang, The 0xAA55 Forum. All rights reserved.

  This program is distributed in the hope that it will be useful, but
  without any warranty (no matter implied warranty or merchantability
  or fitness for a particular purpose, etc.).

  File Location: /src/efimain.h
*/

#include <Uefi.h>
#include <Guid/SmBios.h>
#include <Guid/GlobalVariable.h>
#include <Library/UefiLib.h>
#include <Protocol/DevicePathFromText.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DevicePathUtilities.h>
#include <Protocol/HiiFont.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/SimpleFileSystem.h>

#if defined(MDE_CPU_X64)
#define __stosp		__stosq
#elif defined(MDE_CPU_IA32)
#define __stosp		__stosd
#endif

EFI_STATUS EFIAPI UefiBootServicesTableLibConstructor(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS EFIAPI UefiRuntimeServicesTableLibConstructor(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS EFIAPI UefiLibConstructor(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS EFIAPI DevicePathLibConstructor(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable);

EFI_SIMPLE_TEXT_INPUT_PROTOCOL *StdIn=NULL;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdOut=NULL;

EFI_GUID gEfiSimpleFileSystemProtocolGuid=EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID gEfiGraphicsOutputProtocolGuid=EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GUID gEfiUgaDrawProtocolGuid=EFI_UGA_DRAW_PROTOCOL_GUID;
EFI_GUID gEfiHiiFontProtocolGuid=EFI_HII_FONT_PROTOCOL_GUID;
EFI_GUID gEfiSimpleTextOutProtocolGuid=EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathProtocolGuid=EFI_DEVICE_PATH_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathUtilitiesProtocolGuid=EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathToTextProtocolGuid=EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
EFI_GUID gEfiDevicePathFromTextProtocolGuid=EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL_GUID;
EFI_GUID gEfiGlobalVariableGuid=EFI_GLOBAL_VARIABLE;
EFI_GUID gEfiSmbiosTableGuid=SMBIOS_TABLE_GUID;

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;

SMBIOS_TABLE_ENTRY_POINT *SmBiosTable=NULL;

CHAR8* MemTypeList[0x30]=
{
	"Reserved",		// Type=0x00
	"Other",		// Type=0x01
	"Unknown",		// Type=0x02
	"DRAM",			// Type=0x03
	"EDRAM",		// Type=0x04
	"VRAM",			// Type=0x05
	"SRAM",			// Type=0x06
	"RAM",			// Type=0x07
	"ROM",			// Type=0x08
	"FLASH",		// Type=0x09
	"EEPROM",		// Type=0x0A
	"FEPROM",		// Type=0x0B
	"EPROM",		// Type=0x0C
	"CDRAM",		// Type=0x0D
	"3DRAM",		// Type=0x0E
	"SDRAM",		// Type=0x0F
	"SGRAM",		// Type=0x10
	"RDRAM",		// Type=0x11
	"DDR",			// Type=0x12
	"DDR2",			// Type=0x13
	"DDR2 FB-DIMM",	// Type=0x14
	"Reserved",		// Type=0x15
	"Reserved",		// Type=0x16
	"Reserved",		// Type=0x17
	"DDR3",			// Type=0x18
	"FBD2",			// Type=0x19
	"DDR4",			// Type=0x1A
	"LPDDR",		// Type=0x1B
	"LPDDR2",		// Type=0x1C
	"LPDDR3",		// Type=0x1D
	"LPDDR4",		// Type=0x1E
	"Logical NVD",	// Type=0x1F
	"HBM",			// Type=0x20
	"HBM2",			// Type=0x21
	"DDR5",			// Type=0x22
	"LPDDR5",		// Type=0x23
	"Reserved",		// Type=0x24
	"Reserved",		// Type=0x25
	"Reserved",		// Type=0x26
	"Reserved",		// Type=0x27
	"Reserved",		// Type=0x28
	"Reserved",		// Type=0x29
	"Reserved",		// Type=0x2A
	"Reserved",		// Type=0x2B
	"Reserved",		// Type=0x2C
	"Reserved",		// Type=0x2D
	"Reserved",		// Type=0x2E
	"Reserved"		// Type=0x2F
};

CHAR8* MemTechList[0x10]=
{
	"Reserved",		// Type=0x0
	"Other",		// Type=0x1
	"Unknown",		// Type=0x2
	"DRAM",			// Type=0x3
	"NVDIMM-N",		// Type=0x4
	"NVDIMM-F",		// Type=0x5
	"NVDIMM-P",		// Type=0x6
	"Intel Optane",	// Type=0x7
	"Reserved",		// Type=0x8
	"Reserved",		// Type=0x9
	"Reserved",		// Type=0xA
	"Reserved",		// Type=0xB
	"Reserved",		// Type=0xC
	"Reserved",		// Type=0xD
	"Reserved",		// Type=0xE
	"Reserved"		// Type=0xF
};

CHAR8* MemFormList[0x18]=
{
	"Reserved",		// Type=0x00
	"Other",		// Type=0x01
	"Unknown",		// Type=0x02
	"SIMM",			// Type=0x03
	"SIP",			// Type=0x04
	"Chip",			// Type=0x05
	"DIP",			// Type=0x06
	"ZIP",			// Type=0x07
	"Proprietary",	// Type=0x08
	"DIMM",			// Type=0x09
	"TSOP",			// Type=0x0A
	"Row of Chips",	// Type=0x0B
	"RIMM",		// Type=0x0C
	"SODIMM",		// Type=0x0D
	"SRIMM",		// Type=0x0E
	"FB-DIMM",		// Type=0x0F
	"Die",			// Type=0x10
	"Reserved",		// Type=0x11
	"Reserved",		// Type=0x12
	"Reserved",		// Type=0x13
	"Reserved",		// Type=0x14
	"Reserved",		// Type=0x15
	"Reserved",		// Type=0x16
	"Reserved"		// Type=0x17
};

CHAR16* MemTypeDetailList[0x10]=
{
	L" Reserved",			// Bit 0
	L" Other",			// Bit 1
	L" Unknown",			// Bit 2
	L" Fast-Paged",		// Bit 3
	L" Static Column",	// Bit 4
	L" Pseudo-Static",	// Bit 5
	L" RAMBUS",			// Bit 6
	L" Synchronous",		// Bit 7
	L" CMOS",				// Bit 8
	L" EDO",				// Bit 9
	L" Window DRAM",		// Bit 10
	L" Cache DRAM",		// Bit 11
	L" Non-Volatile",		// Bit 12
	L" Registered",		// Bit 13
	L" Unbuffered",		// Bit 14
	L" LRDIMM"			// Bit 15
};