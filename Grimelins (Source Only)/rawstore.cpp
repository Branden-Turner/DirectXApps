/*! ====================================================================================================================
  @file rawstore.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date Oct.28.2011

  @brief 
    Raw storage of multiple types in contiguous memory.

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Segment.

#include "precompiled.h"
#include "rawstore.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition Segment.

#define INITIAL_SIZE_BYTES 16

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code Segment.

namespace Grimelins
{


  SCRIPT_ClassStart(RawStore);
  SCRIPT_CTOR_Disabled();
  SCRIPT_MethodOverload(AddU1, "setB", u1, (Text, u1 const&));
  SCRIPT_MethodOverload(AddS32, "setI", u1, (Text, s32 const&));
  SCRIPT_MethodOverload(AddF32, "setF", u1, (Text, f32 const&));
  SCRIPT_MethodOverload(AddString, "setT", u1, (Text, Text const&));
  SCRIPT_MethodOverload(AddScriptVec2, "setV2", u1, (Text, ScriptVec2 const&));
  SCRIPT_MethodOverload(AddScriptVec3, "setV3", u1, (Text, ScriptVec3 const&));
  SCRIPT_MethodOverload(AddScriptVec4, "setV4", u1, (Text, ScriptVec4 const&));
  SCRIPT_MethodOverload(AddQuat, "setQ", u1, (Text, Quaternion const&));

  SCRIPT_MethodOverload(GetU1, "getB", u1, (Text) const);
  SCRIPT_MethodOverload(GetS32, "getI", s32, (Text) const);
  SCRIPT_MethodOverload(GetF32, "getF", f32, (Text) const);
  SCRIPT_MethodOverload(GetString, "getT", Text, (Text) const);
  SCRIPT_MethodOverload(GetScriptVec2, "getV2", ScriptVec2, (Text) const);
  SCRIPT_MethodOverload(GetScriptVec3, "getV3", ScriptVec3, (Text) const);
  SCRIPT_MethodOverload(GetScriptVec4, "getV4", ScriptVec4, (Text) const);
  SCRIPT_MethodOverload(GetQuat, "getQ", Quaternion, (Text) const);

  SCRIPT_MethodOverload(HasU1, "hasB", u1, (Text) const);
  SCRIPT_MethodOverload(HasS32, "hasI", u1, (Text) const);
  SCRIPT_MethodOverload(HasF32, "hasF", u1, (Text) const);
  SCRIPT_MethodOverload(HasString, "hasT", u1, (Text) const);
  SCRIPT_MethodOverload(HasScriptVec2, "hasV2", u1, (Text) const);
  SCRIPT_MethodOverload(HasScriptVec3, "hasV3", u1, (Text) const);
  SCRIPT_MethodOverload(HasScriptVec4, "hasV4", u1, (Text) const);
  SCRIPT_MethodOverload(HasQuat, "hasQ", u1, (Text) const);

  SCRIPT_MethodOverload(HasData, "hasData", u1, (void) const);

  SCRIPT_ClassEnd();
  //////////////////////////////////////////////////////////////////////////
  // iterator

  RawStore::iterator::iterator(void) : mCurrent(NULL)
  {

  }

  RawStore::iterator& RawStore::iterator::operator++(void)
  {
    if (!mCurrent)
      throw Exception("Can not increment uninitialized RawStore::iterator");

    mCurrent = RawStore::next(mCurrent);
    return *this;
  }

  RawStore::iterator RawStore::iterator::operator++(s32)
  {
    if (!mCurrent)
      throw Exception("Can not increment uninitialized RawStore::iterator");

    iterator ret(mCurrent);
    mCurrent = RawStore::next(mCurrent);
    return ret;
  }

  u1 RawStore::iterator::operator==(iterator const& rhs) const
  {
    return mCurrent == rhs.mCurrent;
  }

  u1 RawStore::iterator::operator!=(iterator const& rhs) const
  {
    return mCurrent != rhs.mCurrent;
  }

  u1 RawStore::iterator::operator<(iterator const& rhs) const
  {
    return mCurrent < rhs.mCurrent;
  }

  u1 RawStore::iterator::operator>(iterator const& rhs) const
  {
    return mCurrent > rhs.mCurrent;
  }

  u1 RawStore::iterator::operator<=(iterator const& rhs) const
  {
    return mCurrent <= rhs.mCurrent;
  }

  u1 RawStore::iterator::operator>=(iterator const& rhs) const
  {
    return mCurrent >= rhs.mCurrent;
  }

  RawStore::data RawStore::iterator::operator*(void) const
  {
    if (!mCurrent)
      throw Exception("Can not access uninitialized RawStore::iterator");

    return RawStore::getDataPackage(mCurrent);
  }

  RawStore::data RawStore::iterator::operator->(void) const
  {
    if (!mCurrent)
      throw Exception("Can not access uninitialized RawStore::iterator");

    return RawStore::getDataPackage(mCurrent);
  }

  RawStore::iterator::iterator(ch* data) : mCurrent(data)
  {

  }

  RawStore::iterator& RawStore::iterator::operator=(ch* data)
  {
    mCurrent = data;
    return *this;
  }


  //////////////////////////////////////////////////////////////////////////
  // const_iterator
  

  RawStore::const_iterator::const_iterator(void) : mCurrent(NULL)
  {

  }

  RawStore::const_iterator& RawStore::const_iterator::operator++(void)
  {
    if (!mCurrent)
      throw Exception("Can not increment uninitialized RawStore::iterator");

    mCurrent = RawStore::next(mCurrent);
    return *this;
  }

  RawStore::const_iterator RawStore::const_iterator::operator++(s32)
  {
    if (!mCurrent)
      throw Exception("Can not increment uninitialized RawStore::iterator");

    const_iterator ret(mCurrent);
    mCurrent = RawStore::next(mCurrent);
    return ret;
  }

  u1 RawStore::const_iterator::operator==(const_iterator const& rhs) const
  {
    return mCurrent == rhs.mCurrent;
  }

  u1 RawStore::const_iterator::operator!=(const_iterator const& rhs) const
  {
    return mCurrent != rhs.mCurrent;
  }

  u1 RawStore::const_iterator::operator<(const_iterator const& rhs) const
  {
    return mCurrent < rhs.mCurrent;
  }

  u1 RawStore::const_iterator::operator>(const_iterator const& rhs) const
  {
    return mCurrent > rhs.mCurrent;
  }

  u1 RawStore::const_iterator::operator<=(const_iterator const& rhs) const
  {
    return mCurrent <= rhs.mCurrent;
  }

  u1 RawStore::const_iterator::operator>=(const_iterator const& rhs) const
  {
    return mCurrent >= rhs.mCurrent;
  }

  RawStore::constdata RawStore::const_iterator::operator*(void) const
  {
    if (!mCurrent)
      throw Exception("Can not access uninitialized RawStore::iterator");

    return getConstDataPackage(mCurrent);
  }

  RawStore::constdata RawStore::const_iterator::operator->(void) const
  {
    if (!mCurrent)
      throw Exception("Can not access uninitialized RawStore::iterator");

    return getConstDataPackage(mCurrent);
  }

  RawStore::const_iterator::const_iterator(ch* data) : mCurrent(data)
  {

  }

  RawStore::const_iterator& RawStore::const_iterator::operator=(ch* data)
  {
    mCurrent = data;
    return *this;
  }




  //////////////////////////////////////////////////////////////////////////
  // Public Autos.

  RawStore::RawStore(void) : mData(NULL), 
                             mEnd(NULL), 
                             mCapacity(0)
  {

  }

  RawStore::~RawStore(void)
  {
    if (mData)
      free(mData);
  }

  RawStore::RawStore(RawStore const& rhs) : mData(NULL),
                                            mEnd(NULL),
                                            mCapacity(0)
  {
    deepcopy(rhs);
  }

  RawStore& RawStore::operator=(RawStore const& rhs)
  {
    deepcopy(rhs);
    return *this;
  }


  //////////////////////////////////////////////////////////////////////////
  // Public Iteration.

  RawStore::iterator RawStore::begin(void)
  {
    return iterator(mData);
  }

  RawStore::const_iterator RawStore::begin(void) const
  {
    return const_iterator(mData);
  }

  RawStore::iterator RawStore::end(void)
  {
    return iterator(mData + (mEnd - mData));
  }

  RawStore::const_iterator RawStore::end(void) const
  {
    return const_iterator(mData + (mEnd - mData));
  }


  //////////////////////////////////////////////////////////////////////////
  // Public Access.


#define IMPLEMENT_ADD_TYPE(TName, FName)                    \
  u1 RawStore::Add##FName(Text id, TName const& dat)        \
  {                                                         \
    return insert(id.c_str(), (ch*)&dat, dat_##TName);      \
  }

#define IMPLEMENT_GET_TYPE(TName, FName)                \
  TName RawStore::Get##FName(Text id) const             \
  {                                                     \
    TName ret;                                          \
    if (!retrieve(id.c_str(), (ch*)&ret, dat_##TName))  \
    {                                                   \
    ErrorIf(true, "INVALID INDEX:\n   - Attempt to call Get%s('%s') failed.\n   - There is no %s value at index '%s'.\n\n POSSIBLE PROBLEMS:\n   - Incorrect version of Get() function is being used\n   - Index '%s' is spelled wrong.\n   - Index '%s' is not guarenteed to exist.\n\nPOSSIBLE SOLUTIONS:\n   - Ensure correct type matching is used (U32, S32, U16...)\n   - Ensure spelling of '%s' is correct.\n   - Check Has%s('%s') first.\n", #FName, id.c_str(), #FName, id.c_str(), id.c_str(), id.c_str(), id.c_str(), #FName, id.c_str());\
  throw Exception(E_UNKNOWN_ID);                        \
    }                                                   \
    return ret;                                         \
  }

#define IMPLEMENT_HAS_TYPE(TName, FName)                    \
  u1 RawStore::Has##FName(Text id) const                    \
  {                                                         \
    TName ret;                                              \
    return (!retrieve(id.c_str(), (ch*)&ret, dat_##TName)); \
  }
#define IMPLENT_ADD_GET_HAS_TYPE(TName, FName) IMPLEMENT_HAS_TYPE(TName, FName); IMPLEMENT_ADD_TYPE(TName, FName); IMPLEMENT_GET_TYPE(TName, FName)


  IMPLENT_ADD_GET_HAS_TYPE(u1, U1);
  IMPLENT_ADD_GET_HAS_TYPE(u8, U8);
  IMPLENT_ADD_GET_HAS_TYPE(s8, S8);
  IMPLENT_ADD_GET_HAS_TYPE(u16, U16);
  IMPLENT_ADD_GET_HAS_TYPE(s16, S16);
  IMPLENT_ADD_GET_HAS_TYPE(u32, U32);
  IMPLENT_ADD_GET_HAS_TYPE(s32, S32);
  IMPLENT_ADD_GET_HAS_TYPE(f32, F32);
  IMPLENT_ADD_GET_HAS_TYPE(f64, F64);
  IMPLENT_ADD_GET_HAS_TYPE(vec2, Vec2);
  IMPLENT_ADD_GET_HAS_TYPE(vec3, Vec3);
  IMPLENT_ADD_GET_HAS_TYPE(vec4, Vec4);
  IMPLENT_ADD_GET_HAS_TYPE(Quaternion, Quat);
  IMPLENT_ADD_GET_HAS_TYPE(ScriptVec2, ScriptVec2);
  IMPLENT_ADD_GET_HAS_TYPE(ScriptVec3, ScriptVec3);
  IMPLENT_ADD_GET_HAS_TYPE(ScriptVec4, ScriptVec4);

  /*IMPLENT_ADD_GET_HAS_TYPE(Text, String);*/
  u1 RawStore::AddString(Text id, Text const& dat)
  {
    return insert(id.c_str(), (ch*)dat.c_str(), dat_str);
  }
  std::string RawStore::GetString(Text id) const
  {
    ch* dat = find(id.c_str());
    if (dat == NULL)
      throw Exception(E_UNKNOWN_ID);
    // The string is stored after type marker, id string, strlen + '\0'
    return std::string(dat + sizeof(DatType) + strlen(dat + sizeof(DatType)) + sizeof(u8) + 1);
  }
  u1 RawStore::HasString(Text id) const
  {
    return find(id.c_str()) != NULL;
  }




  u1 RawStore::HasData(void) const
  {
    return ((s32)mCapacity < (mEnd - mData));
  }


  //////////////////////////////////////////////////////////////////////////
  // Private Helpers.

  void RawStore::grow(size_t newsize)
  {
    ErrorIf ((s32)newsize < (mEnd - mData), "Shrinking would result in data loss!");

    if (newsize == mCapacity)
      return;

    ch* newBlock = (ch*)malloc(newsize);
    
    if (newBlock == NULL) 
      throw Exception(E_MEM_CONT_NOT_NEOUGH);

    size_t inUse = mEnd - mData;
    std::memcpy(newBlock, mData, inUse);

    ch* oldBlock = mData;
    mData = newBlock;
    mEnd = mData + inUse;
    
    if (oldBlock)
      free(oldBlock);

    mCapacity = newsize;
  }

  u1 RawStore::doesFit(size_t size) const
  {
    return ( mCapacity - (mEnd - mData) >= size );
  }

  u1 RawStore::insert(ch const* id, ch* data, DatType datType)
  {
    // Check if id is available
    if (!isFree(id))
      return false;

    // Compute total size of block about to be inserted
    static size_t markersize = sizeof(DatType);
    size_t idsize = strlen(id) + 1;
    
    size_t datsize;
    
    if (datType == dat_str)
    {
      // For strings the total size is a u8 which stores the strlen + the strlen + '\0'
      u32 len = strlen(data) + 1;
      
      if (len > 255)
        return false;

      datsize = sizeof(u8) + len;
    }
    else 
    {
      datsize = getDatSize(datType);
    }

    size_t sizeRequired = markersize + idsize + datsize;

    // Grow container if needed
    if (!doesFit(sizeRequired))
      grow(mCapacity + sizeRequired * 3); // Grow a little more than needed in case more insertion happens

    // Copy marker
    std::memcpy(mEnd, (ch*)&datType, markersize);
    mEnd += markersize;

    // Copy id
    std::memcpy(mEnd, id, idsize);
    mEnd += idsize;


    if (datType == dat_str)
    {
      // Copy strlen + 1 right before the block
      u8 len = strlen(data) + 1;
      std::memcpy(mEnd, (ch*)&len, sizeof(u8));
      mEnd += sizeof(u8);

      std::memcpy(mEnd, data, len);
      mEnd += len;
    }
    else
    {
      // Copy data
      std::memcpy(mEnd, data, datsize);
      mEnd += datsize;
    }

    return true;
  }

  u1 RawStore::retrieve(ch const* id, ch* outData, DatType datType) const
  {
    ErrorIf(datType == dat_str, "Function can not retrieve data of type string!");

    ch* dat = find(id);

    if (!dat)
      return false;

    if (*((DatType*)dat) != datType)
      return false;

    std::memcpy(outData, dat + sizeof(DatType) + strlen(id) + 1, getDatSize(datType));

    return true;
  }

  u1 RawStore::isFree(ch const* id) const
  {
    return ( find(id) == NULL );
  }


  ch* RawStore::find(ch const* id) const
  {
    if (!mData)
      return NULL;

    ch* curr = mData;

    while (curr < mEnd)
    {
      ch const* currId = curr + sizeof(DatType);

      if (strcmp(id, currId) == 0)
        return curr;

      size_t increment = sizeof(DatType) + strlen(curr + sizeof(DatType)) + 1;

      DatType marker = *((DatType*)curr);

      if (marker == dat_str)
      {
        increment += sizeof(u8) + *((u8*)(curr + increment));
      }
      else
      {
        increment += getDatSize(marker);
      }

      curr += increment;
    }

    ErrorIf(curr != mEnd, "Wrongly incremented during search algorithm");
    return NULL;
  }

  size_t RawStore::getDatSize(DatType const& type)
  {

    switch (type)
    {
    case dat_u1:
      return sizeof(u1);
    case dat_u8:
      return sizeof(u8);
    case dat_s8:
      return sizeof(s8);
    case dat_u16:
      return sizeof(u16);
    case dat_s16:
      return sizeof(s16);
    case dat_u32:
      return sizeof(u32);
    case dat_s32:
      return sizeof(s32);
    case dat_f32:
      return sizeof(f32);
    case dat_u64:
      return sizeof(u64);
    case dat_s64:
      return sizeof(s64);
    case dat_f64:
      return sizeof(f64);
    case dat_vec2:
      return sizeof(f32) * 2;
    case dat_vec3:
      return sizeof(f32) * 3;
    case dat_vec4:
      return sizeof(f32) * 4;
    }

    Error("Wrong type passed for check");
    return -1;
  }

  ch* RawStore::next(ch* current)
  {
    DatType type = *RCAST(DatType*, current);

    size_t size;

    u8 len = strlen(current + sizeof(DatType)) + 1;

    if (type == dat_str)
    {
      size = *(current + sizeof(DatType) + len) + sizeof(u8);
    }
    else
      size = RawStore::getDatSize(type);

    return current + sizeof(DatType) + len + size;
  }

  RawStore::data RawStore::getDataPackage(ch* dat)
  {
    data ret;
    ret.Type = *RCAST(DatType*, dat);
    ret.Id = dat + sizeof(DatType);

    ret.Data = dat + sizeof(DatType) + strlen(ret.Id) + 1;

    if (ret.Type == dat_str)
      ret.Data = RCAST(ch*, ret.Data) + sizeof(u8);

    return ret;
  }

  RawStore::constdata RawStore::getConstDataPackage(ch* dat)
  {
    return *RCAST(constdata*, &getDataPackage(dat));
  }


  void RawStore::deepcopy(RawStore const& rhs)
  {
    if (mData)
      free(mData);

    mData = NULL;
    mEnd = NULL;
    mCapacity = 0;

    if (!rhs.mData)
      return;

    mData = (ch*)malloc(rhs.mCapacity);

    size_t sizeInUse = rhs.mEnd - rhs.mData;
    std::memcpy(mData, rhs.mData, sizeInUse);

    mEnd = mData + sizeInUse;
    mCapacity = rhs.mCapacity;
  }

} // namespace Virtus