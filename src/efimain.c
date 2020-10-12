/*
  UefiAccessSmBios - Simple demo of Accessing SMBIOS

  Copyright 2020, Zero Tang, The 0xAA55 Forum. All rights reserved.

  This program is distributed in the hope that it will be useful, but
  without any warranty (no matter implied warranty or merchantability
  or fitness for a particular purpose, etc.).

  File Location: /src/efimain.c
*/

#include <Uefi.h>
#include <IndustryStandard/SmBios.h>
#include <Library/UefiLib.h>
#include <Register/Intel/Cpuid.h>
#include <intrin.h>
#include "efimain.h"

INTN EfiCompareGuid(EFI_GUID *Guid1,EFI_GUID *Guid2)
{
	if(Guid1->Data1>Guid2->Data1)
		return 1;
	else if(Guid1->Data1<Guid2->Data1)
		return -1;
	if(Guid1->Data2>Guid2->Data2)
		return 1;
	else if(Guid1->Data2<Guid2->Data2)
		return -1;
	if(Guid1->Data3>Guid2->Data3)
		return 1;
	else if(Guid1->Data3<Guid2->Data3)
		return -1;
	for(UINT8 i=0;i<8;i++)
	{
		if(Guid1->Data4[i]>Guid2->Data4[i])
			return 1;
		else if(Guid1->Data4[i]<Guid2->Data4[i])
			return -1;
	}
	return 0;
}

void PrintProcessorBrand()
{
	CHAR8 Brand[0x31];
	AsmCpuid(CPUID_BRAND_STRING1,(UINT32*)&Brand[0x00],(UINT32*)&Brand[0x04],(UINT32*)&Brand[0x08],(UINT32*)&Brand[0x0C]);
	AsmCpuid(CPUID_BRAND_STRING2,(UINT32*)&Brand[0x10],(UINT32*)&Brand[0x14],(UINT32*)&Brand[0x18],(UINT32*)&Brand[0x1C]);
	AsmCpuid(CPUID_BRAND_STRING3,(UINT32*)&Brand[0x20],(UINT32*)&Brand[0x24],(UINT32*)&Brand[0x28],(UINT32*)&Brand[0x2C]);
	Brand[0x30]='\0';
	Print(L"Processor Brand: %a\n",Brand);
}

void BlockUntilKeyStroke(IN CHAR16 Unicode)
{
	EFI_INPUT_KEY InKey;
	do
	{
		UINTN fi=0;
		gBS->WaitForEvent(1,&StdIn->WaitForKey,&fi);
		StdIn->ReadKeyStroke(StdIn,&InKey);
	}while(InKey.UnicodeChar!=Unicode);
}

void SetConsoleModeToMaximumRows()
{
	UINTN MaxHgt=0,OptIndex;
	for(UINTN i=0;i<StdOut->Mode->MaxMode;i++)
	{
		UINTN Col,Row;
		EFI_STATUS st=StdOut->QueryMode(StdOut,i,&Col,&Row);
		if(st==EFI_SUCCESS)
		{
			if(Row>MaxHgt)
			{
				OptIndex=i;
				MaxHgt=Row;
			}
		}
	}
	StdOut->SetMode(StdOut,OptIndex);
	StdOut->ClearScreen(StdOut);
}

EFI_STATUS EfiLocateSmBiosTable()
{
	for(UINTN i=0;i<gST->NumberOfTableEntries;i++)
	{
		if(EfiCompareGuid(&gST->ConfigurationTable[i].VendorGuid,&gEfiSmbiosTableGuid)==0)
		{
			SmBiosTable=(SMBIOS_TABLE_ENTRY_POINT*)gST->ConfigurationTable[i].VendorTable;
			return EFI_SUCCESS;
		}
	}
	return EFI_NOT_FOUND;
}

UINTN GetSmBiosEntryLength(IN SMBIOS_STRUCTURE *Entry)
{
	CHAR8* StringPool=(CHAR8*)((UINTN)Entry+Entry->Length);
	UINTN i=0;
	while(StringPool[i]!='\0' || StringPool[i+1]!='\0')i++;
	return Entry->Length+i+2;
}

