/*
 Copyright (c) 2013, Insomniac Games
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the
 following disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 \file
 \author Ron Pieket \n<http://www.ItShouldJustWorkTM.com> \n<http://twitter.com/RonPieket>
 */
/* MojoLib is documented at: http://www.ItShouldJustWorkTM.com/mojolib/ */

// ---------------------------------------------------------------------------------------------------------------

#pragma once

#include "MojoConstants.h"
#include "MojoAbstractSet.h"
#include "MojoCollector.h"
#include "MojoUtil.h"

/**
 \class MojoIntersection
 \ingroup group_boolean
 Represents the boolean intersection of two or more sets.
 A key is an element of the intersection if it is an element of all sets.
 
 `S.Contains(key) = S1.Contains(key) AND S2.Contains(key) AND S3.Contains(key) ...`

 `S = S1 ∩ S2 ∩ S3 ...`

 \image html Set-Intersection.png
 */
template< typename key_T >
class MojoIntersection final : public MojoAbstractSet< key_T >
{
public:
  /**
   Default constructor for up to four input sets.
   More sets may be added by calling Add()
   \param[in] s1 Input set.
   \param[in] s2 Input set.
   \param[in] s3 Input set.
   \param[in] s4 Input set.
   */
  MojoIntersection( const MojoAbstractSet< key_T >* s1 = NULL, const MojoAbstractSet< key_T >* s2 = NULL,
                   const MojoAbstractSet< key_T >* s3 = NULL, const MojoAbstractSet< key_T >* s4 = NULL );
  /**
   Add another input set.
   \param[in] s The set to add.
   \return *this, for convenient chaining:
   \code
   my_set.Add( &s1 ).Add( &s2 ).Add( &s3 );
   \endcode
   */
  MojoIntersection& Add( const MojoAbstractSet< key_T >* s );
  virtual bool Contains( const key_T& key ) const override;
  virtual bool Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;
  
private:
  const MojoAbstractSet< key_T >* m_Sets[ kMojoInputSetMax ];
  int                       m_SetCount;
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename key_T >
MojoIntersection< key_T >::MojoIntersection( const MojoAbstractSet< key_T >* s1,
                                             const MojoAbstractSet< key_T >* s2,
                                             const MojoAbstractSet< key_T >* s3,
                                             const MojoAbstractSet< key_T >* s4 )
: m_SetCount( 0 )
{
  Add( s1 );
  Add( s2 );
  Add( s3 );
  Add( s4 );
}

template< typename key_T >
inline MojoIntersection< key_T >& MojoIntersection< key_T >::Add( const MojoAbstractSet< key_T >* s )
{
  if( s )
  {
    if( m_SetCount < kMojoInputSetMax )
    {
      m_Sets[ m_SetCount++ ] = s;
    }
  }
  return *this;
}

template< typename key_T >
inline bool MojoIntersection< key_T >::Contains( const key_T& key ) const
{
  for( int i = 0; i < m_SetCount; ++i )
  {
    if( !m_Sets[ i ]->Contains( key ) )
      return false;
  }
  return true;
}

template< typename key_T >
inline int MojoIntersection< key_T >::_GetEnumerationCost() const
{
  int lowest_cost = INT_MAX;
  for( int i = 0; i < m_SetCount; ++i )
  {
    const MojoAbstractSet< key_T >* set = m_Sets[ i ];
    int cost = set->_GetEnumerationCost();
    if( cost < lowest_cost )
    {
      lowest_cost = cost;
    }
  }
  return lowest_cost;
}

template< typename key_T >
inline int MojoIntersection< key_T >::_GetChangeCount() const
{
  int count = 0;
  for( int i = 0; i < m_SetCount; ++i )
  {
    count += m_Sets[ i ]->_GetChangeCount();
  }
  return count;
}

template< typename key_T >
inline bool MojoIntersection< key_T >::Enumerate( const MojoCollector< key_T >& collector,
                                                 const MojoAbstractSet< key_T >* limit ) const
{
  MojoIntersection< key_T > combined_limit;
  
  const MojoAbstractSet< key_T >* smaller = m_Sets[ 0 ];
  int lowest_cost = smaller->_GetEnumerationCost();
  
  for( int i = 1; i < m_SetCount; ++i )
  {
    const MojoAbstractSet< key_T >* set = m_Sets[ i ];
    int cost = set->_GetEnumerationCost();
    if( cost < lowest_cost )
    {
      combined_limit.Add( smaller );
      smaller = set;
      lowest_cost = cost;
    }
    else
    {
      combined_limit.Add( set );
    }
  }
  
  if( limit )
  {
    combined_limit.Add( limit );
  }
  
  return smaller->Enumerate( collector, &combined_limit );
}

// ---------------------------------------------------------------------------------------------------------------
