/*! ====================================================================================================================
  @file optional.h

  Codename Virtus

  @author Maximilian Manndorff <max@lo3.com>
  @date May.14.2011

  @brief 
    

                  * Copyright 2011 - 2012, DigiPen Institute of Technology, All Rights Reserved *
===================================================================================================================== */
#ifndef VIRTUS_OPTIONAL_H
#define VIRTUS_OPTIONAL_H

namespace Grimelins
{
  /// Used as the empty data for the <see cref="Optional"> class.
  class OptionalEmpty
  {

  };

  /// Base class for the <see cref="Optional"> class which manages the memory used by the latter.
  template <unsigned long Tsize>
  class BOptional
  {
  public:
    BOptional(void);
    BOptional(BOptional const& orig);
    BOptional& operator=(BOptional const& rhs);

    bool IsValid(void) const;

  protected:
    bool m_bValid;
    char m_Data[Tsize];
  };


  /// <summary>
  /// The optional class allows optional return values. A common problem is often what to return in case of a function
  /// failure. The best solution to this problem is to return this class with either the Data embedded or marked as 
  /// invalid.
  /// </summary>
  /// <remarks>
  /// The class defaults to marking the data as invalid.
  /// </remarks>
  /// <example>
  /// This code shows a case where the Optional class is useful.
  /// <code>
  /// optional<float> Divide(float number, float divisor)
  /// {
  ///   optional<float> result; // Defaults to invalid
  ///   
  ///   if (divisor != 0.0f)
  ///     result = number / divisor;
  ///
  ///   return result;
  /// }
  /// </code>
  /// In this example optional will contain the result of the division if the divisor is not zero, otherwise
  /// optional will be marked as invalid.
  /// </example>
  template <typename T>
  class Optional : public BOptional<sizeof(T)>
  {
  public:
    // Autos - Class defaults to invalid
    Optional(void);
    Optional(T const& dat);
    Optional(OptionalEmpty const&);
    Optional(Optional const& orig);
    ~Optional(void);

  public:
    // Operators
    Optional& operator=(T const& dat);
    Optional& operator=(Optional const& orig);

    bool operator==(Optional const& rhs) const;
    bool operator<(Optional const& rhs) const;

  public:
    // Access
    T const& operator*(void) const;
    T& operator*(void);
    T& operator->(void);

  public:
    // Methods
    void Clear(void);
    bool IsValid(void) const;

  private:
    //Methods
    T const* const GetT(void) const;
    T* const GetT(void);
    void construct(T const& dat);
    void destroy(void);
  };

  class OptionalException
  {
  public:
    enum ErrCode
    {
      INVALID_ACCESS
    };

    OptionalException(ErrCode code) : m_Error(code) {}
    ErrCode what(void) const { return m_Error; }

  private:
    ErrCode m_Error;
  };

} // namespace virtus

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "optional.inl"


#endif // Include Guard