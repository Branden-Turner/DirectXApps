/*! ====================================================================================================================
  @file rawstore.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Jan.28.2012

  @brief 
    Stores raw data of multiple types in one contiguous array. Ideal for networked messages.
    The container is deliberately not templated, it must be under tight control what types can be
    added to the container to ensure that all data is in the array (imagine adding a std::list that points
    somewhere else in memory).

    This container is not intended to store large amounts of data. Use it for sending a pay load over some kind
    of boundary to decouple systems / processes.

    Lookup complexity O(N)
    Insertion complexity O(C)
    Remove: N/A

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration segment.

namespace Grimelins
{
  
  SCRIPT_HeaderDefine(RawStore);

  class RawStore
  {
    friend class iterator;
    friend class const_iterator;

  public:
    // Type identifiers placed before data blocks. Deliberately picked signature 0x7F7F7F00 - 0x7F7F7FXX
    // for network boundary translation (some pattern).
    // All types are in range (dat_nos, dat_noe)
    enum DatType
    {
      dat_nos = 0x7F7F7F00, // Begin of data range

      dat_u1,
      dat_u8,
      dat_s8,
      dat_u16,
      dat_s16,
      dat_f16,
      dat_u32,
      dat_s32,
      dat_f32,
      dat_u64,
      dat_s64,
      dat_f64,
      dat_str, // Special case, size of str is stored right before data block containing string
      dat_vec2,
      dat_vec3,
      dat_vec4,
      dat_Quaternion,
      dat_ScriptVec2,
      dat_ScriptVec3,
      dat_ScriptVec4,

      dat_noe, // End of data range
      dat_err = 0x7FFFFFFF // guarantee sizeof(DatType) == sizeof(s32)
    };

    enum Error
    {
      E_UNKNOWN_ID
    };

    struct data
    {
      DatType Type;
      ch const* Id;
      void* Data;
    };

    struct constdata
    {
      DatType Type;
      ch const* Id;
      void const* Data;
    };

    class iterator
    {
      friend class RawStore;

    public:
      iterator(void);
      iterator& operator++(void);
      iterator operator++(s32);
      u1 operator==(iterator const& rhs) const;
      u1 operator!=(iterator const& rhs) const;
      u1 operator<(iterator const& rhs) const;
      u1 operator>(iterator const& rhs) const;
      u1 operator<=(iterator const& rhs) const;
      u1 operator>=(iterator const& rhs) const;

      data operator*(void) const;
      data operator->(void) const;

    private:
      iterator(ch* data);
      iterator& operator=(ch* data);

    private:
      ch* mCurrent;
    };

    class const_iterator
    {
      friend class RawStore;

    public:
      const_iterator(void);
      const_iterator& operator++(void);
      const_iterator operator++(s32);
      u1 operator==(const_iterator const& rhs) const;
      u1 operator!=(const_iterator const& rhs) const;
      u1 operator<(const_iterator const& rhs) const;
      u1 operator>(const_iterator const& rhs) const;
      u1 operator<=(const_iterator const& rhs) const;
      u1 operator>=(const_iterator const& rhs) const;

      constdata operator*(void) const;
      constdata operator->(void) const;
      
    private:
      const_iterator(ch* data);
      const_iterator& operator=(ch* data);

    private:
      ch* mCurrent;
    };

  public: // Autos.
    RawStore(void);
    ~RawStore(void);

    RawStore(RawStore const& rhs);
    RawStore& operator=(RawStore const& rhs);

  public: // Iteration.
    iterator begin(void);
    const_iterator begin(void) const;
    iterator end(void);
    const_iterator end(void) const;

  public: // Data Modification



    //define three function for each type.
    //1.  bool   AddXxx(CText id, Yyy dat);     //where Xxx is the typename with leading capital, and Yyy is the true type name
    //1.  Yyy    GetXxx(CText id);              //where Xxx is the typename with leading capital, and Yyy is the true type name
    //1.  bool   HasXxx(CText id);              //where Xxx is the typename with leading capital, and Yyy is the true type name
#define Prototype_Add_Get_Has(TName, FName)        \
        u1 Add##FName(Text id, TName const& dat); \
        TName Get##FName(Text id) const;          \
        u1 Has##FName(Text id) const
    Prototype_Add_Get_Has(u1, U1);
    Prototype_Add_Get_Has(u8, U8);
    Prototype_Add_Get_Has(s8, S8);
    Prototype_Add_Get_Has(u16, U16);
    Prototype_Add_Get_Has(s16, S16);
    Prototype_Add_Get_Has(u32, U32);
    Prototype_Add_Get_Has(s32, S32);
    Prototype_Add_Get_Has(f32, F32);
    Prototype_Add_Get_Has(f64, F64);
    Prototype_Add_Get_Has(Text, String);
    Prototype_Add_Get_Has(vec2, Vec2);
    Prototype_Add_Get_Has(vec3, Vec3);
    Prototype_Add_Get_Has(vec4, Vec4);
    Prototype_Add_Get_Has(Quaternion, Quat);
    Prototype_Add_Get_Has(ScriptVec2, ScriptVec2);
    Prototype_Add_Get_Has(ScriptVec3, ScriptVec3);
    Prototype_Add_Get_Has(ScriptVec4, ScriptVec4);
#undef Prototype_Add_Get_Has

    u1 HasData(void) const;

  private: // Helpers.
    u1 isFree(ch const* id) const;
    u1 doesFit(size_t size) const;

    u1 insert(ch const* id, ch* data, DatType datType);
    u1 retrieve(ch const* id, ch* outData, DatType datType) const;
    ch* find(ch const* id) const;

    size_t memInUse(void);
    void grow(size_t newsize);
    void deepcopy(RawStore const& rhs);

    static ch* next(ch* current);
    static size_t getDatSize(DatType const& type);
    static data getDataPackage(ch* dat);
    static constdata getConstDataPackage(ch* dat);



  private: // Data.
    ch* mData;
    ch* mEnd;
    size_t mCapacity;
  };

} // namespace Grimelins


/* TEST CASE 1

RawStore test;


u1 first = false;
u8 second = 'S';
s8 third = -1;
u16 fourth = 4444;
s16 fifth = -5555;
u32 sixth = 66666666;
s32 sevents = -77777777;
f32 eigth = 888.888f;
u64 ninth = 9999999999999999999;
s64 tenth = -01000000000000000000;
f64 eleventh = 111111.11111111;
std::string twelfth = "TWELVE TWELVE TWELVE";
vec2 thirteent = vec2(13.13f, 13.13f);
vec3 fourteenth = vec3(14.14f, 14.14f, 14.14f);
vec4 fifteenth = vec4(15.15f, 15.15f, 15.15f, 15.15f);

test.add("seventh", sevents);
test.add("fourth", fourth);
test.add("eigth", eigth);
test.add("ninth", ninth);
test.add("fifteenth", fifteenth);
test.add("sixth", sixth);
test.add("fourteenth", fourteenth);
test.add("tenth", tenth);
test.add("fifth", fifth);
test.add("thirteenth", thirteent);
test.add("first", first);
test.add("third", third);
test.add("eleventh", eleventh);
test.add("second", second);

u1 resfirst;
u8 ressecond;
s8 resthird;
u16 resfourth;
s16 resfifth;
u32 ressixth;
s32 resseventh;
f32 reseigth;
u64 resninth;
s64 restenth;
f64 reseleventh;
std::string restwelfth;
vec2 resthirteenth;
vec3 resfourteenth;
vec4 resfifteenth;

u1 res;

res = test.get("first", &resfirst);
res = test.get("eigth", &reseigth);
res = test.get("fourteenth", &resfourteenth);
res = test.get("third", &resthird);
res = test.get("sixth", &ressixth);
res = test.get("seventh", &resseventh);
res = test.get("eleventh", &reseleventh);
res = test.get("thirteenth", &resthirteenth);
res = test.get("twelfth", &restwelfth);
res = test.get("tenth", &restenth);
res = test.get("fifteenth", &resfifteenth);
res = test.get("fourth", &resfourth);
res = test.get("second", &ressecond);
res = test.get("fifth", &resfifth);
res = test.get("ninth", &resninth);

*/

