/*! ====================================================================================================================
  @file Resources->inl

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date 30.Sep.2011

  @brief 
    Resource management

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */

namespace Grimelins
{
    //////////////////////////////////////////////////////////////////////////
    // Resource Manager for user type instantiations

    template <typename USERDATATYPE>
    ResourceManager<USERDATATYPE>::ResourceManager(void) : mResCount(0)
    {

    }

    template <typename USERDATATYPE>
    ResourceManager<USERDATATYPE>::~ResourceManager(void)
    {
      ErrorIf(mResCount != 0, "Warning: Shutting down resource manager with open Resources->");

      ResourceTable::iterator it = mResources.begin();
      ResourceTable::iterator endIt = mResources.end();

      for (; it != endIt; ++it)
       if (*it)
         delete *it;
    }

    template <typename USERDATATYPE>
    typename USERDATATYPE* ResourceManager<USERDATATYPE>::Acquire(Handle& outHandle)
    {
      ErrorIf(!outHandle.IsNull(), "Attempt to Acquire resource using initialized handle");
      USERDATATYPE* ret = new USERDATATYPE();
      ++mResCount;

      u32 id;

      if (mFreeSlots.empty())
      {
        id = mMagicNumbers.size();
        outHandle.Init(id, this->GetOwnerId());
        mMagicNumbers.push_back(outHandle.GetMagic());
        mResources.push_back(ret);
      }
      else
      {
        id = mFreeSlots.back();
        mFreeSlots.pop_back();
        outHandle.Init(id, this->GetOwnerId());
        mMagicNumbers[id] = outHandle.GetMagic();
        mResources[id] = ret;
      }

      return ret;
    }

    template <typename USERDATATYPE>
    void ResourceManager<USERDATATYPE>::Release(Handle handle)
    {
      if (handle.IsNull())
        return;

      u32 index = handle.GetIndex();

      ErrorIf(index >= mResources.size(), "Invalid handle!");
      ErrorIf(mMagicNumbers[index] != handle.GetMagic(), "Invalid handle!");

      delete mResources[index];
      --mResCount;
      mResources[index] = NULL;
      mMagicNumbers[index] = 0;
      mFreeSlots.push_back(index);
    }

    template <typename USERDATATYPE>
    typename USERDATATYPE* ResourceManager<USERDATATYPE>::Dereference(Handle handle)
    {
      if (handle.IsNull())
        return NULL;

      u32 index = handle.GetIndex();

#if VIRTUS_CONFIG_DEBUG_MODE || VIRTUS_CONFIG_PROFILE_MODE

      if ( index >= mResources.size() ||
        mMagicNumbers[index] != handle.GetMagic() )
      {
        Error("Corrupted handle passed to Dereference");
        return NULL;
      }

#endif

      return mResources[index];
    }

    template <typename USERDATATYPE>
    typename USERDATATYPE const* ResourceManager<USERDATATYPE>::Dereference(Handle handle) const
    {
      return CCAST(ResourceManager()<USERDATATYPE>*, this)->Dereference(handle);
    }

    template <typename USERDATATYPE>
    u1 ResourceManager<USERDATATYPE>::HasResources(void) const
    {
      return mResCount != 0;
    }

    template <typename USERDATATYPE>
    u32 ResourceManager<USERDATATYPE>::GetResourceCount(void) const
    {
      return mResCount;
    }

    template <typename USERDATATYPE>
    u1 ResourceManager<USERDATATYPE>::IsValid(Handle handle) const
    {
      if (handle.IsNull())
        return false;

      u32 index = handle.GetIndex();

      if (index >= mResources.size())
        return false;

      if (mMagicNumbers[index] != handle.GetMagic())
        return false;

      return true;
    }


    //////////////////////////////////////////////////////////////////////////
    // ResourceMaster

    template <typename USERDATATYPE>
    void ResourceMaster::InitializeManager(ResourceManager<USERDATATYPE>& manager)
    {
      ErrorIf(manager.mOwnerId != 0, "Attempt to initialize in use Resource Manager");

      u32 id;

      if (mFreeVec.size())
      {
        // Found an available entry
        id = mFreeVec.back();
        mFreeVec.pop_back();
        mResourceManagers[id++] = &manager;
      }
      else
      {
        // Create a new entry
        mResourceManagers.push_back(&manager);
        id = mResourceManagers.size();
      }

      manager.mOwnerId = id;
      manager.mMaster = this;
    }

} // namespace Virtus