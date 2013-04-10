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

// Standard Libs
#include <limits.h>

// -- Mojo
#include "MojoStatus.h"
#include "MojoConfig.h"
#include "MojoAlloc.h"
#include "MojoAbstractSet.h"
#include "MojoCollector.h"

/**
 \class MojoArray
 \ingroup group_container
 An array that can grow and shrink; elements can be added or removed at either end.
 \tparam value_T Value type.
 */
template< typename value_T >
class MojoArray final : public MojoAbstractSet< value_T >
{
public:
  /**
   Default constructor does not allocate any memory. Array cannot be used until Create() has been called.
   */
  MojoArray()
  {
    Init();
  }
  /**
   Initializing constructor allocates resources and prepares array for use.
   \param[in] name The name of the array. This will be passed to the allocator.
   \param[in] not_found_value Value to be returned if index was out of range.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array If dynamic memory allocation is not desired, specify an array of `value_T` to use.
   \param[in] fixed_array_count Size of `fixed_array`.
   */
  MojoArray( const char* name, const value_T& not_found_value = value_T(), const MojoConfig* config = NULL,
            MojoAlloc* alloc = NULL, value_T* fixed_array = NULL, int fixed_array_count = 0 )
  {
    Init();
    Create( name, not_found_value, config, alloc, fixed_array, fixed_array_count );
  }
  
  /**
   Allocate resources and prepare array for use.
   \param[in] name The name of the array. This will be passed to the allocator.
   \param[in] not_found_value Value to be returned if index was out of range.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array If dynamic memory allocation is not desired, specify an array of `value_T` to use.
   \param[in] fixed_array_count Size of `fixed_array`.
   \return Status code
   */
  MojoStatus Create( const char* name, const value_T& not_found_value = value_T(),
                    const MojoConfig* config = NULL, MojoAlloc* alloc = NULL, value_T* fixed_array = NULL,
                    int fixed_array_count = 0 );

  /**
   Return table status state. This is the only way to find out if something went wrong in the default constructor.
   If Create() was used, the returned status code will be the same.
   \return Status code.
   */
  MojoStatus GetStatus() const;
  
  ~MojoArray();
  
  /**
   Release all resources.
   */
  void Destroy();
  
  /**
   Remove all elements from the array.
   */
  MojoStatus Clear();

  /**
   Append value at the end of the array.
   \param[in] value Value to append.
   \return Status code. Failure may occur of dynamic allocation was disabled and the array is full.
   */
  MojoStatus Push( const value_T& value );

  /**
   Insert value at the front of the array.
   This will be the new index 0, and all other elements move up by one position.
   \param[in] value Value to insert.
   \return Status code. Failure may occur of dynamic allocation was disabled and the array is full.
   */
  MojoStatus Unshift( const value_T& value );

  /**
   Remove value from the end of the array.
   \return The removed value. If array was empty the not_found_value will be returned.
   */
  value_T Pop();

  /**
   Remove value from the front of the array.
   The remaining elements move down by one position.
   \return The removed value. If array was empty the not_found_value will be returned.
   */
  value_T Shift();

  /**
   Insert element at the indicated position.
   Elements after the position will shift up by one.
   \param[in] index The index into the array.
   If index is negative, it is from the end otf the array. For example, -1 indicates the last element.
   \param[in] value The value to insert.
   \return Status code. Failure may occur of dynamic allocation was disabled and the array is full.
   */
  MojoStatus Insert( int index, const value_T& value );

  /**
   Remove a single element from any point in the array.
   The elements after will move down by one position.
   \param[in] index The index into the array.
   If index is negative, it is from the end otf the array. For example, -1 indicates the last element.
   \return The removed value. If index was out of range, the not_found_value will be returned.
   */
  value_T Remove( int index );

  /**
   Remove multiple elements from any point in the array.
   The elements after will move down.
   \param[in] index The index into the array.
   If index is negative, it is from the end otf the array. For example, -1 indicates the last element.
   \param[in] count the number of elements to remove. If omitted, all elements from `index` to the end of the
   array will be removed.
   \return Status code.
   */
  MojoStatus RemoveRange( int index, int count = INT_MAX );

  /**
   Return the number of elements in the array.
   \return The number of elements in the array.
   */
  int GetCount() const;

  /**
   Replace a single element.
   \param[in] index Position in the array.
   If index is negative, it is from the end otf the array. For example, -1 indicates the last element.
   \param[in] value The new value.
   \return The old value.
   */
  value_T SwapAt( int index, const value_T& value ) const;

