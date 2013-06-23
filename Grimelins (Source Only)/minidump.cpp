/*! ====================================================================================================================
  @file minidump.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    Minidump writer for crash logs.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "minidump.h"

#include <Dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")


namespace Grimelins
{
    

    //////////////////////////////////////////////////////////////////////////
    // Autos

    MiniDumpWriter::MiniDumpWriter( void )
    {
      // Get module name and extract executable name
      char FilePathBUffer[1024];
      GetModuleFileNameA( NULL, FilePathBUffer, 1024 );
      std::string modname = strrchr( FilePathBUffer, '\\' ) + 1;
      size_t pos = modname.find_first_of( '.' );
      modname.erase( pos );

      // Initialize basic exception info block (to prevent additional actions when SEH occurs)
      SecureZeroMemory( &mExComBlock, sizeof( ExceptionInfoBlock ) );
      mExComBlock.FileName = generateDumpName( modname );
      mExComBlock.ExInf = NULL;
      mExComBlock.Proc = GetCurrentProcess();
      mExComBlock.ProcId = GetCurrentProcessId();
      mExComBlock.ThreadId = GetCurrentThreadId();
      mExComBlock.SEHOccured = false;
      mExComBlock.SchedExit = false;

      // Create idle dump thread
      mThread = CreateThread( NULL, 0, WriteExceptionThread, (void*)&mExComBlock, CREATE_SUSPENDED, NULL );
      assert( mThread != 0 && "Thread needs to start for MiniDumpWriter" );
    }

    MiniDumpWriter::~MiniDumpWriter( void )
    {
      // Signal thread to terminate and wait for regular termination
      mExComBlock.SchedExit = true;
      ResumeThread( mThread );
      WaitForSingleObject( mThread, INFINITE );
      CloseHandle( mThread );
      CloseHandle( mExComBlock.Proc );
    }

    //////////////////////////////////////////////////////////////////////////
    // Publics

    void MiniDumpWriter::DumpException( EXCEPTION_POINTERS* watchData )
    {
      mExComBlock.ExInf = watchData;
      mExComBlock.SEHOccured = true;

      ResumeThread(mThread);
      WaitForSingleObject(mThread, INFINITE);
    }

    DWORD WINAPI MiniDumpWriter::WriteExceptionThread(void* pExInf)
    {
      ExceptionInfoBlock* data = ( ExceptionInfoBlock* )( pExInf );

      while ( 1 )
      {

        if ( data->SchedExit )
        {
          return 0;
        }
        else if ( data->SEHOccured )
        {

          MessageBoxA( NULL, "The application terminated in an undefined manner. A MiniDump will be written.", 
            NULL, MB_OK | MB_ICONERROR );

          HANDLE hFile = CreateFileA( data->FileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
            NULL );

          if (!hFile || !data->ExInf)
          {
            MessageBoxA( NULL, "Unable to write minidump information to disk.", NULL, MB_OK | MB_ICONERROR );
            return -1;
          }

          MINIDUMP_EXCEPTION_INFORMATION exInf;
          SecureZeroMemory( &exInf, sizeof( MINIDUMP_EXCEPTION_INFORMATION ) );

          exInf.ThreadId = data->ThreadId;
          exInf.ExceptionPointers = data->ExInf;
          exInf.ClientPointers = TRUE;

          MiniDumpWriteDump( data->Proc, data->ProcId, hFile, MiniDumpNormal, &exInf, NULL, NULL );
          CloseHandle( hFile );
          return 0;

        }
        else
        {

          Sleep (0);

        }


      }

    }


    std::string MiniDumpWriter::generateDumpName( std::string moduleName ) const
    {
      SYSTEMTIME Time;
      SecureZeroMemory(&Time, sizeof( SYSTEMTIME ) );
      GetSystemTime( &Time );

      std::stringstream str;
      str << moduleName << "__" << Time.wYear << "_" << Time.wMonth << "_" << Time.wDay << "__";
      str << Time.wHour << "_" << Time.wMinute << "_" << Time.wSecond << ".dmp";

      return str.str();
    }


} // namespace Virtus


