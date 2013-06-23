#include <d3d10.h>
#include <d3dx10.h>
#include <DxErr.h>

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
  {                                                          \
  HRESULT hr = (x);                                      \
  if(FAILED(hr))                                         \
    {                                                      \
    DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
    }                                                      \
  }
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }


//*****************************************************************************
// Convenience functions.
//*****************************************************************************

// Converts ARGB 32-bit color format to ABGR 32-bit color format.
inline UINT ARGB2ABGR(UINT argb)
{
	BYTE A = (argb >> 24) & 0xff;
	BYTE R = (argb >> 16) & 0xff;
	BYTE G = (argb >>  8) & 0xff;
	BYTE B = (argb >>  0) & 0xff;

	return (A << 24) | (B << 16) | (G << 8) | (R << 0);
}

// Returns random float in [0, 1).
inline float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
inline float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

// Returns random vector on the unit sphere.
inline D3DXVECTOR3 RandUnitVec3()
{
	D3DXVECTOR3 v(RandF(), RandF(), RandF());
	D3DXVec3Normalize(&v, &v);
	return v;
}
 
template<typename T>
inline T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
inline T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}
 
template<typename T>
inline T Lerp(const T& a, const T& b, float t)
{
	return a + (b-a)*t;
}

template<typename T>
inline T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x); 
}