  /**
   Return a single element.
   \param[in] index Position in the array.
   If index is negative, it is from the end otf the array. For example, -1 indicates the last element.
   \return The value at the indicated position. If index was out of range, the not_found_value is returned.
   */
  value_T GetAt( int index ) const;

  /**
   Alias for MojoArray::GetAt()
   */
  value_T operator[]( int index ) const { return GetAt( index ); }

  /**
   Return the name given at creation.
   \return The name.
   */
  const char* GetName() const { return m_Name; }

  /**
   Test presence of a value.
   \param[in] value The value to look for.
   \return true if value is present
   \warning This is a performance hazard. The array is simply scanned. Use only with small arrays, or where
   performance is not an issue.
   */
  virtual bool Contains( const value_T& value ) const override;

  virtual void Enumerate( const MojoCollector< value_T >& collector,
                         const MojoAbstractSet< value_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;
  
private:

  MojoAlloc*          m_Alloc;
  const char*         m_Name;
  value_T*            m_Buffer;
  value_T             m_NotFoundValue;

  int                 m_StartIndex;
  int                 m_ActiveCount;
  int                 m_BufferCount;       // Entries allocated
  int                 m_ChangeCount;
  MojoStatus          m_Status;
  MojoConfig          m_Config;

  void Init();
  MojoStatus Resize( int new_capacity );
  MojoStatus Grow( int count = 1 );
  MojoStatus Shrink();
  void Move( int from_index, int to_index, int count, bool clear );
  value_T* AllocAndConstruct( int new_buffer_count );
  void DestructAndFree( value_T* old_buffer, int old_buffer_count );
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

/**
 \class MojoArrayCollector
 Specialization of MojoCollector, with MojoArray as receiver.
 */
template< typename value_T >
class MojoArrayCollector final : public MojoCollector< value_T >
{
public:
  /**
   Construct from a MojoArray pointer.
   \param[in] array The array to receive data.
   */
  MojoArrayCollector( MojoArray< value_T >* array )
  : m_Array( array )
  {}
  virtual void Push( const value_T& value ) const override
  {
    m_Array->Push( value );
  }
private:
  MojoArray< value_T >* m_Array;
};

template< typename value_T >
void MojoArray< value_T >::Init()
{
  m_Alloc = NULL;
  m_Name = NULL;
  m_Buffer = NULL;
  m_StartIndex = 0;
  m_BufferCount = 0;
  m_ActiveCount = 0;
  m_ChangeCount = 0;
  m_Status = kMojoStatus_NotInitialized;
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Create( const char* name, const value_T& not_found_value,
                                        const MojoConfig* config, MojoAlloc* alloc, value_T* fixed_array,
                                        int fixed_array_count )
{
  if( !config )
  {
    config = MojoConfig::GetDefault();
  }
  if( !alloc )
  {
    alloc = MojoAlloc::GetDefault();
  }
  if( config->m_BufferMinCount < kMojoTableMinCount )
  {
    m_Status = kMojoStatus_InvalidArguments;
  }
  else
  {
    m_Name            = name;
    m_Alloc           = alloc;
    m_Config          = *config;
    m_NotFoundValue   = not_found_value;

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
    }
    
    m_Status = m_Buffer ? kMojoStatus_Ok : kMojoStatus_CouldNotAlloc;
  }
  return m_Status;
}

template< typename value_T >
MojoArray< value_T >::~MojoArray()
{
  Destroy();
}

template< typename value_T >
MojoStatus MojoArray< value_T >::GetStatus() const
{
  return m_Status;
}

template< typename value_T >
void MojoArray< value_T >::Destroy()
{
  if( m_Alloc )
  {
    DestructAndFree( m_Buffer, m_BufferCount );
  }
  Init();
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Clear()
{
  for( int i = 0; i < m_ActiveCount; ++i )
  {
    int index = ( m_StartIndex + i ) % m_BufferCount;
    m_Buffer[ index ] = value_T();
  }
  m_StartIndex = 0;
  m_ActiveCount = 0;
  m_ChangeCount += 1;
  return Resize( m_Config.m_BufferMinCount );
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Insert( int index, const value_T& value )
{
  MojoStatus status = m_Status;
  if( !status )
  {
    status = Grow();
    if( !status )
    {
      // Negative index means "from the end". Calculate positive equivalent.
      index = ( index + m_ActiveCount ) % m_ActiveCount;

      int start_count = index;
      int end_count = m_ActiveCount - index;
      if( end_count < start_count )
      {
        // Mode the end portion.
        Move( index, index + 1, end_count, false );
        int i = ( m_StartIndex + i ) % m_BufferCount;
        m_Buffer[ i ] = value;
      }
      else
      {
        // Move the start
        Move( 0, -1, start_count, false );
        m_StartIndex = ( m_StartIndex - 1 + m_BufferCount ) % m_BufferCount;
        int i = ( m_StartIndex + i ) % m_BufferCount;
        m_Buffer[ i ] = value;
      }
      m_ActiveCount += 1;
      m_ChangeCount += 1;
    }
  }
  return status;
}

template< typename value_T >
value_T MojoArray< value_T >::Remove( int index )
{
  if( index > m_ActiveCount || index < -m_ActiveCount )
  {
    return m_NotFoundValue;
  }
  // Negative index means "from the end". Calculate positive equivalent.
  index = ( index + m_ActiveCount ) % m_ActiveCount;
  value_T return_value = GetAt( index );
  RemoveRange( index, 1 );
  m_ChangeCount += 1;
  return return_value;
}

template< typename value_T >
void MojoArray< value_T >::Move( int from_index, int to_index, int count, bool clear )
{
  // Avoid trouble with mod operation when moving below index 0.
  from_index += m_BufferCount;
  to_index += m_BufferCount;

  if( to_index < from_index )
  {
    // Move down
    for( int i = 0; i < count; ++i )
    {
      int to = ( m_StartIndex + to_index + i ) % m_BufferCount;
      int from = ( m_StartIndex + from_index + i ) % m_BufferCount;
      m_Buffer[ to ] = m_Buffer[ from ];
    }
    if( clear )
    {
      for( int i = to_index + count; i < from_index + count; ++i )
      {
        int index = ( m_StartIndex + i ) % m_BufferCount;
        m_Buffer[ index ] = value_T();
      }
    }
  }
  else if( to_index > from_index )
  {
    // Move up
    for( int i = count - 1; i >= 0; --i )
    {
      int to = ( m_StartIndex + to_index + i ) % m_BufferCount;
      int from = ( m_StartIndex + from_index + i ) % m_BufferCount;
      m_Buffer[ to ] = m_Buffer[ from ];
    }
    if( clear )
    {
      for( int i = from_index; i < to_index; ++i )
      {
        int index = ( m_StartIndex + i ) % m_BufferCount;
        m_Buffer[ index ] = value_T();
      }
    }
  }
}

template< typename value_T >
MojoStatus MojoArray< value_T >::RemoveRange( int index, int count )
{
  if( index >= m_ActiveCount || index < -m_ActiveCount )
  {
    return kMojoStatus_IndexOutOfRange;
  }
  if( count == 0 )
  {
    return kMojoStatus_Ok;
  }
  // Calculate positive index.
  index = ( index + m_ActiveCount ) % m_ActiveCount;
  // count == INT_MAX means "to the end"
  count = MojoMin( count, m_ActiveCount - index );

  // Decide whether it is more efficient to move the start or the end of the array.
  int start_count = index;
  int end_count = m_ActiveCount - index - count;
  if( end_count < start_count )
  {
    // Mode the end portion.
    Move( index + count, index, end_count, true );
    m_ActiveCount -= count;
    m_ChangeCount += 1;
  }
  else
  {
    // Move the start portion.
    Move( 0, count, start_count, true );
    m_StartIndex = ( m_StartIndex + count ) % m_BufferCount;
    m_ActiveCount -= count;
    m_ChangeCount += 1;
  }
  return Shrink();
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Push( const value_T& value )
{
  MojoStatus status = m_Status;
  if( !status )
  {
    status = Grow();
    if( !status )
    {
      int index = ( m_StartIndex + m_ActiveCount ) % m_BufferCount;
      m_Buffer[ index ] = value;
      m_ActiveCount += 1;
      m_ChangeCount += 1;
    }
  }
  return status;
}

template< typename value_T >
value_T MojoArray< value_T >::Pop()
{
  if( !m_ActiveCount || m_Status )
  {
    return m_NotFoundValue;
  }
  int index = ( m_StartIndex + m_ActiveCount - 1 + m_BufferCount ) % m_BufferCount;
  value_T value = m_Buffer[ index ];
  m_Buffer[ index ] = value_T();
  m_ActiveCount -= 1;
  m_ChangeCount += 1;
  Shrink();
  return value;
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Unshift( const value_T& value )
{
  MojoStatus status = m_Status;
  if( !status )
  {
    status = Grow();
    if( !status )
    {
      int index = ( m_StartIndex - 1 + m_BufferCount ) % m_BufferCount;
      m_Buffer[ index ] = value;
      m_StartIndex = index;
      m_ActiveCount += 1;
      m_ChangeCount += 1;
    }
  }  
  return status;
}

template< typename value_T >
value_T MojoArray< value_T >::Shift()
{
  if( !m_ActiveCount || m_Status )
  {
    return m_NotFoundValue;
  }
  int index = m_StartIndex;
  value_T value = m_Buffer[ index ];
  m_Buffer[ index ] = value_T();
  m_StartIndex = ( m_StartIndex + 1 ) % m_BufferCount;
  m_ActiveCount -= 1;
  m_ChangeCount += 1;
  Shrink();
  return value;
}

template< typename value_T >
int MojoArray< value_T >::GetCount() const
{
  return m_ActiveCount;
}

template< typename value_T >
value_T MojoArray< value_T >::GetAt( int index ) const
{
  if( !m_Status && index < m_ActiveCount && index >= -m_ActiveCount )
  {
    index = ( index + m_ActiveCount ) % m_ActiveCount;
    return m_Buffer[ ( index + m_StartIndex ) % m_BufferCount ];
  }
  else
  {
    return m_NotFoundValue;
  }
}

template< typename value_T >
value_T* MojoArray< value_T >::AllocAndConstruct( int new_buffer_count )
{
  value_T* new_buffer = ( value_T* )m_Alloc->Allocate( new_buffer_count * sizeof( value_T ), m_Name );
  if( new_buffer )
  {
    for( int i = 0; i < new_buffer_count; ++i )
    {
      new( new_buffer + i ) value_T();
    }
  }
  return new_buffer;
}

template< typename value_T >
void MojoArray< value_T >::DestructAndFree( value_T* old_buffer, int old_buffer_count )
{
  if( old_buffer )
  {
    for( int i = 0; i < old_buffer_count; ++i )
    {
      old_buffer[ i ].~value_T();
    }
    m_Alloc->Free( old_buffer );
  }
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Resize( int new_capacity )
{
  if( m_Status )
  {
    return m_Status;
  }

  if( !m_Config.m_DynamicAlloc || !m_Config.m_DynamicTable )
  {
    return kMojoStatus_CouldNotAlloc;
  }

  value_T* new_buffer = AllocAndConstruct( new_capacity );

  if( !new_buffer )
  {
    return kMojoStatus_CouldNotAlloc;
  }

  // Copy used portion from old to new array
  for( int i = 0; i < m_ActiveCount; ++i )
  {
    new_buffer[ i ] = GetAt( i );
  }

  DestructAndFree( m_Buffer, m_BufferCount );
  m_Buffer = new_buffer;
  m_BufferCount = new_capacity;
  m_StartIndex = 0;
  
  return kMojoStatus_Ok;
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Grow( int count )
{
  if( GetCount() + count > m_BufferCount )
  {
    int new_buffer_count = m_BufferCount * 2;
    while( GetCount() + count > new_buffer_count )
    {
      new_buffer_count *= 2;
    }
    return Resize( new_buffer_count );
  }
  return kMojoStatus_Ok;
}

template< typename value_T >
MojoStatus MojoArray< value_T >::Shrink()
{
  // Less than 25% full
  if( m_Config.m_DynamicAlloc &&
     GetCount() * 100 < m_BufferCount * 25 &&
     m_BufferCount > m_Config.m_BufferMinCount )
  {
    return Resize( m_BufferCount / 2 );
  }
  return kMojoStatus_Ok;
}

template< typename value_T >
bool MojoArray< value_T >::Contains( const value_T& value ) const
{
  for( int i = 0; i < m_ActiveCount; ++i )
  {
    if( GetAt( i ) == value )
    {
      return true;
    }
  }
  return false;
}

template< typename value_T >
void MojoArray< value_T >::Enumerate( const MojoCollector< value_T >& collector,
                                     const MojoAbstractSet< value_T >* limit ) const
{
  if( limit )
  {
    for( int i = 0; i < m_ActiveCount; ++i )
    {
      value_T value = GetAt( i );
      if( limit->Contains( value ) )
      {
        collector.Push( value );
      }
    }
  }
  else
  {
    for( int i = 0; i < m_ActiveCount; ++i )
    {
      collector.Push( GetAt( i ) );
    }
  }
}

template< typename value_T >
int MojoArray< value_T >::_GetEnumerationCost() const
{
  return GetCount() / 2;
}

template< typename value_T >
int MojoArray< value_T >::_GetChangeCount() const
{
  return m_ChangeCount;
}

// ---------------------------------------------------------------------------------------------------------------
