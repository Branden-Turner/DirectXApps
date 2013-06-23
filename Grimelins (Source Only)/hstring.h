/*! ====================================================================================================================
  @file hashstring.h

  Codename Virtus

  @author Chris Savoie <chris@chrissavoie.com> (compile time hash)
  @author Maximilian Manndorff <max@lo3.com> (hstring part)

  @date May.18.2011

  @brief 
    (Un)/ Hashes strings at compile & run Time->

    * "THE BEER-WARE LICENSE" (Revision 42):
    * <chris@chrissavoie.com> wrote portions of this file (compile time hashing). As long as you retain this notice you
    * can do whatever you want with this stuff. If we meet some day, and you think
    * this stuff is worth it, you can buy me a beer in return Chris Savoie
    ***   I so will -- Max

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_HSTRING_H
#define VIRTUS_HSTRING_H
namespace Grimelins
{

#define CONSTHASH_DEPTH 64 // Don't change above this, compiler sets limit at this cap. This means no strings > 64 chars will be compile time hashed

  static const size_t CONSTHASH_CONSTANTS[CONSTHASH_DEPTH+1] =
  {
    0x6157FFFF, 0x5387FFFF, 0x8ECBFFFF, 0xB3DBFFFF, 0x1AFDFFFF, 0xD1FDFFFF, 0x19B3FFFF, 0xE6C7FFFF, 
    0x53BDFFFF, 0xCDAFFFFF, 0xE543FFFF, 0x369DFFFF, 0x8135FFFF, 0x50E1FFFF, 0x115BFFFF, 0x07D1FFFF, 
    0x9AA1FFFF, 0x4D87FFFF, 0x442BFFFF, 0xEAA5FFFF, 0xAEDBFFFF, 0xB6A5FFFF, 0xAFE9FFFF, 0xE895FFFF, 
    0x4E05FFFF, 0xF8BFFFFF, 0xCB5DFFFF, 0x2F85FFFF, 0xF1DFFFFF, 0xD5ADFFFF, 0x438DFFFF, 0x6073FFFF, 
    0xA99FFFFF, 0x2E0BFFFF, 0xF729FFFF, 0x5D01FFFF, 0x1ACDFFFF, 0xFAD1FFFF, 0xD86BFFFF, 0xE909FFFF, 
    0xD3BDFFFF, 0xF35BFFFF, 0xD53DFFFF, 0x4DC1FFFF, 0x6897FFFF, 0x6E4DFFFF, 0x305BFFFF, 0x6F0DFFFF, 
    0x33C9FFFF, 0xC955FFFF, 0xC1EDFFFF, 0x48D5FFFF, 0x0CF5FFFF, 0x356BFFFF, 0x5F65FFFF, 0x71C1FFFF, 
    0x3F13FFFF, 0x489DFFFF, 0xEBA3FFFF, 0x340DFFFF, 0xF537FFFF, 0xD5E7FFFF, 0x6D27FFFF, 0x89D7FFFF, 
    0xA93FFFFF, 
  };

  static const size_t CONSTHASH_MULTS[CONSTHASH_DEPTH+1] =
  {
    33,  34,  35,  36,  37,  38,  39,  40, 
    41,  42,  43,  44,  45,  46,  47,  48, 
    49,  50,  51,  52,  53,  54,  55,  56, 
    57,  58,  59,  60,  61,  62,  63,  64, 
    65,  66,  67,  68,  69,  70,  71,  72, 
    73,  74,  75,  76,  77,  78,  79,  80, 
    81,  82,  83,  84,  85,  86,  87,  88, 
    89,  90,  91,  92,  93,  94,  95,  96, 
    97, 
  };

#define CONSTHASH_RECURSE_00(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[0] : CONSTHASH_MULTS[0] * CONSTHASH_RECURSE_01(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_01(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[1] : CONSTHASH_MULTS[1] * CONSTHASH_RECURSE_02(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_02(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[2] : CONSTHASH_MULTS[2] * CONSTHASH_RECURSE_03(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_03(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[3] : CONSTHASH_MULTS[3] * CONSTHASH_RECURSE_04(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_04(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[4] : CONSTHASH_MULTS[4] * CONSTHASH_RECURSE_05(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_05(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[5] : CONSTHASH_MULTS[5] * CONSTHASH_RECURSE_06(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_06(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[6] : CONSTHASH_MULTS[6] * CONSTHASH_RECURSE_07(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_07(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[7] : CONSTHASH_MULTS[7] * CONSTHASH_RECURSE_08(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_08(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[8] : CONSTHASH_MULTS[8] * CONSTHASH_RECURSE_09(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_09(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[9] : CONSTHASH_MULTS[9] * CONSTHASH_RECURSE_10(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_10(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[10] : CONSTHASH_MULTS[10] * CONSTHASH_RECURSE_11(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_11(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[11] : CONSTHASH_MULTS[11] * CONSTHASH_RECURSE_12(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_12(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[12] : CONSTHASH_MULTS[12] * CONSTHASH_RECURSE_13(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_13(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[13] : CONSTHASH_MULTS[13] * CONSTHASH_RECURSE_14(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_14(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[14] : CONSTHASH_MULTS[14] * CONSTHASH_RECURSE_15(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_15(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[15] : CONSTHASH_MULTS[15] * CONSTHASH_RECURSE_16(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_16(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[16] : CONSTHASH_MULTS[16] * CONSTHASH_RECURSE_17(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_17(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[17] : CONSTHASH_MULTS[17] * CONSTHASH_RECURSE_18(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_18(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[18] : CONSTHASH_MULTS[18] * CONSTHASH_RECURSE_19(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_19(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[19] : CONSTHASH_MULTS[19] * CONSTHASH_RECURSE_20(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_20(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[20] : CONSTHASH_MULTS[20] * CONSTHASH_RECURSE_21(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_21(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[21] : CONSTHASH_MULTS[21] * CONSTHASH_RECURSE_22(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_22(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[22] : CONSTHASH_MULTS[22] * CONSTHASH_RECURSE_23(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_23(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[23] : CONSTHASH_MULTS[23] * CONSTHASH_RECURSE_24(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_24(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[24] : CONSTHASH_MULTS[24] * CONSTHASH_RECURSE_25(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_25(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[25] : CONSTHASH_MULTS[25] * CONSTHASH_RECURSE_26(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_26(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[26] : CONSTHASH_MULTS[26] * CONSTHASH_RECURSE_27(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_27(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[27] : CONSTHASH_MULTS[27] * CONSTHASH_RECURSE_28(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_28(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[28] : CONSTHASH_MULTS[28] * CONSTHASH_RECURSE_29(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_29(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[29] : CONSTHASH_MULTS[29] * CONSTHASH_RECURSE_30(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_30(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[30] : CONSTHASH_MULTS[30] * CONSTHASH_RECURSE_31(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_31(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[31] : CONSTHASH_MULTS[31] * CONSTHASH_RECURSE_32(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_32(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[32] : CONSTHASH_MULTS[32] * CONSTHASH_RECURSE_33(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_33(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[33] : CONSTHASH_MULTS[33] * CONSTHASH_RECURSE_34(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_34(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[34] : CONSTHASH_MULTS[34] * CONSTHASH_RECURSE_35(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_35(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[35] : CONSTHASH_MULTS[35] * CONSTHASH_RECURSE_36(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_36(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[36] : CONSTHASH_MULTS[36] * CONSTHASH_RECURSE_37(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_37(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[37] : CONSTHASH_MULTS[37] * CONSTHASH_RECURSE_38(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_38(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[38] : CONSTHASH_MULTS[38] * CONSTHASH_RECURSE_39(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_39(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[39] : CONSTHASH_MULTS[39] * CONSTHASH_RECURSE_40(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_40(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[40] : CONSTHASH_MULTS[40] * CONSTHASH_RECURSE_41(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_41(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[41] : CONSTHASH_MULTS[41] * CONSTHASH_RECURSE_42(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_42(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[42] : CONSTHASH_MULTS[42] * CONSTHASH_RECURSE_43(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_43(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[43] : CONSTHASH_MULTS[43] * CONSTHASH_RECURSE_44(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_44(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[44] : CONSTHASH_MULTS[44] * CONSTHASH_RECURSE_45(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_45(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[45] : CONSTHASH_MULTS[45] * CONSTHASH_RECURSE_46(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_46(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[46] : CONSTHASH_MULTS[46] * CONSTHASH_RECURSE_47(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_47(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[47] : CONSTHASH_MULTS[47] * CONSTHASH_RECURSE_48(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_48(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[48] : CONSTHASH_MULTS[48] * CONSTHASH_RECURSE_49(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_49(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[49] : CONSTHASH_MULTS[49] * CONSTHASH_RECURSE_50(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_50(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[50] : CONSTHASH_MULTS[50] * CONSTHASH_RECURSE_51(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_51(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[51] : CONSTHASH_MULTS[51] * CONSTHASH_RECURSE_52(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_52(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[52] : CONSTHASH_MULTS[52] * CONSTHASH_RECURSE_53(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_53(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[53] : CONSTHASH_MULTS[53] * CONSTHASH_RECURSE_54(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_54(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[54] : CONSTHASH_MULTS[54] * CONSTHASH_RECURSE_55(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_55(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[55] : CONSTHASH_MULTS[55] * CONSTHASH_RECURSE_56(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_56(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[56] : CONSTHASH_MULTS[56] * CONSTHASH_RECURSE_57(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_57(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[57] : CONSTHASH_MULTS[57] * CONSTHASH_RECURSE_58(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_58(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[58] : CONSTHASH_MULTS[58] * CONSTHASH_RECURSE_59(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_59(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[59] : CONSTHASH_MULTS[59] * CONSTHASH_RECURSE_60(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_60(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[60] : CONSTHASH_MULTS[60] * CONSTHASH_RECURSE_61(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_61(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[61] : CONSTHASH_MULTS[61] * CONSTHASH_RECURSE_62(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_62(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[62] : CONSTHASH_MULTS[62] * CONSTHASH_RECURSE_63(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_63(str, value) CONSTHASH_FUNCTION((*(str+1) == 0 ? CONSTHASH_CONSTANTS[63] : CONSTHASH_MULTS[63] * CONSTHASH_RECURSE_64(str+1, *(str+1))), value)
#define CONSTHASH_RECURSE_64(str, value) CONSTHASH_CONSTANTS[64]

#define CONSTHASH_FUNCTION(next, value) ((value << 15) | value | 33) + ((11 * value * value * 257) ^ CONSTHASH_CONSTANTS[value & 0x3f] ^ (next))

#define CONSTHASH(str) CONSTHASH_RECURSE_00(str, *str)


// #if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE
#if 1
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Debug Version

#define H(str) (str)

  class hstring
  {
  public: // Autos
    hstring(void);
    hstring(const char* str);
    hstring(std::string const& st);
    hstring(hstring const& rhs);
    ~hstring(void);

  public: // Operators
    hstring& operator=(const char* str);
    hstring& operator=(hstring const& rhs);
    bool operator==(hstring const& rhs) const;
    bool operator!=(hstring const& rhs) const;
    bool operator<(hstring const& rhs) const;
    bool operator>(hstring const& rhs) const;
    bool empty(void) const;

  public: // Methods
    ch const* const str(void) const;

  private: // Data
    std::string m_String;
  };

#else
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Release Version

#define H(str) (CONSTHASH(str))

  class hstring
  {
  public: // Autos
    hstring(void);
    hstring(s32 str);
    hstring(ch const* str);
    hstring(string const& str);
    hstring(hstring const& rhs);
    ~hstring(void);

  public: // Operators
    hstring& operator=(s32 str);
    hstring& operator=(hstring const& rhs);
    bool operator==(hstring const& rhs) const;
    bool operator!=(hstring const& rhs) const;
    bool operator<(hstring const& rhs) const;
    bool operator>(hstring const& rhs) const;
    bool empty(void) const;

  public: // Methods
    ch const* const str(void) const;

  private: // Data
    s32 m_Hash;
    mutable ch m_CStr[11]; // Maximum number in u32 is 4 294 967 296 which has 10 digits
  };

#endif

} // namespace virtus

#endif // Include Guard
