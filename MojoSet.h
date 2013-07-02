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

// -- Standard Libs
#include <stdint.h>
#include <assert.h>
#include <new>

// -- Mojo
#include "MojoConstants.h"
#include "MojoStatus.h"
#include "MojoAlloc.h"
#include "MojoConfig.h"
#include "MojoUtil.h"
#include "MojoArray.h"
#include "MojoAbstractSet.h"
#include "MojoCollector.h"

/**
 \class MojoSet
 \ingroup group_container
 A key-only hash table.
 Also implements the MojoAbstractSet interface.
 \see MojoForEachKey
 \tparam key_T Key type. Must be hashable.
 */
template< typename key_T >
class MojoSet final : public MojoAbstractSet< key_T >
{
public:
  
  /**
   Default constructor. You must call Create() before the set is ready for use.
   */
  MojoSet()
  {
    Init();
  }

  /**
   Initializing constructor. No need to call Create().
   \param[in] name The name of the set. Will also be used for internal memory allocation.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global default will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array You may provide an array that the set will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   */
  MojoSet( const char* name, const MojoConfig* config = NULL, MojoAlloc* alloc = NULL,
          key_T* fixed_array = NULL, int fixed_array_count = 0 )
  {
    Init();
    Create( name, config, alloc, fixed_array, fixed_array_count );
  }
  
  /**
   Create after default constructor or Destroy().
   \param[in] name The name of the set. Will also be used for internal memory allocation.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global default will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array You may provide an array that the set will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   \return Status code.
   */
  MojoStatus Create( const char* name, const MojoConfig* config = NULL, MojoAlloc* alloc = NULL,
                   key_T* fixed_array = NULL, int fixed_array_count = 0 );

  /**
   Remove all keys and free all allocated buffers.
   */
  void Destroy();
  
  /**
   Remove all the leys.
   */
  MojoStatus Clear();
  
  /**
   Insert key into set. If key already exists in set, does nothing.
   \param[in] key Key to insert.
   \return Status code.
   */
  MojoStatus Insert( const key_T& key );

  /**
   Remove key from the set.
   */
  MojoStatus Remove( const key_T& key );

  /**
   Test presence of a key.
   */
  virtual bool Contains( const key_T& key ) const override;

  /**
   Return table status state. This is the only way to find out if something went wrong in the default constructor.
   If Create() was used, the returned status code will be the same.
   \return Status code.
   */
  MojoStatus GetStatus() const;
  
  /**
   Get number of keys in the set.
   */
  int GetCount() const;

  /**
   Return name of the set.
   */
  const char* GetName() const { return m_Name; }

  key_T GetOnlyElement() const { return m_ActiveCount == 1 ? _GetKeyAt( _GetFirstIndex() ) : key_T(); }

  /**
   Get index of first occupied slot in table. This is used for the ForEach... macros. It must be declared public
   to work with the macros, but should be considered private.
   \private
   */
  int _GetFirstIndex() const;

  /**
   Get index of next occupied slot in table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  int _GetNextIndex( int index ) const;

  /**
   Verify that table index is in range. This is used for the ForEach... macros. It must be declared public to work
   with the macros, but should be considered private.
   \private
   */
  bool _IsIndexValid( int index ) const;

