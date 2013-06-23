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

/** \cond HIDE_FORWARD_REFERENCE */
template< typename value_T > class MojoArray;
template< typename value_T > class MojoCollector;
/** \endcond */

/**
 \interface MojoAbstractSet
 \ingroup group_set_common
 Interface for all sets.
 */
template< typename key_T >
class MojoAbstractSet
{
public:
  /**
   Test if a key is an element of the set.
   */
  virtual bool Contains( const key_T& key ) const = 0;
  /**
   Push all keys into the collector object.
   */
  virtual bool Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const = 0;
  /**
   Used internally to optimize enumeration.
   \private
   */
  virtual int _GetEnumerationCost() const = 0;
  /**
   Used internally to accomodate caching.
   \private
   */
  virtual int _GetChangeCount() const = 0;
  virtual ~MojoAbstractSet() {}
};

// ---------------------------------------------------------------------------------------------------------------

/**
 \class MojoTestCollector
 Specialization of MojoCollector, with MojoSet as receiver.
 */
template< typename value_T >
class MojoTestCollector final : public MojoCollector< value_T >
{
public:
  /**
   Construct from a MojoSet pointer.
   \param[in] set The set to receive data.
   */
  MojoTestCollector( const MojoAbstractSet< value_T >* set )
  : m_Set( set )
  {}
  virtual bool Push( const value_T& value ) const override
  {
    return m_Set->Contains( value );
  }
private:
  const MojoAbstractSet< value_T >* m_Set;
};

template< typename key_T >
bool MojoSubsetOf( const MojoAbstractSet< key_T >* first, const MojoAbstractSet< key_T >* second )
{
  MojoTestCollector< key_T > collector( second );
  return first->Enumerate( collector );
}

// ---------------------------------------------------------------------------------------------------------------

template< typename key_T >
bool MojoSupersetOf( const MojoAbstractSet< key_T >* first, const MojoAbstractSet< key_T >* second )
{
  return MojoSubsetOf( second, first );
}

// ---------------------------------------------------------------------------------------------------------------

template< typename key_T >
bool MojoEqualTo( const MojoAbstractSet< key_T >* first, const MojoAbstractSet< key_T >* second )
{
  return MojoSubsetOf( first, second ) && MojoSubsetOf( second, first );
}

// ---------------------------------------------------------------------------------------------------------------

