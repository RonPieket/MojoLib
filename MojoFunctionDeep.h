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
#include "MojoMultiMap.h"

// ---------------------------------------------------------------------------------------------------------------

/**
 \class MojoFunctionDeep
 \ingroup group_function
 Contains all children of the keys in the input set. Input keys with no children have no effect.
 \image html Func-Inverse-Open-Shallow.png
 \note Enumeration may result in duplicate entries. Enumeration into a MojoSetCollector is recommended.
 */
template< typename key_T >
class MojoFunctionDeep : public MojoAbstractSet< key_T >
{
  class EnumMultiCollector final : public MojoCollector< key_T >
  {
  public:
    EnumMultiCollector( const MojoCollector< key_T >& collector, const MojoMultiMap< key_T, key_T >* multi_map,
                  const MojoAbstractSet< key_T >* limit )
    : m_Collector( collector )
    , m_MultiMap( multi_map )
    , m_Limit( limit )
    {}

    virtual bool Push( const key_T& key ) const override
    {
      const MojoSet< key_T >* values = m_MultiMap->Find( key );
      if( !values )
      {
        return true;
      }
      if( !values->Enumerate( m_Collector, m_Limit ) )
      {
        return false;
      }
      // The recursive part:
      return values->Enumerate( EnumMultiCollector( m_Collector, m_MultiMap, m_Limit ) );
    }

  private:
    const MojoCollector< key_T >&       m_Collector;
    const MojoMultiMap< key_T, key_T >* m_MultiMap;
    const MojoAbstractSet< key_T >*     m_Limit;
  };

  class TestMultiCollector final : public MojoCollector< key_T >
  {
  public:
    TestMultiCollector( const MojoMultiMap< key_T, key_T >* multi_map, const key_T& value )
    : m_Value( value )
    , m_MultiMap( multi_map )
    {}

    virtual bool Push( const key_T& key ) const override
    {
      const MojoSet< key_T >* values = m_MultiMap->Find( key );
      // Must return _false_ if found, true if not found (keep searching)
      if( !values )
      {
        return true;
      }
      if( values->Contains( m_Value ) )
      {
        return false;
      }
      // The recursive part:
      return values->Enumerate( TestMultiCollector( m_MultiMap, m_Value ) );
    }

  private:
    key_T m_Value;
    const MojoMultiMap< key_T, key_T >* m_MultiMap;
  };
  
  class EnumCollector final : public MojoCollector< key_T >
  {
  public:
    EnumCollector( const MojoCollector< key_T >& collector, const MojoMap< key_T, key_T >* map,
                  const MojoAbstractSet< key_T >* limit )
    : m_Collector( collector )
    , m_Map( map )
    , m_Limit( limit )
    {}

    virtual bool Push( const key_T& key ) const override
    {
      key_T value = m_Map->Find( key );
      while( !value.IsHashNull() )
      {
        if( !m_Limit || m_Limit->Contains( value ) )
        {
          if( !m_Collector.Push( value ) )
          {
            return false;
          }
        }
        value = m_Map->Find( value );
      }
      return true;
    }

  private:
    const MojoCollector< key_T >&   m_Collector;
    const MojoMap< key_T, key_T >*  m_Map;
    const MojoAbstractSet< key_T >* m_Limit;
  };

  class TestCollector final : public MojoCollector< key_T >
  {
  public:
    TestCollector( const MojoMap< key_T, key_T >* map, const key_T& value )
    : m_Value( value )
    , m_Map( map )
    {}

    virtual bool Push( const key_T& key ) const override
    {
      key_T value = m_Map->Find( key );
      // Must return _false_ if found, true if not found (keep searching)
      while( !value.IsHashNull() )
      {
        if( value == m_Value )
        {
          return false;
        }
        // The recursive part:
        value = m_Map->Find( value );
      }
      return true;
    }

  private:
    key_T m_Value;
    const MojoMap< key_T, key_T >* m_Map;
  };
  
public:
  MojoFunctionDeep( const MojoAbstractSet< key_T >* input_set, const MojoMultiMap< key_T, key_T >* multi_map )
  : m_InputSet( input_set )
  , m_MultiMap( multi_map )
  , m_Map( NULL )
  {}

  MojoFunctionDeep( const MojoAbstractSet< key_T >* input_set, const MojoMap< key_T, key_T >* map )
  : m_InputSet( input_set )
  , m_MultiMap( NULL )
  , m_Map( map )
  {}

  virtual bool Contains( const key_T& value ) const
  {
    if( m_MultiMap )
    {
      return !m_InputSet->Enumerate( TestMultiCollector( m_MultiMap, value ) );
    }
    else if( m_Map )
    {
      return !m_InputSet->Enumerate( TestCollector( m_Map, value ) );
    }
    return false;
  }

  virtual bool Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const
  {
    if( m_MultiMap )
    {
      return m_InputSet->Enumerate( EnumMultiCollector( collector, m_MultiMap, limit ) );
    }
    else if( m_Map )
    {
      return m_InputSet->Enumerate( EnumCollector( collector, m_Map, limit ) );
    }
    return true;
  }

  virtual int _GetEnumerationCost() const
  {
    if( m_MultiMap )
    {
      return m_MultiMap->_GetEnumerationCost();
    }
    else if( m_Map )
    {
      return m_Map->_GetEnumerationCost();
    }
    return 0;
  }

  virtual int _GetChangeCount() const
  {
    if( m_MultiMap )
    {
      return m_MultiMap->_GetChangeCount();
    }
    else if( m_Map )
    {
      return m_Map->_GetChangeCount();
    }
    return 0;
  }

private:

  const MojoAbstractSet< key_T >*     m_InputSet;
  const MojoMultiMap< key_T, key_T >* m_MultiMap;
  const MojoMap< key_T, key_T >*      m_Map;
};

// ---------------------------------------------------------------------------------------------------------------
