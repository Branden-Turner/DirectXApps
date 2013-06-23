/*! ====================================================================================================================
  @file minidump.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    Minidump writer for crash logs.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_MINIDUMP_H
#define VIRTUS_MINIDUMP_H


namespace Grimelins
{
    
    class MiniDumpWriter
    {
    public: // Autos.
      MiniDumpWriter( void );
      ~MiniDumpWriter( void );

    public: // Interface.
      void DumpException( EXCEPTION_POINTERS* watchData );
      static DWORD WINAPI WriteExceptionThread(void* data); 

    private: // Helpers.
      std::string generateDumpName( std::string moduleName ) const;

    private: // Decl.
      struct ExceptionInfoBlock
      {
        std::string FileName;
        EXCEPTION_POINTERS* ExInf;
        DWORD ProcId;
        HANDLE Proc;
        DWORD ThreadId;
        bool SEHOccured;
        bool SchedExit;
      };

    private: // Data.
      ExceptionInfoBlock mExComBlock;
      HANDLE mThread;

    };

} // namespace Virtus

#endif // Include Guard