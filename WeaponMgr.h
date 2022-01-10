#pragma once
//
//#define	Bypass28_3					0x3EFB3C         // UPDATE: E8 ?? ?? ?? ?? 4C ?? ?? 48 ?? ?? 0F 84 ?? ?? ?? ?? F3
//#define WeaponIndex					0x61C8             // UPDATE...
//#define _WP_Amount						5119                // TOTAL NUMBER OF WEAPONS
//#define WeaponMrg						0x1CB57C0       // UPDATE: 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 08 F3 0F 11 B1
//
//struct WEAPONs_
//{
//	char Data[WeaponIndex];
//} *BackupWeapon[_WP_Amount];
//
//WEAPONs_* SkyhtCF(int index)
//{
//	return BackupWeapon[index];
//}
//
//enum WeaponType
//{
//	Pistol = 0,
//	Shotgun = 1,
//	SMG = 2,
//	Rifle = 3,
//	Sniper = 4,
//	MachineGun = 5,
//	Grenades = 6,
//	Knife = 7,
//	C4 = 9,
//	RAPPEL = 10,
//	SPECIALKICK = 12,
//	DualKnife = 65543
//};
//
//bool Function_WeaponBypass()
//{
//	static bool isOne;
//
//	DWORD64 pWeaponMgr = *(DWORD64*)(SkyhtHack.CShell + WeaponMrg);
//
//	if (!isOne)
//	{
//		for (int i = 0; i < _WP_Amount; i++)
//		{
//			DWORD64 dwWeapon = *(DWORD64*)(pWeaponMgr + i * 8);
//			if (dwWeapon)
//			{
//				BackupWeapon[i] = new WEAPONs_;
//				memcpy(reinterpret_cast<void*>(BackupWeapon[i]), reinterpret_cast<void*>(dwWeapon), sizeof(WEAPONs_));
//				isOne = true;
//			}
//		}
//	}
//	else if (isOne)
//	{
//		DWORD64	address = SkyhtHack.CShell + Bypass28_3;
//		DWORD oldProtect;
//		VirtualProtect((void*)address, 0x7, PAGE_EXECUTE_READWRITE, &oldProtect);
//		*(DWORD*)(address + 1) = (DWORD)((DWORD64)SkyhtCF - address - 5);
//		VirtualProtect((void*)address, 0x7, oldProtect, &oldProtect);
//
//		for (int i = 0; i < _WP_Amount; i++)
//		{
//			DWORD64 dwWeapon = *(DWORD*)(pWeaponMgr + i * 8);
//			if (dwWeapon)
//			{
//				if ((*(BYTE*)(dwWeapon + 0x2) != WeaponType::Rifle) || (*(BYTE*)(dwWeapon + 0x2) != WeaponType::Sniper) || (*(BYTE*)(dwWeapon + 0x2) != WeaponType::Shotgun) || (*(BYTE*)(dwWeapon + 0x2) != WeaponType::Pistol) || (*(BYTE*)(dwWeapon + 0x2) != WeaponType::SMG))
//				{
//					*(float*)(dwWeapon + ChangeWeaponAnimRatio) = 99999999999999;
//					*(float*)(dwWeapon + ReloadAnimRatio) = 9999999999;
//				}
//			}
//		}
//
//	}
//
//	return false;
//}