/*! ====================================================================================================================
  @file helpers.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.14.2011

  @brief 
    Helper defines and methods.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_HELPERS_H
#define VIRTUS_HELPERS_H

// Team Common Abbreviations & Helpers
#define RCAST(TYPE, DATA) reinterpret_cast<TYPE>(DATA)
#define SCAST(TYPE, DATA) static_cast<TYPE>(DATA)
#define CCAST(TYPE, DATA) const_cast<TYPE>(DATA)
#define DCAST(TYPE, DATA) dynamic_cast<TYPE>(DATA)

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#define printConsole(msg) do { \
  RawStore arg; \
  arg.AddString("message", msg); \
  gCore->Events.Post(H("con_print"), arg, MSG_PRIORITY_MEDIUM); \
} while(0)

#include "helpers.inl"

#endif // Include Guard