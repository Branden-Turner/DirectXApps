/*! ====================================================================================================================
  @file resource.cpp

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 30.Sep.2011

  @brief 
    Resource management

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#include "precompiled.h"
#include "resman.h"

namespace Grimelins
{
    //////////////////////////////////////////////////////////////////////////
    // Handle

    void Handle::Init(u32 index, u32 owner)
    {
      ErrorIf(!IsNull(), "Reinitialization of handle is forbidden");
      ErrorIf(index > MAX_INDEX, "Initialization with invalid index");

      static u32 sAutoMagic = 0;

      if (++sAutoMagic > MAX_MAGIC)
        sAutoMagic = 1; // 0 is reserved for invalid index

      mIndex = index;
      mMagic = sAutoMagic;
      mOwner = owner;
    }

    //////////////////////////////////////////////////////////////////////////
    // Resource Manager Base

    IResourceManager::IResourceManager(void) : mMaster(NULL), mOwnerId(0)
    {

    }

    IResourceManager::~IResourceManager(void)
    {
      ErrorIf(mOwnerId && !mMaster, "Error: Corrupted Resource Manager");
      ErrorIf(mMaster && !mOwnerId, "Error: Corrupted Resource Manager");

      if (mMaster)
      {
        mMaster->UninitializeManager(*this);
      }
    }

    u8 IResourceManager::GetOwnerId(void) const
    {
      return mOwnerId;
    }

    u1 IResourceManager::IsValid(void) const
    {
      return mOwnerId != 0;
    }





    //////////////////////////////////////////////////////////////////////////
    // ResourceMaster

    ResourceMaster::ResourceMaster(void) : mManagerCount(0)
    {

    }

    ResourceMaster::~ResourceMaster(void)
    {
      ErrorIf(mManagerCount != 0, "WARNING: ResourceMaster is shutting down with open resources!");
    }


    void ResourceMaster::UninitializeManager(IResourceManager& manager)
    {
      u8 ownerId = manager.GetOwnerId();

      if (ownerId == 0)
        return; // not initialized

      ErrorIf(manager.HasResources(), "Warning: Attempt to unitialize resource manager with open Resources->");
      ErrorIf(ownerId < 1 || ownerId > mResourceManagers.size(), "Warning: Attempt to unitialize invalid resource manager.");
      ErrorIf(manager.mMaster != this, "Error: Corrupted resource manager");
      ErrorIf(mResourceManagers[ownerId - 1] != &manager, "Error: Invalid Resource manager.");

      mFreeVec.push_back(ownerId - 1);
      mResourceManagers[ownerId - 1] = NULL;

      manager.mOwnerId = 0;
      manager.mMaster = NULL;
    }

    u32 ResourceMaster::GetManagerCount(void) const
    {
      return mManagerCount;
    }

} // namespace Virtus