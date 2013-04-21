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
#include <string.h>

/**
 \file MojoUtil.h
 Bits and pieces that make hashing easier.
 */

/**
 \ingroup group_util
 A modified FNV-1a algorithm.
 The modification consists of the processing of two additional '+' characters at the end of the string. This
 improves distribution of series of strings that only differ by their last character, such as "string1",
 "string2".
 <br>Also, the function returns zero if the input pointer is NULL or the length is 0.
 \param s The zero terminated string to hash.
 \return A hash code.
 */
uint32_t MojoFnv32( const char* s );
/**
 \ingroup group_util
 A modified FNV-1a algorithm.
 The modification consists of the processing of two additional '+' characters at the end of the string. This
 improves distribution of series of strings that only differ by their last character, such as "string1",
 "string2".
 <br>Also, the function returns zero if the input pointer is NULL or the length is 0.
 \param s The string to hash.
 \param count The number of characters.
 \return A hash code.
 */
uint32_t MojoFnv32( const char* s, int count );
/**
 \ingroup group_util
 A modified FNV-1a algorithm.
 The modification consists of the processing of two additional '+' characters at the end of the string. This
 improves distribution of series of strings that only differ by their last character, such as "string1",
 "string2".
 <br>Also, the function returns zero if the input pointer is NULL or the length is 0.
 \param s The zero terminated string to hash.
 \return A hash code.
 */
uint64_t MojoFnv64( const char* s );
/**
 \ingroup group_util
 A modified FNV-1a algorithm.
 The modification consists of the processing of two additional '+' characters at the end of the string. This
 improves distribution of series of strings that only differ by their last character, such as "string1",
 "string2".
 <br>Also, the function returns zero if the input pointer is NULL or the length is 0.
 \param s The string to hash.
 \param count The number of characters.
 \return A hash code.
 */
uint64_t MojoFnv64( const char* s, int count );

/**
 \ingroup group_util
 Substitute for std::max. Something in the libraries we use here at Insomniac causes a compile error if I use
 std::max
 \param[in] a First value
 \param[in] b Second value
 \return The larger of a and b.
 */
template< typename T >
T MojoMax( const T& a, const T& b ) { return a > b ? a : b; }

/**
 \ingroup group_util
 Substitute for std::min. Something in the libraries we use at Insomniac causes a compile error if I use std::min
 \param[in] a First value
 \param[in] b Second value
 \return The smaller of a and b.
 */
template< typename T >
T MojoMin( const T& a, const T& b ) { return a <= b ? a : b; }

/**
 \ingroup group_util
 If your keys are unique integers and also well-distributed, for example ( 0xc94f1aa2, 0x278a827f, 0x18f12203 ),
 *and never zero*, the MojoHash wrapper template will endow it with the required members to be used as a key_T.
 The value you provide will be used as-is. Integer value 0 will be considered Null.
 
 The MojoHash wrapper template has an implicit constructor and a cast operator to make the wrapper nearly
 invisible, other than in the container declaration.

 \code{.cpp}
 MojoSet< MojoHash< int > > set( "set" ); // Here you see it...

 int keys[ 6 ];
 for( int i = 0; i < 3; ++i )
 {
   keys[ i ] = rand();
   set.Insert( keys[ i ] );               // ...and here you don't
 }
 for( int i = 0; i < 3; ++i )
 {
   printf( "%s ", set.Contains( keys[ i ] ) ? "yes" : "no" );
 }
 printf( "\n" );
 \endcode
 
 This will print: `yes yes yes`
 */
template< typename T >
class MojoHash
{
public:
  /**
   Default constructor will initialize a Null hash code
   \note Hash table algorithm requires this constructor to mark empty slots in hash table.
   */
  MojoHash()
    : m_Key( 0 )
  {}

  /**
   Construct implicitly from type T
   */
  MojoHash( T key )
    : m_Key( key )
  {}

  /**
   Test equality.
   \param[in] other Value to compare
   \return true if equal.
   \note Hash table algorithm requires this operator.
   */
  bool operator== ( const MojoHash& other ) const
  {
    return m_Key == other.m_Key;
  }

  /**
   Implicitly convert to type T.
   */
  operator T() const
  {
    return m_Key;
  }

  /**
   Return the unaltered value.
   \return Same value as the object was initialized with.
   \note Hash table algorithm requires this function.
   */
  uint64_t GetHash() const
  {
    return m_Key;
  }

  /**
   Test for Null.
   Value 0 means Null, used to mark an empty slot in the hash table.
   \return true if Null.
   \note Hash table algorithm requires this function.
   */
  bool IsHashNull() const
  {
    return m_Key == 0;
  }

private:
  T m_Key;
};

