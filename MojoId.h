/*
 Copyright (c) 2013, Insomniac Games
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 \file
 \author Ron Pieket \n<http://www.ItShouldJustWorkTM.com> \n<http://twitter.com/RonPieket>
 */
/* MojoLib is documented at: http://www.itshouldjustworktm.com/mojolib/ */
#pragma once

// -- Standard Libs
#include <stdint.h>

// -- Mojo
#include "MojoUtil.h"

/**
 \class MojoId
 \ingroup group_id
 A general purpose string id class.

 A MojoId is a string-like identifier. A MojoId is stored as a 64-bit integer, and backed by a reference counted string
 dictionary. A MojoId very suitable for indexing a hash table.

 MojoId is used throughout MojoLib for several purposes.

 A MojoId object can be implicitly constructed from a C-string. Therefore, whenever an API calls for a MojoId
 (or, more commonly, for a <tt>const MojoId&</tt>), you may pass it a <tt>char*</tt>. A MojoId may also be tested for
 (in)equality with a C-string.
 For instance:
 \code
 void Foo( const MojoId& id )
 {
   if( id == "bar" )  // Equality with string
     printf( "Is it chocolate?\n" );
 }
 Foo( "bar" ); // String argument will be implicitly converted into MojoId
 \endcode
 */
class MojoId
{
public:
  /**
   Default constructor initializes MojoId to Null.
   */
  MojoId();
  /**
   Copy constructor. Needed to update internal reference counting.
   \param[in] other The other MojoId to copy.
   */
  MojoId( const MojoId& other );
  /**
   Construct from C-string.
   \param[in] c_string The C-string to store.
   \note The string is *copied* into the dictionary (unless it is already there), so strings in temporary buffers are
   fine.
   */
  MojoId( const char* c_string );
  /**
   Assignment operator. Needed to update internal reference counting.
   \param[in] other The other MojoId to copy.
   \return Standard `*this` for this type of operator.
   */
  MojoId& operator= ( const MojoId& other );
  /**
   Assignment from a C-string.
   \param[in] c_string The C-string to store.
   \note The string is *copied* into the dictionary (unless it is already there), so strings in temporary buffers are
   \return Standard `*this` for this type of operator.
   fine.
   */
  MojoId& operator= ( const char* c_string );
  /**
   Test equality.
   \param[in] other Other MojoId to compare
   \return true if equal.
   \note Hash table algorithm counts on this operator.
   */
  bool operator== ( const MojoId& other ) const;
  /**
   Test equality.
   \param[in] c_string String to compare
   \return true if equal.
   */
  bool operator== ( const char* c_string ) const;
  /**
   Test inequality.
   \param[in] other Other MojoId to compare
   \return true if different.
   */
  bool operator!= ( const MojoId& other ) const;
  /**
   Test inequality.
   \param[in] c_string String to compare
   \return true if different.
   */
  bool operator!= ( const char* c_string ) const;
  /**
   Test Null. A MojoId is considered Null if:
   - it has been constructed with the default constructor, and never assigned;
   - it was constructed from or assigned an empty C-string;
   - SetNull() was called
   \return true if Null.
   */
  bool IsNull() const;
  /**
   Set to Null.
   */
  void SetNull();

  ~MojoId();

  /**
   Convert to C-string.
   \return String from the dictionary.
   */
  const char* AsCString() const;
  /**
   Convert to 64-bit integer. This is the MojoFnv64() of the string this id this MojoId is currently associated with.
   \return The internal hash code.
   */
  uint64_t AsUint64() const;

  /**
   Find C-string in the backing dictionary for given hash code.
   You will only need this if you store the internal hash code a a plain uint64_t, not a MojoId object.
   \param[in] hash_code Hash code to find.
   \return Found C-string or NULL if not found.
   */
  static const char* FindCString( uint64_t hash_code );

  /**
   A MojoId that is Null. This is essentially the same as MojoId(), the default constructor.
   \return A Null MojoId.
   */
  static const MojoId& Null() { return s_Null; }

  /**
   Directly update reference count in backing dictionary.
   You will only need this if you store the internal hash code a a plain uint64_t, not a MojoId object.
   \param[in] hash_code Hash code of entry to update.
   */
  static void IncRefCount( uint64_t hash_code );

  /**
   Directly update reference count in backing dictionary.
   You will only need this if you store the internal hash code a a plain uint64_t, not a MojoId object.
   \param[in] hash_code Hash code of entry to update.
   */
  static void DecRefCount( uint64_t hash_code );

  /**
   Return the internal hash code.
   \return Internal hash code.
   \note Hash table algorithm counts on this function.
   */
  uint64_t GetHash() const { return AsUint64(); }

  /**
   Test for Null.
   This is used to mark an empty slot in the hash table.
   \return true if Null.
   \note Hash table algorithm counts on this function.
   */
  bool IsHashNull() const { return IsNull(); }

private:
  uint64_t  m_HashValue;
  static const MojoId s_Null;
};

// ---------------------------------------------------------------------------------------------------------------------
// Inline implementations

// default constructor
inline MojoId::MojoId()
{
  m_HashValue = 0;
}

inline bool MojoId::IsNull() const
{
  return !m_HashValue;
}

inline bool MojoId::operator!= ( const MojoId& other ) const
{
  return !( *this == other );
}

inline bool MojoId::operator!= ( const char* c_string ) const
{
  return !( *this == c_string );
}

inline bool MojoId::operator== ( const MojoId& other ) const
{
  return m_HashValue == other.m_HashValue;
}

inline bool MojoId::operator== ( const char* c_string ) const
{
  if( c_string )
    return m_HashValue == MojoFnv64( c_string );
  else
    return IsNull();
}

inline uint64_t MojoId::AsUint64() const
{
  return m_HashValue;
}

// destructor
inline MojoId::~MojoId()
{
  SetNull();
}
