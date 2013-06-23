/*! ====================================================================================================================
  @file Resources->h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 30.Sep.2011

  @brief 
    Resource management

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_RESOURCES_H
#define VIRTUS_RESOURCES_H

#include "singleton.h"

namespace Grimelins
{

    //////////////////////////////////////////////////////////////////////////
    // Handle Class

    class Handle
    {
      friend class IResourceManager;

      enum
      {
        MAX_BITS_INDEX = 16,
        MAX_BITS_MAGIC = 8,
        MAX_BITS_OWNER = 8,

        MAX_INDEX = (1 << MAX_BITS_INDEX) - 1,
        MAX_MAGIC = (1 << MAX_BITS_MAGIC) - 1,
        MAX_OWNER = (1 << MAX_BITS_OWNER) - 1
      };

      union
      {
        struct  
        {
          u16 mIndex : MAX_BITS_INDEX; // HO 16 bits as resource index
          u8  mMagic : MAX_BITS_MAGIC; // Next 8 bits for magic validation number
          u8  mOwner : MAX_BITS_OWNER; // Next 8 bits for owner id
        };

        u32 mHandle;
      };

    public:
      Handle(void) : mHandle(0) {}

      void Init(u32 index, u32 owner);
      u16 GetIndex(void) const { return mIndex; }
      u8  GetMagic(void) const { return mMagic; }
      u8  GetOwner(void) const { return mOwner; }
      u32 GetHandle(void) const { return mHandle; }
      u1  IsNull(void) const { return !mHandle; }

      operator u32(void) const { return mHandle; }
      operator u1(void) const { return !!mMagic; }
      u1 operator==(Handle const& rhs) const { return mHandle == rhs.mHandle; }
      u1 operator!=(Handle const& rhs) const { return mHandle != rhs.mHandle; }
      u1 operator< (Handle const& rhs) const { return mHandle <  rhs.mHandle; }
      u1 operator<=(Handle const& rhs) const { return mHandle <= rhs.mHandle; }
      u1 operator> (Handle const& rhs) const { return mHandle >  rhs.mHandle; }
      u1 operator>=(Handle const& rhs) const { return mHandle >= rhs.mHandle; }

      static Handle Construct(u32 num) { Handle ret; ret.mHandle = num; return ret; }
    };

    #define NullHandle Handle()


    //////////////////////////////////////////////////////////////////////////
    // Resource Manager Base

    class IResourceManager
    {
      friend class ResourceMaster;

    public:
      IResourceManager(void);
      virtual ~IResourceManager(void);
      u8 GetOwnerId(void) const;
      u1 IsValid(void) const;

    public:
      virtual u1 HasResources(void) const = 0;
      virtual u32 GetResourceCount(void) const = 0;
      virtual u1 IsValid(Handle handle) const = 0;

    protected:
      void resetHandle(Handle* handle) { handle->mHandle = 0; }

    private:
      ResourceMaster* mMaster;
      u8              mOwnerId;
    };

    //////////////////////////////////////////////////////////////////////////
    // Resource Manager Type instantiations

    template <typename USERDATATYPE>
    class ResourceManager : public IResourceManager
    {
    public:
      ResourceManager(void); // Auto Initializes to invalid state (in base)
      virtual ~ResourceManager(void); // Auto unregisters and cleans up (in base)
    
    private: // Deny copies
      ResourceManager(ResourceManager const& rhs);
      ResourceManager& operator=(ResourceManager const& rhs);
     

    public:
      USERDATATYPE* Acquire(Handle& outHandle);
      void Release(Handle handle);

      USERDATATYPE* Dereference(Handle handle);
      USERDATATYPE const* Dereference(Handle handle) const;

      virtual u1 HasResources(void) const;
      virtual u32 GetResourceCount(void) const;
      virtual u1 IsValid(Handle handle) const;

    private: // Decl
      typedef std::vector<USERDATATYPE*> ResourceTable;
      typedef std::vector<u8>            MagicTable;
      typedef std::vector<u32>           FreeTable;

    private: // Data
      ResourceTable   mResources;
      MagicTable      mMagicNumbers;
      FreeTable       mFreeSlots;
      u32             mResCount;
    };


    //////////////////////////////////////////////////////////////////////////
    // ResourceMaster (the Manager of ResourceManagers -- come up with a better name)

    class ResourceMaster : public Singleton<ResourceMaster>
    {
      friend class Core;

    public:
      ResourceMaster(void);
      ~ResourceMaster(void);

    public:
      template <typename USERDATATYPE>
      void InitializeManager(ResourceManager<USERDATATYPE>& manager);
      void UninitializeManager(IResourceManager& manager);

      u32  GetManagerCount(void) const;

    private:
      std::vector<IResourceManager*> mResourceManagers;
      std::vector<u32>               mFreeVec;
      u32                       mManagerCount;
    };


} // namespace Virtus

#include "resman.inl"

#endif // include guard