/* net.h -- SceNet exports/imports
 *
 * Copyright (C) 2019 TheFloW
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __NET_H__
#define __NET_H__

// Exports

// SceNet
#define SceNet_sceNetRecv 0x285d
#define SceNet_sceNetDumpRead 0x2901
#define SceNet_sceNetResolverStartAton 0x2525
#define SceNet_sceNetSetsockopt 0x2879
#define SceNet_sceNetGetMacAddress 0x22a9
#define SceNet_sceNetNtohs 0x23ed
#define SceNet_sceNetConnect 0x2801
#define SceNet_sceNetBind 0x27e9
#define SceNet_sceNetHtonll 0x23c9
#define SceNet_sceNetGetSockIdInfo 0x274d
#define SceNet_sceNetAccept 0x27f9
#define SceNet_sceNetGetsockname 0x2899
#define SceNet_sceNetResolverStartNtoa 0x2485
#define SceNet_SceNet_1F0080B3 0x1729
#define SceNet_sceNetGetpeername 0x28a1
#define SceNet_sceNetSocketClose 0x28b1
#define SceNet_sceNetShowNetstat 0x20c1
#define SceNet_sceNetResolverDestroy 0x2649
#define SceNet_sceNetResolverAbort 0x26c5
#define SceNet_sceNetDumpAbort 0x2911
#define SceNet_sceNetEpollWait 0x28c9
#define SceNet_sceNetHtonl 0x23d1
#define SceNet_sceNetEpollControl 0x28c1
#define SceNet_SceNet_522D2118 0x1611
#define SceNet_sceNetSendto 0x2819
#define SceNet_sceNetEmulationSet 0x2139
#define SceNet_sceNetShowIfconfig 0x1fd1
#define SceNet_sceNetShutdown 0x2871
#define SceNet_sceNetShowRoute 0x2049
#define SceNet_sceNetResolverCreate 0x23f9
#define SceNet_sceNetEpollDestroy 0x28e9
#define SceNet_sceNetListen 0x27f1
#define SceNet_sceNetDumpDestroy 0x2909
#define SceNet_sceNetIcmConnect 0x2809
#define SceNet_sceNetEtherNtostr 0x2275
#define SceNet_sceNetResolverGetError 0x25c9
#define SceNet_sceNetSocketAbort 0x27d9
#define SceNet_sceNetEpollWaitCB 0x28d9
#define SceNet_sceNetEpollAbort 0x28f1
#define SceNet_sceNetInetNtop 0x23a9
#define SceNet_sceNetSendmsg 0x2811
#define SceNet_sceNetHtons 0x23d5
#define SceNet_sceNetGetStatisticsInfo 0x113d
#define SceNet_sceNetEmulationGet 0x21bd
#define SceNet_sceNetDumpCreate 0x28f9
#define SceNet_sceNetGetSockInfo 0x2745
#define SceNet_sceNetRecvfrom 0x2849
#define SceNet_sceNetGetsockopt 0x2889
#define SceNet_sceNetNtohl 0x23e9
#define SceNet_sceNetInetPton 0x23b1
#define SceNet_sceNetSetDnsInfo 0xdb1
#define SceNet_SceNet_D886F498 0x2329
#define SceNet_sceNetRecvmsg 0x2841
#define SceNet_sceNetErrnoLoc 0x2945
#define SceNet_sceNetSend 0x282d
#define SceNet_sceNetTerm 0xa75
#define SceNet_sceNetInit 0xa5d
#define SceNet_sceNetEtherStrton 0x2241
#define SceNet_sceNetSocket 0x27e1
#define SceNet_sceNetEpollCreate 0x28b9
#define SceNet_sceNetNtohll 0x23e1
#define SceNet_sceNetClearDnsCache 0xeb1

// SceNetInternal
#define SceNet_SceNetInternal_01B090F1 0x123d
#define SceNet_SceNetInternal_04E6136D 0x1add
#define SceNet_SceNetInternal_0DFC23D6 0x14f9
#define SceNet_sceNetDescriptorCtlInternal 0x2921
#define SceNet_SceNetInternal_104E4820 0x1e45
#define SceNet_SceNetInternal_16222ED6 0xe31
#define SceNet_SceNetInternal_225C4801 0x1dc1
#define SceNet_SceNetInternal_235DE96C 0xbfd
#define SceNet_SceNetInternal_26EFF46D 0x1355
#define SceNet_SceNetInternal_2FEC8B74 0x1c05
#define SceNet_sceNetIoctlInternal 0x28a9
#define SceNet_SceNetInternal_3B5F94B7 0x1e09
#define SceNet_SceNetInternal_4054D2D8 0x1e79
#define SceNet_SceNetInternal_46E28336 0x1729
#define SceNet_SceNetInternal_4EF96CD8 0x1e3d
#define SceNet_SceNetInternal_4F5E0E8A 0x13e1
#define SceNet_SceNetInternal_5B0E370C 0xfd5
#define SceNet_sceNetEmulationGetInternal 0x21bd
#define SceNet_SceNetInternal_65AFF9BE 0xa69
#define SceNet_SceNetInternal_689B9D7D 0x1c21
#define SceNet_SceNetInternal_694F8996 0x1d1d
#define SceNet_SceNetInternal_7493FB0C 0x194d
#define SceNet_SceNetInternal_7F63379E 0xd31
#define SceNet_SceNetInternal_7FD92AE7 0xbc9
#define SceNet_SceNetInternal_8157DE3E 0xc6d
#define SceNet_SceNetInternal_853EEABE 0x1841
#define SceNet_sceNetDescriptorCloseInternal 0x2929
#define SceNet_SceNetInternal_916EA798 0x23bd
#define SceNet_sceNetIcmConnectInternal 0x2809
#define SceNet_SceNetInternal_97527BDB 0x1f05
#define SceNet_SceNetInternal_9D12D8A4 0x1f9d
#define SceNet_SceNetInternal_9DBD2F60 0x1611
#define SceNet_SceNetInternal_A2C2BC5F 0x1bf5
#define SceNet_SceNetInternal_A64CFBF5 0x1cad
#define SceNet_SceNetInternal_A6B9F5D2 0xb41
#define SceNet_SceNetInternal_A9F2277C 0x11bd
#define SceNet_sceNetControlInternal 0xcbd
#define SceNet_SceNetInternal_AAA34850 0x10bd
#define SceNet_SceNetInternal_B01ABD0A 0x1b69
#define SceNet_SceNetInternal_BDA92B76 0xf3d
#define SceNet_SceNetInternal_C2FF615E 0x1f95
#define SceNet_SceNetInternal_CF49E9C4 0x1041
#define SceNet_SceNetInternal_DE8DD55E 0x18cd
#define SceNet_SceNetInternal_DEF2357F 0x169d
#define SceNet_sceNetEmulationSetInternal 0x2139
#define SceNet_SceNetInternal_E03F6A77 0x12c9
#define SceNet_sceNetDescriptorCreateInternal 0x2919
#define SceNet_SceNetInternal_E62A2FAA 0x1bfd
#define SceNet_SceNetInternal_EAC33599 0x19cd
#define SceNet_SceNetInternal_EDAA3453 0x1a5d
#define SceNet_SceNetInternal_EE1985D9 0xac5
#define SceNet_SceNetInternal_EE94BA15 0xc31
#define SceNet_SceNetInternal_EE94BF52 0x146d
#define SceNet_SceNetInternal_EF421A65 0x1585
#define SceNet_SceNetInternal_F3917021 0xbc1
#define SceNet_SceNetInternal_F6291476 0x1f81
#define SceNet_SceNetInternal_F9E3A8B7 0x17b5
#define SceNet_sceNetSysctlInternal 0x2931

// Imports

// SceNetPsForSyscalls
#define SceNet_sceNetSyscallSetsockopt 0x9f30
#define SceNet_sceNetSyscallRecvfrom 0x9f40
#define SceNet_sceNetSyscallConnect 0x9f50
#define SceNet_sceNetSyscallClose 0x9f60
#define SceNet_sceNetSyscallDumpClose 0x9f70
#define SceNet_sceNetSyscallBind 0x9f80
#define SceNet_sceNetSyscallIoctl 0x9f90
#define SceNet_sceNetSyscallRecvmsg 0x9fa0
#define SceNet_sceNetSyscallSendto 0x9fb0
#define SceNet_sceNetSyscallDumpRead 0x9fc0
#define SceNet_sceNetSyscallSysctl 0x9fd0
#define SceNet_sceNetSyscallDumpCreate 0x9fe0
#define SceNet_sceNetSyscallAccept 0x9ff0
#define SceNet_sceNetSyscallDumpAbort 0xa000
#define SceNet_sceNetSyscallGetsockname 0xa010
#define SceNet_sceNetSyscallEpollClose 0xa020
#define SceNet_sceNetSyscallSocket 0xa030
#define SceNet_sceNetSyscallDescriptorClose 0xa040
#define SceNet_sceNetSyscallGetIfList 0xa050
#define SceNet_sceNetSyscallIcmConnect 0xa060
#define SceNet_sceNetSyscallEpollAbort 0xa070
#define SceNet_sceNetSyscallShutdown 0xa080
#define SceNet_sceNetSyscallDescriptorCtl 0xa090
#define SceNet_sceNetSyscallEpollCreate 0xa0a0
#define SceNet_sceNetSyscallSendmsg 0xa0b0
#define SceNet_sceNetSyscallListen 0xa0c0
#define SceNet_sceNetSyscallDescriptorCreate 0xa0d0
#define SceNet_sceNetSyscallGetsockopt 0xa0e0
#define SceNet_sceNetSyscallGetpeername 0xa0f0
#define SceNet_sceNetSyscallEpollCtl 0xa100
#define SceNet_sceNetSyscallControl 0xa110
#define SceNet_sceNetSyscallGetSockinfo 0xa120
#define SceNet_sceNetSyscallSocketAbort 0xa130
#define SceNet_sceNetSyscallEpollWait 0xa140

// SceLibKernel
#define SceNet_sceKernelGetThreadId 0xa150
#define SceNet_sceClibMemcpy_safe 0xa160
#define SceNet_sceClibStrchr 0xa170
#define SceNet_sceClibMemset 0xa180
#define SceNet_sceClibStrncmp 0xa190
#define SceNet_sceKernelWaitEventFlag 0xa1a0
#define SceNet_sceKernelCreateEventFlag 0xa1b0
#define SceNet_sceClibSnprintf 0xa1c0
#define SceNet_sceClibMemcmp 0xa1d0
#define SceNet_sceClibStrncat 0xa1e0
#define SceNet_sceClibStrnlen 0xa1f0
#define SceNet_sceKernelGetProcessTimeWide 0xa200
#define SceNet_sceKernelGetTLSAddr 0xa210
#define SceNet_sceClibStrncpy 0xa220
#define SceNet_sceKernelCreateThread 0xa230
#define SceNet_sceKernelWaitThreadEnd 0xa240
#define SceNet_sceKernelStartThread 0xa250
#define SceNet_sceClibPrintf 0xa260

// SceThreadmgr
#define SceNet_sceKernelDeleteThread 0xa270
#define SceNet_sceKernelDelayThread 0xa280
#define SceNet_sceKernelDeleteEventFlag 0xa290
#define SceNet_sceKernelDelayThreadCB 0xa2a0
#define SceNet_sceKernelCreateCallback 0xa2b0
#define SceNet_sceKernelDeleteCallback 0xa2c0
#define SceNet_sceKernelSetEventFlag 0xa2d0

// SceThreadmgrCoredumpTime
#define SceNet_sceKernelExitThread 0xa2e0

#endif
