/*! ====================================================================================================================
  @file appdiagnostics.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jun.14.2011

  @brief 
    Diagnostics sub system of application layer. Runs diagnostics on machine / system to determine compatibility.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <direct.h>               // _getdrive()
#include <Mmsystem.h>             // timeGetTime()
#pragma comment(lib, "Winmm.lib") // timeGetTime()

#include "appdiagnostics.h"

#define SLEEPTIME 0

namespace Grimelins
{
  namespace Application
  {

    namespace // static
    {
      s32 sMilliseconds;
      s64 sTicks;
      s32 sMilliseconds0;
      s64 sTicks0;
    }

    s32 GetCPUSpeed( s32 Sampletime )
    {
      static s32 CPU_SPEED = 0;

      if ( CPU_SPEED != 0 )
      {
        //This will assure that the 0.5 second delay happens only once
        return CPU_SPEED;	
      }

      if ( StartTimingCPU() )
        return 0;

      //This will lock the application for 1 second
      do
      {
        UpdateCPUTime();
        Sleep(SLEEPTIME);
      } while ( sMilliseconds < Sampletime );

      CPU_SPEED = CalcCPUSpeed();
      return CPU_SPEED;
    }

    void CheckInstance( ch const* title )
    {
      // Precent other instance from accessing this function at the same time
      HANDLE handle = CreateMutexA(NULL, TRUE, title); 

      // Does anyone else think 'ERROR_SUCCESS' is a bit of a dichotomy? 
      if ( ::GetLastError() != ERROR_SUCCESS ) 
      { 
        HWND hWnd = FindWindowA( title, NULL ); 

        if (hWnd) 
        {
          // An instance is already running
          ShowWindow( hWnd, SW_SHOWNORMAL ); 
          SetFocus( hWnd ); 
          SetForegroundWindow( hWnd ); 
          SetActiveWindow( hWnd ); 


          throw Exception(E_INSTANCE_NOT_ONLY); // force exit
        } 
      } 
    }

    void CheckHardDisk( u64 spaceRequired )
    {
      // Check for enough free disk space on the current disk. 
      s32 drive = _getdrive(); 
      struct _diskfree_t diskfree; 
      _getdiskfree( drive, &diskfree ); 
      u64 neededClusters = spaceRequired / ( diskfree.sectors_per_cluster * diskfree.bytes_per_sector ); 

      if ( diskfree.avail_clusters < neededClusters )
        throw Exception(E_HD_NOT_ENOUGH_SPACE);
    }

    void CheckMemory( u64 physRamNeeded, u64 vRamNeeded )
    {
      MEMORYSTATUS status; 
      ZeroMemory(&status, sizeof(MEMORYSTATUS));

      GlobalMemoryStatus( &status ); 

      if ( status.dwAvailPhys < physRamNeeded ) 
        throw Exception(E_MEM_PHYS_NOT_ENOUGH); // Ram physically not present

      // Check for enough free memory. 

      if ( status.dwAvailVirtual < vRamNeeded ) 
        throw Exception(E_MEM_VIRT_NOT_ENOUGH); // Ram virtually not available

      s8* buff = (s8*) Utilities::Memory::__BYPASSMALLOC((size_t)vRamNeeded * sizeof(s8));

      if ( buff ) 
        Utilities::Memory::__BYPASSFREE( buff ); 
      else 
        throw Exception(E_MEM_CONT_NOT_NEOUGH); // Not enough contiguous memory available
    }

    s32 StartTimingCPU( void )
    {
      //
      // detect ability to get info
      //
      __asm
      {
        pushfd					      ; push extended flags
          pop		eax				    ; store eflags into eax
          mov		ebx, eax		  ; save EBX for testing later
          xor		eax, (1<<21)	; switch bit 21
          push	eax				    ; push eflags
          popfd					      ; pop them again
          pushfd					    ; push extended flags
          pop		eax				    ; store eflags into eax
          cmp		eax, ebx		  ; see if bit 21 has changed
          jz		no_cpuid		  ; make sure it's now on
      }

      // make ourselves high priority just for the time between
      // when we measure the time and the CPU ticks
      u32 dwPriorityClass = GetPriorityClass( GetCurrentProcess() );
      s32 dwThreadPriority = GetThreadPriority( GetCurrentThread() );
      SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
      SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

      // start timing
      sMilliseconds0 = (s32)timeGetTime();

      __asm
      {
          lea		ecx, sTicks0		      ; get the offset
          mov		dword ptr [ecx], 0	  ; zero the memory
          mov		dword ptr [ecx+4], 0	;
          rdtsc						            ; read time-stamp counter
          mov		[ecx], eax			      ; store the negative
          mov		[ecx+4], edx		      ; in the variable
      }

      // restore thread priority
      SetThreadPriority( GetCurrentThread(), dwThreadPriority );
      SetPriorityClass( GetCurrentProcess(), dwPriorityClass );

      return 0;

  no_cpuid:
      return -1;
    }
    
    void UpdateCPUTime( void )
    {
      // make ourselves high priority just for the time between
      // when we measure the time and the CPU ticks
      u32 dwPriorityClass = GetPriorityClass( GetCurrentProcess() );
      s32 dwThreadPriority = GetThreadPriority( GetCurrentThread() );
      SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
      SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

      // get the times
      sMilliseconds	 = -sMilliseconds0;
      sTicks			     = -sTicks0;

      sMilliseconds	+= (s32)timeGetTime();

      __asm
      {
        lea		ecx, sTicks		; get the offset
          rdtsc						      ; read time-stamp counter
          add		[ecx], eax			; add the tick count
          adc		[ecx+4], edx		;
      }

      // restore thread priority
      SetThreadPriority( GetCurrentThread(), dwThreadPriority );
      SetPriorityClass( GetCurrentProcess(), dwPriorityClass );

      return;
    }

    s32 CalcCPUSpeed( void )
    {
      // get the actual cpu speed in MHz, and
      // then find the one in the CPU speed list
      // that is closest
      const struct tagCPUSPEEDS
      {
        f32	fSpeed;
        s32		iSpeed;
      } cpu_speeds[] = {
        // valid CPU speeds that are not integrally divisible by
        // 16.67 MHz
        {  60.00f,	 60 },
        {  75.00f,	 75 },
        {  90.00f,	 90 },
        { 120.00f,	120 },
        { 180.00f,	180 },
      };

      // find the closest one
      f32	fSpeed = ( (f32)sTicks ) / ( (f32)sMilliseconds * 1000.0f );
      s32	iSpeed = cpu_speeds[0].iSpeed;
      f32	fDiff  = fabs(fSpeed - cpu_speeds[0].fSpeed);

      for ( s32 i = 1 ; i < sizeof(cpu_speeds) / sizeof(cpu_speeds[0]); i++ )
      {
        f32 fTmpDiff = fabs( fSpeed - cpu_speeds[i].fSpeed );

        if ( fTmpDiff < fDiff )
        {
          iSpeed = cpu_speeds[i].iSpeed;
          fDiff = fTmpDiff;
        }
      }

      // now, calculate the nearest multiple of fIncr
      // speed
      // now, if the closest one is not within one incr, calculate
      // the nearest multiple of fIncr speed and see if that's
      // closer
      const f32 fIncr = 16.66666666666666666666667f;
      const s32 iIncr = 4267; // fIncr << 8

      // get the number of fIncr quantums the speed is
      s32	iQuantums	= (s32)( ( fSpeed / fIncr ) + 0.5f );
      f32	fQuantumSpeed	= (f32)iQuantums * fIncr;
      f32	fTmpDiff	= fabs( fQuantumSpeed - fSpeed );

      if ( fTmpDiff < fDiff )
      {
        iSpeed = ( iQuantums * iIncr ) >> 8;
        fDiff = fTmpDiff;
      }

      return iSpeed;
    }

  } // namespace Application
} // namespace Virtus