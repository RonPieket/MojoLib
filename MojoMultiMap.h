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
#include "MojoStatus.h"
#include "MojoAlloc.h"

#include "MojoConfig.h"
#include "MojoUtil.h"
#include "MojoArray.h"
#include "MojoAbstractSet.h"
#include "MojoKeyValue.h"

/**
 \class MojoMultiMap
 \ingroup group_container
 A one-to-many hash table. Every key has one or more values. If a key is not present in the table, its value is
 not_found_value. The not_found_value is specified when the table is initialized.
 Also implements the MojoAbstractSet interface. As a MojoAbstractSet, the map work more like a set. That is, only
 the presence of keys is used.
 \see MojoForEachMultiValue
 \tparam key_T Key type. Must be hashable.
 \tparam value_T Value type.
 */
template< typename key_T, typename value_T >
class MojoMultiMap final : public MojoAbstractSet< key_T >
{
public:
  /**
   Shorthand specialization of MojoKeyValue.
   */
  typedef MojoKeyValue< key_T, value_T > KeyValue;
  
  /**
   Default constructor. You must call Create() before the map is ready for use.
   */
  MojoMultiMap()
  {
    Init();
  }
  
  /**
   Initializing constructor. No need to call Create().
   \param[in] name The name of the map. Will also be used for internal memory allocation.
   \param[in] not_found_value The value to return if nothing was found.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global default will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   */
  MojoMultiMap( const char* name, const value_T& not_found_value = value_T(), const MojoConfig* config = NULL,
               MojoAlloc* alloc = NULL )
  {
    Init();
    Create( name, not_found_value, config, alloc );
  }
  
  /**
   Create after default constructor or Destroy().
   \param[in] name The name of the map. Will also be used for internal memory allocation.
   \param[in] not_found_value The value to return if nothing was found.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global default will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \return Status code.
   */
  MojoStatus Create( const char* name, const value_T& not_found_value = value_T(),
                    const MojoConfig* config = NULL, MojoAlloc* alloc = NULL );
  
  /**
   Remove all entries and free all allocated buffers.
   */
  void Destroy();
  
  /**
   Remove all key-value pairs.
   */
  MojoStatus Clear();
  
  /**
   Insert key-value pair into the map. If key already exists in map, the value is overwritten.
   \param[in] key Key of the key-value pair to insert.
   \param[in] value Value of the key-value pair to insert.
   \return Status code.
   */
  MojoStatus Insert( const key_T& key, const value_T& value );
  
  /**
   Remove all key-value pairs with given key from the map.
   \param[in] key Key to remove.
   \return Status code.
   */
  MojoStatus Remove( const key_T& key );

  /**
   Remove key-value pair from the map.
   \param[in] key Key of the key-value pair to remove.
   \param[in] value Value of the key-value pair to remove.
   \return Status code.
   */
  MojoStatus Remove( const key_T& key, const value_T& value );
  
  /**
   Find set of values value that is associated with the key.
   */
  const MojoSet< value_T >* Find( const key_T& key ) const;
  
  /**
   Test presence of a key.
   \param[in] key Key to seach for.
   \return true if key is in the map.
   */
  virtual bool Contains( const key_T& key ) const override;

  /**
   Test presence of a key-value pair.
   \param[in] key Key of the key-value pair to seach for.
   \param[in] value Value of the key-value pair to seach for.
   \return true if key-value pair is in the map.
   */
  bool Contains( const key_T& key, const value_T& value ) const;
  
  /**
   Square bracket operator is an alias for Find()
   */
  const MojoSet< value_T >* operator[]( const key_T& key ) const { return Find( key ); }
  
  /**
   Return table status state. This is the only way to find out if something went wrong in the default constructor.
   If Create() was used, the returned status code will be the same.
   \return Status code.
   */
  MojoStatus GetStatus() const;
  
  /**
   Get number of keys in the map.
   \return Number of keys.
   */
  int GetCount() const;

  /**
   Get number of values associated with key.
   \param[in] key Key of the key-value pair to seach for.
   \return Number of valuess.
   */
  int GetValueCount( const key_T& key ) const;

  /**
   Return name of the map.
   \return Given name.
   */
  const char* GetName() const { return m_Name; }

  virtual bool Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;
  
  virtual ~MojoMultiMap();

private:

  MojoAlloc*          m_Alloc;
  const char*         m_Name;
  value_T             m_NotFoundValue;

