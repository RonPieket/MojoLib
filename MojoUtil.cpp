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

// -- Self
#include "MojoUtil.h"

// -- Standard Libs
#include <stdint.h>

/**
 As per http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
 */
#define kFnvPrimeU32 16777619
/**
 As per http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
 */
#define kFnvBasisU32 2166136261
/**
 As per http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
 */
#define kFnvPrimeU64 1099511628211ULL
/**
 As per http://www.isthe.com/chongo/tech/comp/fnv/#FNV-param
 */
#define kFnvBasisU64 14695981039346656037ULL

uint32_t MojoFnv32( const char* s )
{
  if( !s || s[ 0 ] == 0 )
  {
    return 0;
  }
  uint32_t hash = kFnvBasisU32;
  for( char c = *s++; c; c = *s++ )
  {
    hash = ( hash ^ c ) * kFnvPrimeU32;
  }
  hash = ( hash ^ '~' ) * kFnvPrimeU32;
  return hash;
}

uint32_t MojoFnv32( const char* s, int count )
{
  if( !s || s[ 0 ] == 0 )
  {
    return 0;
  }
  uint32_t hash = kFnvBasisU32;
  for( int i = 0; i < count; ++i )
  {
    hash = ( hash ^ s[ i ] ) * kFnvPrimeU32;
  }
  hash = ( hash ^ '~' ) * kFnvPrimeU32;
  return hash;
}

uint64_t MojoFnv64( const char* s )
{
  if( !s || s[ 0 ] == 0 )
  {
    return 0;
  }
  uint64_t hash = kFnvBasisU64;
  for( char c = *s++; c; c = *s++ )
  {
    hash = ( hash ^ c ) * kFnvPrimeU64;
  }
  hash = ( hash ^ '~' ) * kFnvPrimeU64;
  return hash;
}

uint64_t MojoFnv64( const char* s, int count )
{
  if( !s || s[ 0 ] == 0 )
  {
    return 0;
  }
  uint64_t hash = kFnvBasisU64;
  for( int i = 0; i < count; ++i )
  {
    hash = ( hash ^ s[ i ] ) * kFnvPrimeU64;
  }
  hash = ( hash ^ '~' ) * kFnvPrimeU64;
  return hash;
}

// ---------------------------------------------------------------------------------------------------------------
