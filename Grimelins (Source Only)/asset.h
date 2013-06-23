/**********************************************************************/
/*
  
  \file
    asset.h
  
  \author
    Chris Peters
    All content © 2011 DigiPen (USA) Corporation, all rights reserved.

  \brief
    A base asset class borrowe from Chris Peters for reference counting
    things.  We may move this into graphics, but for now we'll call it an 
    engine utility.
    
*/
/**********************************************************************/
#ifndef VIRTUS_ASSET_H
#define VIRTUS_ASSET_H

namespace Virtus
{
  //Base asset class provides AddRef/Release semantics for all assets.
  class Asset
  {
  public:
    Asset() : ReferenceCount(1) {};
    virtual void AddRef();
    virtual int Release();
    virtual ~Asset(){};
  private:
    int ReferenceCount;
  };

  inline void Asset::AddRef()
  {
    ++ReferenceCount;
  }

  inline int Asset::Release()
  {
    --ReferenceCount;
    if( ReferenceCount == 0 )
    {
      delete this;
      return 0;
    }
    return ReferenceCount;
  };
}

#endif //VIRTUS_ASSET_H