  MojoMap< key_T, MojoSet< value_T >* > m_Map;

  int                 m_ChangeCount;
  MojoStatus          m_Status;
  MojoConfig          m_Config;

  void                Init();
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Init()
{
  m_Alloc = NULL;
  m_Name = NULL;
  m_ChangeCount = 0;
  m_Status = kMojoStatus_NotInitialized;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::GetStatus() const
{
  return m_Status;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Create( const char* name, const value_T& not_found_value,
                                                  const MojoConfig* config, MojoAlloc* alloc )
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

    m_Status = m_Map.Create( __FUNCTION__, NULL, config, alloc );
  }
  return m_Status;
}

template< typename key_T, typename value_T >
MojoMultiMap< key_T, value_T >::~MojoMultiMap()
{
  Destroy();
}

template< typename key_T, typename value_T >
void MojoMultiMap< key_T, value_T >::Destroy()
{
  key_T key;
  MojoForEachKey( m_Map, key )
  {
    MojoSet< value_T >* set = m_Map.Find( key );
    set->Destroy();
    set->~MojoSet< value_T >();
    m_Alloc->Free( set );
  }

  m_Map.Destroy();
  Init();
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Clear()
{
  key_T key;
  MojoForEachKey( m_Map, key )
  {
    MojoSet< value_T >* set = m_Map.Find( key );
    set->Destroy();
    set->~MojoSet< value_T >();
    m_Alloc->Free( set );
  }

  m_ChangeCount += 1;
  return m_Map.Clear();
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Insert( const key_T& key, const value_T& value )
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
      MojoSet< value_T >* set = m_Map.Find( key );
      if( !set )
      {
        set = ( MojoSet< value_T >* )m_Alloc->Allocate( sizeof( MojoSet< value_T > ), "SET OBJECT" );
        if( set )
        {
          set = new( set ) MojoSet< value_T >;
          set->Create( "SET PAYLOAD", &m_Config, m_Alloc );
          m_Map.Insert( key, set );
        }
        else
        {
          m_Status = kMojoStatus_CouldNotAlloc;
        }
      }
      if( set )
      {
        m_ChangeCount += 1;
        set->Insert( value );
      }
    }
  }
  return status;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Remove( const key_T& key )
{
  if( m_Status )
  {
    return m_Status;
  }
  else if( !key.IsHashNull() )
  {
    MojoSet< value_T >* set = m_Map.Remove( key );
    if( set )
    {
      m_ChangeCount += 1;
      set->Destroy();
      set->~MojoSet< value_T >();
      m_Alloc->Free( set );
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T, typename value_T >
MojoStatus MojoMultiMap< key_T, value_T >::Remove( const key_T& key, const value_T& value )
{
  if( m_Status )
  {
    return m_Status;
  }
  else if( !key.IsHashNull() )
  {
    MojoSet< value_T >* set = m_Map.Find( key );
    if( set )
    {
      m_ChangeCount += 1;
      MojoStatus status = set->Remove( value );
      if( set->GetCount() == 0 )
      {
        Remove( key );
      }
      return status;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename key_T, typename value_T >
const MojoSet< value_T >* MojoMultiMap< key_T, value_T >::Find( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    return m_Map.Find( key );
  }
  return NULL;
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::Contains( const key_T& key ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    return m_Map.Contains( key );
  }
  return false;
}

template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::Contains( const key_T& key, const value_T& value ) const
{
  if( !m_Status && !key.IsHashNull() )
  {
    const MojoSet< value_T >* set = m_Map.Find( key );
    if( set )
    {
      return set->Find( value );
    }
  }
  return false;
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::GetCount() const
{
  return m_Map->GetCount();
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::GetValueCount( const key_T& key ) const
{
  const MojoSet< value_T >* set = m_Map.Find( key );
  if( set )
  {
    return set->GetCount();
  }
  return 0;
}


template< typename key_T, typename value_T >
bool MojoMultiMap< key_T, value_T >::Enumerate( const MojoCollector< key_T >& collector,
                                               const MojoAbstractSet< key_T >* limit ) const
{
  return m_Map.Enumerate( collector, limit );
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetEnumerationCost() const
{
  return m_Map._GetEnumerationCost();
}

template< typename key_T, typename value_T >
int MojoMultiMap< key_T, value_T >::_GetChangeCount() const
{
  return m_ChangeCount;
}

// ---------------------------------------------------------------------------------------------------------------

