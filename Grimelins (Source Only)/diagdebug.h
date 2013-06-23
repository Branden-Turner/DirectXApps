/*! ====================================================================================================================
  @file debug.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.15.2011

  @brief 
    Debugging Diagnostics (Windows / Visual Stdio specific)

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_APPDEBUG_H
#define VIRTUS_APPDEBUG_H

namespace Grimelins
{
  namespace Application
  {
    // Debugging functions
    bool RelayErrorHandler(char const* expression, char const* file, unsigned line, char const* message = 0, ...);
    void DebugPrintHandler(const char* msg, ... );
    void StartDebugDiag(void);
  } // namespace Application
} // Virtus

// Define Debug Diagnostic tools
#if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE

#undef assert

#define BreakIf(exp) if (exp) { Break(); }

#define assert(exp, ...) do { \
  if (!(exp)) \
  if (Grimelins::Application::RelayErrorHandler(#exp, __FILE__, __LINE__, __VA_ARGS__)) \
  Break(); \
} while(0)

#define Assert assert

#define ErrorIf(exp, ...) do { \
  if(exp && Grimelins::Application::RelayErrorHandler(#exp, __FILE__, __LINE__, __VA_ARGS__)) \
    Break(); \
} while(0)

#define Error(...) do { \
  if (Grimelins::Application::RelayErrorHandler(0, __FILE__, __LINE__, __VA_ARGS__)) \
    Break(); \
} while(0)

#define printDebugT(msg, ...) do { \
  Grimelins::Application::DebugPrintHandler("%f: "##msg, gCore->Time.Get(), __VA_ARGS__); \
} while(0)

#define printDebug(msg, ...) do { \
  Grimelins::Application::DebugPrintHandler(msg, __VA_ARGS__); \
} while(0)

#define printAllT(msg, ...) do { \
  Grimelins::Application::DebugPrintHandler("%f: "##msg, gSystemTime->Get(), __VA_ARGS__); \
  printf("%f: "##msg, gSystemTime->Get(), __VA_ARGS__); \
} while(0)

#define printAll(msg, ...) do { \
  Grimelins::Application::DebugPrintHandler(msg, __VA_ARGS__); \
  printf(msg, __VA_ARGS__); \
} while(0)

#define SetUnhandledExceptionHandler(pHandler) SetUnhandledExceptionFilter(pHandler)

#define printFileLine() printDebug("%s(%u) : ", __FILE__, __LINE__)
#define vinline

#else

// Bind all debug diagnostic checks to NOPS
#undef assert
#define BreakIf(...) ((void)(0))
#define assert(...) ((void)(0))
#define Assert assert
#define ErrorIf(exp, ...) if(exp) ((void)(0))
#define Error(...) ((void)(0))

// Disable all printing
#define printDebug(...) ((void)(0))
#define printDebugT(...) ((void)(0))

#define printAllT(msg, ...) do { \
  printf("%f: "##msg, gSystemTime->Get(), __VA_ARGS__); \
} while(0)

#define printAll(msg, ...)  do { \
  printf(msg, __VA_ARGS__); \
} while(0)

#define printFileLine(...) ((void)(0))

// Force inline
#define vinline __forceinline

#endif

#define Stringize( L )			#L
#define MakeString( M, L )		M(L)
#define $Line					\
  MakeString( Stringize, __LINE__ )
#define Reminder				\
  __FILE__ "(" $Line ") : Reminder: "

#endif // Include Guard
