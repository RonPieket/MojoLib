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

#include "MojoAbstractSet.h"
#include "MojoSet.h"
#include "MojoStatus.h"

class MojoAlloc;
struct MojoConfig;

/**
 \class MojoCacheSet
 \ingroup group_set_common
 Designed for optimization of set expressions. Normally, a set expression is evaluated when Contains() or
 Enumerate() is called. The result is not stored, but returned to the caller. This ensures that Contains() and
 Enumerate() always return results based on the current state of the containers at the expression leaves.
 MojoLib tries to optimize evaluation in several ways. However, if you have an expression or sub-expression that:
 - is complex, and
 - is frequently accessed, and
 - is often unchanged between acccesses

 ...then this on-demand evaluation scheme will cause some unnecessary processing and memory access. *If* this is a
 concern, MojoCacheSet can help. A cache set is initialized with a pointer to a set to be cached. When \ref
 MojoCacheSet::Update() is called, it will enumerate the input set into its internal set. This internal set is
 used when the MojoCacheSet is accessed through Contains() and Enumerate().
 */
template< typename key_T >
class MojoCacheSet final : public MojoAbstractSet< key_T >
{
public:

  MojoCacheSet()
  {}

  /**
   Initializing constructor. No need to call Create().
   \param[in] name The name of the set. Will also be used for internal memory allocation.
   \param[in] set_to_cache Pointer to the set we're caching here.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array You may provide an array that the set will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   */
  MojoCacheSet( const char* name, MojoAbstractSet< key_T >* set_to_cache, const MojoConfig* config = NULL,
               MojoAlloc* alloc = NULL, key_T* fixed_array = NULL, int fixed_array_count = 0 )
  {
    Init();
    Create( name, set_to_cache, config, alloc, fixed_array, fixed_array_count );
  }

  /**
   Create after default constructor or Destroy().
   \param[in] name The name of the set. Will also be used for internal memory allocation.
   \param[in] set_to_cache Pointer to the set we're caching here.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array You may provide an array that the set will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   \return Status code.
   */
  MojoStatus Create( const char* name, MojoAbstractSet< key_T >* set_to_cache, const MojoConfig* config = NULL,
                   MojoAlloc* alloc = NULL, key_T* fixed_array = NULL, int fixed_array_count = 0 );

  virtual bool Contains( const key_T& key ) const override;
  virtual void Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;
  
  /**
   If the input set has changed, update the internal set. Otherwise do nothing.
   */
  void Update();

  /**
   Access to internal MojoSet.
   \private
   */
  int _GetFirstIndex() const;
  
  /**
   Access to internal MojoSet.
   \private
   */
  int _GetNextIndex( int index ) const;
  
  /**
   Access to internal MojoSet.
   \private
   */
  bool _IsIndexValid( int index ) const;
  
  /**
   Access to internal MojoSet.
   \private
   */
  key_T _GetKeyAt( int index ) const;

private:
  const char*                     m_Name;
  const MojoAbstractSet< key_T >* m_SetToCache;
  MojoSet< key_T >                m_CachedSet;
  int                             m_ChangeCount;

  void                            Init();
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename key_T >
MojoStatus MojoCacheSet< key_T >::Create( const char* name, MojoAbstractSet< key_T >* set_to_cache,
                                        const MojoConfig* config, MojoAlloc* alloc, key_T* fixed_array,
                                        int fixed_array_count )
{
  m_Name = name;
  m_SetToCache = set_to_cache;
  return m_CachedSet.Create( name, config, alloc, fixed_array, fixed_array_count );
}

template< typename key_T >
bool MojoCacheSet< key_T >::Contains( const key_T& key ) const
{
  return m_CachedSet.Contains( key );
}

template< typename key_T >
void MojoCacheSet< key_T >::Enumerate( const MojoCollector< key_T >& collector,
                                      const MojoAbstractSet< key_T >* limit ) const
{
  m_CachedSet.Enumerate( collector, limit );
}

template< typename key_T >
int MojoCacheSet< key_T >::_GetEnumerationCost() const
{
  return m_CachedSet._GetEnumerationCost();
}

template< typename key_T >
int MojoCacheSet< key_T >::_GetChangeCount() const
{
  return m_ChangeCount;
}

template< typename key_T >
void MojoCacheSet< key_T >::Update()
{
  int change_count = m_SetToCache->_GetChangeCount();
  if( m_ChangeCount != change_count )
  {
    m_ChangeCount = change_count;
    m_CachedSet.Clear();
    m_SetToCache->Enumerate( MojoSetCollector< key_T >( &m_CachedSet ) );
  }
}

template< typename key_T >
int MojoCacheSet< key_T >::_GetFirstIndex() const
{
  return m_CachedSet._GetFirstIndex();
}

template< typename key_T >
int MojoCacheSet< key_T >::_GetNextIndex( int index ) const
{
  return m_CachedSet._GetNextIndex( index );
}

template< typename key_T >
bool MojoCacheSet< key_T >::_IsIndexValid( int index ) const
{
  return m_CachedSet._IsIndexValid( index );
}

template< typename key_T >
key_T MojoCacheSet< key_T >::_GetKeyAt( int index ) const
{
  return m_CachedSet._GetKeyAt( index );
}

template< typename key_T >
void MojoCacheSet< key_T >::Init()
{
}

// ---------------------------------------------------------------------------------------------------------------