EFI_STATUS EfiQueryMemoryModuleInformation(OUT UINT64 *Size)
{
	EFI_STATUS st=EFI_NOT_FOUND;
	UINTN Len=0,Index=0;
	*Size=0;
	for(UINTN CurEntry=(UINTN)SmBiosTable->TableAddress;CurEntry<(UINTN)(SmBiosTable->TableAddress+SmBiosTable->TableLength);CurEntry+=Len)
	{
		SMBIOS_STRUCTURE *Entry=(SMBIOS_STRUCTURE*)CurEntry;
		Len=GetSmBiosEntryLength(Entry);
		if(Entry->Type==SMBIOS_TYPE_MEMORY_DEVICE)
		{
			SMBIOS_TABLE_TYPE17 *MemModInfo=(SMBIOS_TABLE_TYPE17*)Entry;
			if(MemModInfo->Size)
			{
				CHAR8* Strings=(CHAR8*)((UINTN)Entry+Entry->Length);
				// 15 strings should be enough for Memory Module Information.
				CHAR8* StringPool[0x10];
				UINT8 j=1;		// Leave the zeroth as empty.
				UINTN StringLength=0;
				// Initialize the String Pool.
				__stosp((UINTN*)StringPool,(UINTN)"No Info",0x10);
				for(UINTN i=0;i<Len-Entry->Length;i+=StringLength)
				{
					StringLength=AsciiStrnLenS(&Strings[i],Len-Entry->Length-i)+1;
					if(Strings[i]=='\0')break;
					StringPool[j++]=&Strings[i];
				}
				// Memory Module Information in Strings
				CHAR8* DeviceLocator=StringPool[MemModInfo->DeviceLocator];
				CHAR8* BankLocator=StringPool[MemModInfo->BankLocator];
				CHAR8* Manufacturer=StringPool[MemModInfo->Manufacturer];
				CHAR8* SerialNumber=StringPool[MemModInfo->SerialNumber];
				CHAR8* AssetTag=StringPool[MemModInfo->AssetTag];
				CHAR8* PartNumber=StringPool[MemModInfo->PartNumber];
				CHAR8* MemoryType=MemTypeList[MemModInfo->MemoryType];
				CHAR8* FormFactor=MemFormList[MemModInfo->FormFactor];
				Print(L"---------------- Memory Module Information for %a ----------------\n",BankLocator);
				Print(L"Device Slot: %a | Manufacturer: %a | Serial Number: %a | Asset Tag: %a\n",DeviceLocator,Manufacturer,SerialNumber,AssetTag);
				Print(L"Part Number: %a | Size=",PartNumber);
				if(_bittest(&MemModInfo->Size,15))		// Unit is KiB
				{
					*Size+=(MemModInfo->Size&0x7FFF);
					Print(L"%d KiB",MemModInfo->Size&0x7FFF);
				}
				else			// Unit is MiB
				{
					// This RAM module is greater than or equal to 32 GiB.
					if(MemModInfo->Size==0x7FFF)
					{
						UINT64 MemSize=MemModInfo->ExtendedSize&0x7FFFFFFF;
						*Size+=(MemSize<<10);
						Print(L"%d MiB",MemModInfo->ExtendedSize&0x7FFFFFFF);
					}
					else
					{
						*Size+=(MemModInfo->Size<<10);
						Print(L"%d MiB",MemModInfo->Size);
					}
				}
				Print(L" | Type: %a | Form Factor: %a\n",MemoryType,FormFactor);
				StdOut->OutputString(StdOut,L"Memory Type Detail:");
				for(UINT8 i=0;i<16;i++)
					if(_bittest(&MemModInfo->TypeDetail,i))
						StdOut->OutputString(StdOut,MemTypeDetailList[i]);
				Print(L"\nMemory Data Width: %d | Memory Total Width: %d | Transfer Rate: %d MT/s\n",MemModInfo->DataWidth,MemModInfo->TotalWidth,MemModInfo->Speed);
				st=EFI_SUCCESS;
			}
		}
		if(++Index==SmBiosTable->NumberOfSmbiosStructures)break;
	}
	if(st==EFI_SUCCESS)StdOut->OutputString(StdOut,L"-------------------- Memory Module Information Listing Ended --------------------\r\n");
	return st;
}

EFI_STATUS EFIAPI EfiInitialize(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable)
{
	UefiBootServicesTableLibConstructor(ImageHandle,SystemTable);
	UefiRuntimeServicesTableLibConstructor(ImageHandle,SystemTable);
	UefiLibConstructor(ImageHandle,SystemTable);
	DevicePathLibConstructor(ImageHandle,SystemTable);
	StdIn=SystemTable->ConIn;
	StdOut=SystemTable->ConOut;
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI EfiMain(IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS st=EfiInitialize(ImageHandle,SystemTable);
	if(st==EFI_SUCCESS)
	{
		UINT16 RevHi=(UINT16)(SystemTable->Hdr.Revision>>16);
		UINT16 RevLo=(UINT16)(SystemTable->Hdr.Revision&0xFFFF);
		SetConsoleModeToMaximumRows();
		StdOut->OutputString(StdOut,L"UefiAccessSmBios Demo - Simple Demo of Accessing SMBIOS in UEFI\r\n");
		StdOut->OutputString(StdOut,L"Powered by zero.tangptr@gmail.com, Copyright Zero Tang, 0xAA55 Forum, 2020, All Rights Reserved.\r\n");
		PrintProcessorBrand();
		Print(L"UEFI Firmware Vendor: %s Revision: %d.%d\n",SystemTable->FirmwareVendor,RevHi,RevLo);
		st=EfiLocateSmBiosTable();
		// Check Validity of SMBIOS Location.
		if(st==EFI_SUCCESS)
		{
			if(AsciiStrnCmp((CHAR8*)SmBiosTable->AnchorString,"_SM_",4))
			{
				StdOut->OutputString(StdOut,L"The located SMBIOS Table is invalid!\r\n");
				st=EFI_DEVICE_ERROR;
			}
			else
			{
				UINT64 MemorySize=0;
				Print(L"SMBIOS Version: %d.%d\n",SmBiosTable->MajorVersion,SmBiosTable->MinorVersion);
				st=EfiQueryMemoryModuleInformation(&MemorySize);
				if(st==EFI_SUCCESS)Print(L"Total Memory Size: %d KiB\n",MemorySize);
			}
		}
		StdOut->OutputString(StdOut,L"Press Enter key to continue...\r\n");
		BlockUntilKeyStroke(L'\r');
	}
	return st;
}