  /**
   Get key at a specific index in the table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  key_T _GetKeyAt( int index ) const;

  virtual bool Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;

  virtual ~MojoSet();

private:

  MojoAlloc*          m_Alloc;
  const char*         m_Name;
  key_T*              m_Buffer;
  int                 m_BufferCount;     // Entries allocated
  int                 m_ActiveCount;    // Number of key/values assigned
  int                 m_TableCount;     // Portion of the array currently used for hash table
  int                 m_ChangeCount;
  MojoStatus          m_Status;
  MojoConfig          m_Config;
  
  void Init();
  int FindEmptyOrMatching( const key_T& key ) const;
  int FindEmpty( const key_T& key ) const;
  void Reinsert( int index );
  bool RemoveOne( const key_T& key );
  
  MojoStatus Shrink();
  MojoStatus Grow();
  MojoStatus Resize( int new_table_count );
  void ResizeTableInPlace( int old_table_count );
  void CopyTable( key_T* old_table, int old_table_count );
  key_T* AllocAndConstruct( int new_buffer_count );
  void DestructAndFree( key_T* old_buffer, int old_buffer_count );
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

/**
 \class MojoSetCollector
 Specialization of MojoCollector, with MojoSet as receiver.
 */
template< typename value_T >
class MojoSetCollector final : public MojoCollector< value_T >
{
public:
  /**
   Construct from a MojoSet pointer.
   \param[in] set The set to receive data.
   */
  MojoSetCollector( MojoSet< value_T >* set )
  : m_Set( set )
  {}
  virtual bool Push( const value_T& value ) const override
  {
    m_Set->Insert( value );
    return true;
  }
private:
  MojoSet< value_T >* m_Set;
};

template< typename key_T >
void MojoSet< key_T >::Init()
{
  m_Alloc = NULL;
  m_Name = NULL;
  m_Buffer = NULL;
  m_TableCount = 0;
  m_BufferCount = 0;
  m_ActiveCount = 0;
  m_ChangeCount = 0;
  m_Status = kMojoStatus_NotInitialized;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::GetStatus() const
{
  return m_Status;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Create( const char* name, const MojoConfig* config, MojoAlloc* alloc,
                                   key_T* fixed_array, int fixed_array_count )
{
  if( !config )
  {
    config = MojoConfig::GetDefault();
  }
  if( !alloc )
  {
    alloc = MojoAlloc::GetDefault();
  }
  if( m_Status != kMojoStatus_NotInitialized )
  {
    m_Status = kMojoStatus_DoubleInitialized;
  }
  else if( config->m_BufferMinCount < kMojoTableMinCount )
  {
    m_Status = kMojoStatus_InvalidArguments;
  }
  else
  {
    m_Name            = name;
    m_Alloc           = alloc;
    m_Config          = *config;

    if( fixed_array_count )
    {
      m_Config.m_DynamicAlloc = false;
      m_Config.m_BufferMinCount = fixed_array_count;
      m_Alloc                 = NULL; // Destroy relies on this to not free memory.

      m_Buffer                = fixed_array;
      m_BufferCount           = fixed_array_count;
      // Note to self: fixed array is assumed to consist of constructed values. Don't call Construct() here.
    }
    else
    {
      m_BufferCount           = m_Config.m_BufferMinCount;
      m_Buffer                = AllocAndConstruct( m_BufferCount );
      m_TableCount            = m_Config.m_DynamicTable ? kMojoTableMinCount : m_BufferCount;
    }

    m_Status = m_Buffer ? kMojoStatus_Ok : kMojoStatus_CouldNotAlloc;
  }
  return m_Status;
}

template< typename key_T >
MojoSet< key_T >::~MojoSet()
{
  Destroy();
}

template< typename key_T >
void MojoSet< key_T >::Destroy()
{
  if( m_Alloc )
  {
    DestructAndFree( m_Buffer, m_BufferCount );
  }
  Init();
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Clear()
{
  for( int i = 0; i < m_TableCount; ++i )
  {
    m_Buffer[ i ] = key_T();
  }
  m_ActiveCount = 0;
  m_ChangeCount += 1;
  return Resize( m_Config.m_BufferMinCount );
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Insert( const key_T& key )
{
  MojoStatus status = m_Status;
  if( !status )
  {
    if( key.IsHashNull() )
    {
      status = kMojoStatus_InvalidArguments;
    }
    else
    {
      status = Grow();
      if( !status )
      {
        int index = FindEmptyOrMatching( key );
        if( m_Buffer[ index ].IsHashNull() )
        {
          m_Buffer[ index ] = key;
          m_ActiveCount += 1;
          m_ChangeCount += 1;
        }
      }
    }
  }
  return status;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Remove( const key_T& key )
{
  if( m_Status )
  {
    return m_Status;
  }
  else if( !key.IsHashNull() )
  {
    if( RemoveOne( key ) )
    {
      m_ChangeCount += 1;
      Shrink();
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T >
bool MojoSet< key_T >::Contains( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    int index = FindEmptyOrMatching( key );
    return !m_Buffer[ index ].IsHashNull();
  }
  return false;
}

template< typename key_T >
int MojoSet< key_T >::GetCount() const
{
  return m_ActiveCount;
}

template< typename key_T >
int MojoSet< key_T >::_GetFirstIndex() const
{
  if( m_ActiveCount > 0 )
  {
    return _GetNextIndex( -1 );
  }
  return m_TableCount;
}

template< typename key_T >
int MojoSet< key_T >::_GetNextIndex( int index ) const
{
  for( int i = index + 1; i < m_TableCount; ++i )
  {
    if( !m_Buffer[ i ].IsHashNull() )
    {
      return i;
    }
  }
  return m_TableCount;
}

template< typename key_T >
bool MojoSet< key_T >::_IsIndexValid( int index ) const
{
  return !m_Status && index < m_TableCount;
}

template< typename key_T >
key_T MojoSet< key_T >::_GetKeyAt( int index ) const
{
  return m_Buffer[ index ];
}

template< typename key_T >
key_T* MojoSet< key_T >::AllocAndConstruct( int new_buffer_count )
{
  key_T* new_buffer = ( key_T* )m_Alloc->Allocate( new_buffer_count * sizeof( key_T ), m_Name );
  if( new_buffer )
  {
    for( int i = 0; i < new_buffer_count; ++i )
    {
      new( new_buffer + i ) key_T();
    }
  }
  return new_buffer;
}

template< typename key_T >
void MojoSet< key_T >::DestructAndFree( key_T* old_buffer, int old_buffer_count )
{
  if( old_buffer )
  {
    for( int i = 0; i < old_buffer_count; ++i )
    {
      old_buffer[ i ].~key_T();
    }
    m_Alloc->Free( old_buffer );
  }
}

template< typename key_T >
void MojoSet< key_T >::ResizeTableInPlace( int old_table_count )
{
  if( m_TableCount != old_table_count )
  {
    for( int i = 0; i < old_table_count; ++i )
    {
      if( !m_Buffer[ i ].IsHashNull() )
      {
        Reinsert( i );
      }
    }
    for( int i = old_table_count; i < m_TableCount; ++i )
    {
      if( !m_Buffer[ i ].IsHashNull() )
      {
        Reinsert( i );
      }
      else
      {
        break;
      }
    }
  }
}

template< typename key_T >
void MojoSet< key_T >::CopyTable( key_T* old_table, int old_table_count )
{
  for( int i = 0; i < old_table_count; ++i )
  {
    if( !old_table[ i ].IsHashNull() )
    {
      Insert( old_table[ i ] );
    }
  }
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Resize( int new_table_count )
{
  if( m_Status )
  {
    return m_Status;
  }

  bool must_realloc = ( new_table_count > m_BufferCount ) ||
                      ( m_BufferCount > m_Config.m_BufferMinCount && m_Config.m_DynamicAlloc );

  if( must_realloc )
  {
    // Allocate new buffer, copy data over.
    if( !m_Config.m_DynamicAlloc || !m_Config.m_DynamicTable )
    {
      return kMojoStatus_CouldNotAlloc;
    }

    key_T* new_buffer = AllocAndConstruct( new_table_count );

    if( !new_buffer )
    {
      return kMojoStatus_CouldNotAlloc;
    }
    
    // Copy old table into new location.
    int old_table_count = m_TableCount;
    key_T* old_buffer = m_Buffer;

    m_TableCount = new_table_count;
    m_BufferCount = new_table_count;
    m_Buffer = new_buffer;
    m_ActiveCount = 0;
    CopyTable( old_buffer, old_table_count );
    DestructAndFree( old_buffer, old_table_count );
  }
  else
  {
    // Assert explanation: if m_DynamicTable is false, m_BufferCount anf m_TableCount will have been initialized
    // to m_BufferMinCount. So if we want to grow, must_realloc would have been set and we don't get to resize in
    // place. If we want to shrink, the Shrink() function would have tested and not even called Resize()
    assert( m_Config.m_DynamicTable );
    
    // No reallocation. Resize in place.
    int old_table_count = m_TableCount;
    m_TableCount = new_table_count;
    ResizeTableInPlace( old_table_count );
  }
  return  kMojoStatus_Ok;
}

template< typename key_T >
int MojoSet< key_T >::FindEmptyOrMatching( const key_T& key ) const
{
  int start_index = key.GetHash() % m_TableCount;
  
  // Look forward to the end of the key array
  for( int i = start_index; i < m_TableCount; ++i )
  {
    if( m_Buffer[ i ].IsHashNull() || m_Buffer[ i ] == key )
    {
      return i;
    }
  }
  
  // If not found, wrap around to the start
  for( int i = 0; i < start_index; ++i )
  {
    if( m_Buffer[ i ].IsHashNull() || m_Buffer[ i ] == key )
    {
      return i;
    }
  }
  
  return 0;
}

template< typename key_T >
int MojoSet< key_T >::FindEmpty( const key_T& key ) const
{
  int start_index = key.GetHash() % m_TableCount;
  
  // Look forward to the end of the key array
  for( int i = start_index; i < m_TableCount; ++i )
  {
    if( m_Buffer[ i ].IsHashNull() )
    {
      return i;
    }
  }
  
  // If not found, wrap around to the start
  for( int i = 0; i < start_index; ++i )
  {
    if( m_Buffer[ i ].IsHashNull() )
    {
      return i;
    }
  }
  
  return 0;
}

template< typename key_T >
void MojoSet< key_T >::Reinsert( int index )
{
  // Only move the entry if it is in the wrong place (due to collision)
  int new_index = FindEmptyOrMatching( m_Buffer[ index ] );
  if( new_index != index )
  {
    // Occupy new location
    m_Buffer[ new_index ] = m_Buffer[ index ];
    
    // Vacate old location
    m_Buffer[ index ] = key_T();
  }
}

template< typename key_T >
bool MojoSet< key_T >::RemoveOne( const key_T& key )
{
  if( key.IsHashNull() )
  {
    return false;
  }
  
  int index = FindEmptyOrMatching( key );
  if( m_Buffer[ index ].IsHashNull() )
  {
    return false;
  }
  else
  {
    // Clear the slot
    m_Buffer[ index ] = key_T();
    m_ActiveCount--;
    
    // Now fix up entries after this, that may have landed there after a hash collision.
    for( int i = index + 1; i < m_TableCount; ++i )
    {
      if( m_Buffer[ i ].IsHashNull() )
      {
        return true;
      }
      Reinsert( i );
    }
    for( int i = 0; i < index; ++i )
    {
      if( m_Buffer[ i ].IsHashNull() )
      {
        return true;
      }
      Reinsert( i );
    }
    return true;
  }
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Grow()
{
  // Make more room if table is getting crowded
  if( m_ActiveCount * 100 >= m_TableCount * kMojoTableGrowThreshold )
  {
    int new_size = m_TableCount * 2;
    if( !m_Config.m_DynamicAlloc && m_TableCount < m_BufferCount )
    {
      // Max out buffer before failing.
      new_size = MojoMin( new_size, m_BufferCount );
    }
    return Resize( new_size );
  }
  return kMojoStatus_Ok;
}

template< typename key_T >
MojoStatus MojoSet< key_T >::Shrink()
{
  // Shrink if it's getting too empty
  if( m_Config.m_DynamicTable && m_TableCount > kMojoTableMinCount
     && m_ActiveCount * 100 < m_TableCount * kMojoTableShrinkThreshold )
  {
    return Resize( m_TableCount / 2 );
  }
  return kMojoStatus_Ok;
}

template< typename key_T >
bool MojoSet< key_T >::Enumerate( const MojoCollector< key_T >& collector,
                                 const MojoAbstractSet< key_T >* limit ) const
{
  bool more = true;
  if( limit )
  {
    for( int i = _GetFirstIndex(); more && _IsIndexValid( i ); i = _GetNextIndex( i ) )
    {
      key_T key = _GetKeyAt( i );
      if( limit->Contains( key ) )
      {
        more = collector.Push( key );
      }
    }
  }
  else
  {
    for( int i = _GetFirstIndex(); more && _IsIndexValid( i ); i = _GetNextIndex( i ) )
    {
      more = collector.Push( _GetKeyAt( i ) );
    }
  }
  return more;
}

template< typename key_T >
int MojoSet< key_T >::_GetEnumerationCost() const
{
  return GetCount();
}

template< typename key_T >
int MojoSet< key_T >::_GetChangeCount() const
{
  return m_ChangeCount;
}

// ---------------------------------------------------------------------------------------------------------------

/**
 \ingroup group_container
 A macro to help you iterate over the keys in a hash table
 This macro will expand into a <tt>for(;;)</tt> statement that contains all the magic to visit all keys in the
 hash table. This macro works with MojoSet and MojoMap
 \param[in] container MojoSet or MojoMap by reference
 \param[out] key_variable Existing variable that will receive each key in the set or map in turn
 */
#define MojoForEachKey( container, key_variable ) \
for( int _i = ( container )._GetFirstIndex(); \
    ( container )._IsIndexValid( _i ) ? ( key_variable = ( container )._GetKeyAt( _i ), true ) : false; \
    _i = ( container )._GetNextIndex( _i ) )

// ---------------------------------------------------------------------------------------------------------------