/**
 \ingroup group_util
 If your keys are unique integers but not uniformly distributed, for example ( 1, 2, 3, 101, 102, 103 ), *and
 never zero*, the MojoHashable wrapper template will make it conform to the requirements for use as a key_T.
 
 The MojoHashable wrapper template also has an implicit constructor and a cast operator to make the wrapper nearly
 invisible.
 
 \code{.cpp}
 MojoSet< MojoHashable< int > > set( "set" ); // Here you see it...

 for( int i = 0; i < 3; ++i )
 {
   set.Insert( i );                           // ...and here you don't
 }
 for( int i = 0; i < 6; ++i )
 {
   printf( "%s ", set.Contains( i ) ? "yes" : "no" );
 }
 printf( "\n" );
 \endcode
 
 This will print: `no yes yes no no no`
 
 \note Observe the first `no`: value 0 cannot be used as a key.
 */
template< typename T >
class MojoHashable
{
public:
  /**
   Default constructor will initialize a Null hash code
   \note Hash table algorithm requires this constructor to mark empty slots in hash table.
   */
  MojoHashable()
  : m_Key( 0 )
  {}
  
  /**
   Construct implicitly from type T
   */
  MojoHashable( T key )
  : m_Key( key )
  {}
  
  /**
   Test equality.
   \param[in] other Value to compare
   \return true if equal.
   \note Hash table algorithm requires this operator.
   */
  bool operator== ( const MojoHashable& other ) const
  {
    return m_Key == other.m_Key;
  }
  
  /**
   Automatically convert to type T.
   */
  operator T() const
  {
    return m_Key;
  }
  
  /**
   Compute hash code.
   \return MojoFnv64( &value, sizeof( value ) )
   \note Hash table algorithm requires this function.
   */
  uint64_t GetHash() const
  {
    return MojoFnv64( ( const char* )&m_Key, sizeof( m_Key ) );
  }
  
  /**
   Test for Null.
   Value 0 means Null, used to mark an empty slot in the hash table.
   \return true if Null.
   \note Hash table algorithm requires this function.
   */
  bool IsHashNull() const
  {
    return m_Key == 0;
  }
  
private:
  T m_Key;
};

/**
 \ingroup group_util
 If you don't want to use MojoId, but your keys are C-strings, the MojoHashableCString wrapper template can make
 that work. This will store the pointer to the string, not the string body itself. You must make sure that the
 string body is kept in a safe place. The C-string may not be empty or NULL.
 
 The MojoHashableCString wrapper template also has an implicit constructor and a cast operator to make the wrapper
 nearly invisible.
 
 \code{.cpp}
 MojoSet< MojoHashableCString > set( "set" ); // Here you see it...

 set.Insert( "one" );                         // ...and here you don't
 set.Insert( "two" );
 set.Insert( "three" );
 printf( "%s ", set.Contains( "two" ) ? "yes" : "no" );
 printf( "%s ", set.Contains( "four" ) ? "yes" : "no" );
 printf( "%s ", set.Contains( "six" ) ? "yes" : "no" );
 printf( "\n" );
 \endcode
 This will print: `yes no no`
 */
class MojoHashableCString
{
public:
  /**
   Default constructor will initialize a Null hash code
   \note Hash table algorithm requires this constructor to mark empty slots in hash table.
   */
  MojoHashableCString()
  : m_Key( NULL )
  {}

  /**
   Construct implicitly from const char*
   */
  MojoHashableCString( const char* c_string )
  : m_Key( c_string )
  {}

  /**
   Test equality.
   \param[in] other Value to compare
   \return true if equal.
   \note Hash table algorithm requires this operator.
   */
  bool operator== ( const MojoHashableCString& other ) const
  {
    return 0 == strcmp( m_Key, other.m_Key );
  }

  /**
   Implicitly convert to const char*.
   */
  operator const char*() const
  {
    return m_Key;
  }
  
  /**
   Compute hash code.
   \return MojoFnv64( value )
   \note Hash table algorithm requires this function.
   */
  uint64_t GetHash() const
  {
    return MojoFnv64( m_Key );
  }

  /**
   Test for Null.
   NULL means Null, used to mark an empty slot in the hash table.
   \note Hash table algorithm requires this function.
   \return true if Null.
   */
  bool IsHashNull() const
  {
    return m_Key == NULL || m_Key[ 0 ] == '\0';
  }

private:
  const char* m_Key;
};

// ---------------------------------------------------------------------------------------------------------------
