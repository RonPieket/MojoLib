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

// -- Standard Libs
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// -- MojoLib
#include "MojoLib.h"

// -- UnitTest
#include "UnitTest.h"

// -- Visual Studio
#if _MSC_VER
#define snprintf _snprintf_s
#endif

// ---------------------------------------------------------------------------------------------------------------

#ifndef ARRAY_SIZE
#define ARRAY_SIZE( r ) ( sizeof( r ) / sizeof( *( r ) ) )
#endif

// ---------------------------------------------------------------------------------------------------------------

static uint32_t Random()
{
  return ( rand() << 16 ) | rand();
}

// ---------------------------------------------------------------------------------------------------------------

MojoMap< MojoHashable< void* >, const char* > g_AllocName( "g_AllocName", NULL );

class CountingAlloc final : public MojoAlloc
{
public:
  CountingAlloc()
  : m_TotalAlloc( 0 )
  , m_ActiveAlloc( 0 )
  {}
  virtual void* Allocate( size_t byte_count, const char* name ) override
  {
    byte_count += -byte_count & 15; // Make 16-byte aligned
    m_TotalAlloc += 1;
    m_ActiveAlloc += 1;
    void* p = malloc( byte_count );
    g_AllocName.Insert( p, name );
    return p;
  }
  virtual void Free( void* p ) override
  {
    g_AllocName.Remove( p );
    m_ActiveAlloc -= 1;
    free( p );
  }
  void Print()
  {
    printf( "\n" );
    void* p;
    MojoForEachKey( g_AllocName, p )
    {
      const char* name = g_AllocName.Find( p );
      printf( "Alloc: %s\n", name );
    }
  }
  int m_TotalAlloc;
  int m_ActiveAlloc;
};


static CountingAlloc MyCountingAlloc;

// ---------------------------------------------------------------------------------------------------------------
// I'm using RefCountedInt for other unit tests. Better make sure the class is actually working.

REGISTER_UNIT_TEST( RefCountedIntTest, UnitTest )
{
  bool old_use_assert = RefCountedInt::s_UseAssert;
  RefCountedInt::s_UseAssert = false;
  RefCountedInt::s_InfoErrorMessage = NULL;
  
  {
    RefCountedInt a;
    a.~RefCountedInt();
    a = 1;
    EXPECT_NOT_NULL( RefCountedInt::s_InfoErrorMessage );
  }
  RefCountedInt::ClearInfo();
  
  // Test scoping
  {
    RefCountedInt a( 1 );
    RefCountedInt b;
    RefCountedInt c = a;
    EXPECT_STRING( NULL, RefCountedInt::s_InfoErrorMessage );
    EXPECT_INT( 3, RefCountedInt::s_InfoConstructedCount ); // a, b, c
    EXPECT_INT( 2, RefCountedInt::s_InfoAssignedCount );    // a, c
  }
  EXPECT_INT( 0, RefCountedInt::s_InfoConstructedCount );
  EXPECT_INT( 0, RefCountedInt::s_InfoAssignedCount );

  RefCountedInt::s_UseAssert = old_use_assert;
  RefCountedInt::ClearInfo();
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoArrayTest, Container )
{
  MojoArray< RefCountedInt > array;
  MojoStatus status = array.Create( __FUNCTION__, -1 );
  
  EXPECT_INT( kMojoStatus_Ok, status );
  EXPECT_INT( 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( 0, array.GetCount() );
  
  const int max_count = 1000;
  
  // Verify insertion, also correct constructor behavior. Note: scope is used to limit RefCountedInt life span
  for( int i = 0; i < max_count; ++i )
  {
    status = array.Push( i );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  EXPECT_INT( max_count + 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( max_count, array.GetCount() );
  
  // Verify value retrieval
  int sum = 0;
  for( int i = 0; i < array.GetCount(); ++i )
  {
    sum += array[ i ];
  }
  EXPECT_INT( ( max_count * ( max_count - 1 ) ) / 2, sum );
  
  // Verify shift
  sum = 0;
  while( array.GetCount() )
  {
    sum += array.Shift();
  }
  EXPECT_INT( ( max_count * ( max_count - 1 ) ) / 2, sum );
  
  // Verify unshift
  for( int i = 0; i < max_count; ++i )
  {
    array.Unshift( i );
  }
  EXPECT_INT( max_count + 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( max_count, array.GetCount() );
  
  // Verify pop
  sum = 0;
  while( array.GetCount() )
  {
    sum += array.Pop();
  }
  EXPECT_INT( ( max_count * ( max_count - 1 ) ) / 2, sum );
  
  // Verify that you can mix push and unshift
  array.Push( 3 );
  array.Unshift( 2 );
  array.Push( 4 );
  array.Unshift( 1 );
  EXPECT_INT( 1, array[ 0 ] );
  EXPECT_INT( 2, array[ 1 ] );
  EXPECT_INT( 3, array[ 2 ] );
  EXPECT_INT( 4, array[ 3 ] );
  
  // Verify shift and push in a single call (move first element to end)
  array.Push( array.Shift() );
  EXPECT_INT( 2, array[ 0 ] );
  EXPECT_INT( 3, array[ 1 ] );
  EXPECT_INT( 4, array[ 2 ] );
  EXPECT_INT( 1, array[ 3 ] );
  
  // Verify pop and unshift (last element to front)
  array.Unshift( array.Pop() );
  EXPECT_INT( 1, array[ 0 ] );
  EXPECT_INT( 2, array[ 1 ] );
  EXPECT_INT( 3, array[ 2 ] );
  EXPECT_INT( 4, array[ 3 ] );
  
  // Remove all elements
  array.Pop();
  array.Pop();
  array.Pop();
  array.Pop();
  
  // Should be back at beginning state.
  EXPECT_INT( 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( 0, array.GetCount() );
  
  array.Destroy();
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoArrayTestEdge, Container )
{
  const int max_count = 1000;
  const int small_count = 100;

  MojoConfig config;
  config.m_BufferMinCount = max_count;
  config.m_DynamicAlloc = false;

  MojoArray< RefCountedInt > array;
  MojoStatus status = array.Create( __FUNCTION__, -1, &config );
  EXPECT_INT( kMojoStatus_Ok, status );
  EXPECT_INT( 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( 0, array.GetCount() );

  // Fill it to the brim.
  for( int i = 0; i < max_count; ++i )
  {
    status = array.Push( i );
    EXPECT_INT( kMojoStatus_Ok, status );
  }

  EXPECT_INT( max_count + 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( max_count, array.GetCount() );

  // Add some more, should status
  for( int i = max_count; i < max_count + small_count; ++i )
  {
    status = array.Push( i );  // Does not change array.
    EXPECT_INT( kMojoStatus_CouldNotAlloc, status );
  }
  
  // Now rotate the (filled to the brim) array, by pushing what we shift
  for( int i = 0; i < small_count; ++i )
  {
    status = array.Push( array.Shift() );
    EXPECT_INT( kMojoStatus_Ok, status );
  }

  // Same in reverse: unshift what we pop
  for( int i = 0; i < small_count; ++i )
  {
    status = array.Unshift( array.Pop() );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  
  // Pushing what we pop should do nothing
  for( int i = 0; i < small_count; ++i )
  {
    status = array.Push( array.Pop() );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  
  // Also unshifting what we shift
  for( int i = 0; i < small_count; ++i )
  {
    status = array.Unshift( array.Shift() );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  
  // Array should be at its original filled state.
  for( int i = 0; i < max_count; ++i )
  {
    EXPECT_INT( i, array[ i ] );
  }

  EXPECT_INT( max_count + 1, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( max_count, array.GetCount() );

  array.Destroy();

  EXPECT_INT( 1, RefCountedInt::s_InfoAssignedCount );  // Remember not_found_value is still in there.
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );

  // Re-creating a destroyed object should work. (Note this time it's NOT restricted)
  status = array.Create( __FUNCTION__, -1 );
  EXPECT_INT( kMojoStatus_Ok, status );
  EXPECT_INT( 1, RefCountedInt::s_InfoAssignedCount );

  // Try some slicing and dicing.
  // Add some content.
  for( int i = 0; i < max_count; ++i )
  {
    status = array.Push( i );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  EXPECT_INT( max_count + 1, RefCountedInt::s_InfoAssignedCount );

  int value = array.Remove( small_count );
  EXPECT_INT( small_count, value ); // Removed element should be returned
  EXPECT_INT( max_count - 1, array.GetCount() );
  EXPECT_INT( max_count, RefCountedInt::s_InfoAssignedCount );

  // Verify new content
  for( int i = 0; i < max_count - 1; ++i )
  {
    int expect = i;
    if( i >= small_count )
    {
      expect += 1;
    }
    EXPECT_INT( expect, array[ i ] );
  }
  EXPECT_INT( -1, array[ max_count - 1 ] ); // This should no longer be there (-1 is not_found_value)
  
  array.Destroy();

  EXPECT_INT( 1, RefCountedInt::s_InfoAssignedCount );  // Remember not_found_value is still in there.
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoMapTest, Container )
{
  MojoMap< MojoHash< uint32_t >, RefCountedInt > map;
  map.Create( __FUNCTION__, RefCountedInt() );
  uint32_t key;
  
  EXPECT_INT( 0, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( 0, map.GetCount() );
  
  const int key_max_count = kMojoBufferMinCount * 2;
  uint32_t keys[ key_max_count ];
  
  // Generate keys
  for( int i = 0; i < key_max_count; ++i )
  {
    key = Random();
    keys[ i ] = key;
  }
  
  // Verify insertion, also correct constructor behavior.
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    EXPECT_INT( kMojoStatus_Ok, map.Insert( key, 5 ) );
  }
  EXPECT_INT( key_max_count, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( key_max_count, map.GetCount() );
  
  // Verify value retrieval
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    EXPECT_INT( 5, map.Find( key ).GetValue() );
  }
  
  // Verify find failure
  key = 1;
  EXPECT_TRUE( map.Find( key ).IsNull() );
  
  // Reinsert keys, different values. Should have no effect on key count
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    EXPECT_INT( kMojoStatus_Ok, map.Insert( key, RefCountedInt( i ) ) );
  }
  EXPECT_INT( key_max_count, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( key_max_count, map.GetCount() );
  
  // Verify that changes values are correct
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    EXPECT_INT( i, map.Find( key ).GetValue() );
  }
  
  // Verify key iterator
  int iteration_count = 0;
  MojoForEachKey( map, key )
  {
    iteration_count += 1;
  }
  EXPECT_INT( key_max_count, iteration_count );

  // Verify removal
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    map.Remove( key );
  }
  EXPECT_INT( 0, RefCountedInt::s_InfoAssignedCount );
  EXPECT_INT( 0, map.GetCount() );

  map.Destroy();
  EXPECT_INT( 1, RefCountedInt::s_InfoConstructedCount );
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoSetTest, Container )
{
  MojoSet< MojoHash< uint32_t > > set;
  uint32_t key;
  MojoStatus status;
  
  status = set.Create( __FUNCTION__ );
  EXPECT_INT( kMojoStatus_Ok, status );
  
  EXPECT_INT( 0, set.GetCount() );
  
  const int key_max_count = 1000;
  uint32_t keys[ key_max_count ];
  
  // Generate keys
  for( int i = 0; i < key_max_count; ++i )
  {
    key = Random();
    keys[ i ] = key;
  }
  
  // Insert the keys, verify set count
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    status = set.Insert( key );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  EXPECT_INT( key_max_count, set.GetCount() );
  
  // Reinsert the same keys (should have no effect on set count)
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    status = set.Insert( key );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  EXPECT_INT( key_max_count, set.GetCount() );
  
  // Verify Contains() returns true for all inserted key values
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    EXPECT_TRUE( set.Contains( key ) );
  }
  
  // Verify Contains() returns false for other key values
  for( int i = 0; i < key_max_count; ++i )
  {
    key = Random();
    EXPECT_FALSE( set.Contains( key ) );
  }
  
  // Verify Remove() is cool to call for non-existent key values
  for( int i = 0; i < key_max_count; ++i )
  {
    key = Random();
    status = set.Remove( key );
    EXPECT_INT( kMojoStatus_NotFound, status );
  }
  
  // Verify iterator
  int iteration_count = 0;
  MojoForEachKey( set, key )
  {
    iteration_count += 1;
  }
  EXPECT_INT( key_max_count, iteration_count );
  
  // Verify removal
  for( int i = 0; i < key_max_count; ++i )
  {
    key = keys[ i ];
    status = set.Remove( key );
    EXPECT_INT( kMojoStatus_Ok, status );
  }
  EXPECT_INT( 0, set.GetCount() );
  
  set.Destroy();
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoSetTestString, Container )
{
  MojoSet< MojoHashableCString > set( __FUNCTION__ );
  
  // Verify insertion
  set.Insert( "Apple" );
  set.Insert( "Banana" );
  set.Insert( "Orange" );
  set.Insert( "Kiwi" );
  EXPECT_INT( 4, set.GetCount() );
  
  // Verify re-insertion
  set.Insert( "Apple" );
  set.Insert( "Banana" );
  EXPECT_INT( 4, set.GetCount() );
  
  // Verify removal
  set.Remove( "Banana" );
  set.Remove( "Kiwi" );
  EXPECT_INT( 2, set.GetCount() );
  
  // Verify retrieval
  EXPECT_TRUE( set.Contains( "Apple" ) );
  EXPECT_FALSE( set.Contains( "Banana" ) );
  EXPECT_TRUE( set.Contains( "Orange" ) );
  EXPECT_FALSE( set.Contains( "Kiwi" ) );
  
  set.Destroy();
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
}

// ---------------------------------------------------------------------------------------------------------------

static MojoId MakeId( const char* group, int number )
{
  char buffer[ 20 ];
  snprintf( buffer, sizeof buffer, "%s%d", group, number );
  return MojoId( buffer );
}

REGISTER_UNIT_TEST( MojoRelationTest, Container )
{
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );

  MojoRelation< MojoId, MojoId > rel( "test" );

  static const int n = 1000;

  // For first half, insert consecutively
  for( char c = 'a'; c <= 'z'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    for( int i = 0; i < n / 2; ++i )
    {
      MojoId child = MakeId( group, i );
      rel.InsertChildParent( child, parent );
    }
  }
  EXPECT_INT( 26 * n / 2, rel.GetCount() );

  // For second half, insert jumbled up
  for( int i = n / 2; i < n; ++i )
  {
    for( char c = 'z'; c >= 'a'; --c )
    {
      char group[ 20 ];
      snprintf( group, sizeof group, "%c", c );
      MojoId parent = group;
      MojoId child = MakeId( group, i );
      rel.InsertChildParent( child, parent );
    }
  }
  EXPECT_INT( 26 * n, rel.GetCount() );

  // Now remove all even children, to exercise reordering.
  for( char c = 'a'; c <= 'z'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    for( int i = 0; i < n; i += 2 )
    {
      MojoId child = MakeId( group, i );
      rel.RemoveChild( child );
    }
  }
  EXPECT_INT( 26 * n / 2, rel.GetCount() );

  // Now remove all parents above and including 'q'.
  for( char c = 'q'; c <= 'z'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    for( int i = 1; i < n; i += 2 )
    {
      MojoId child = MakeId( group, i );
      rel.RemoveChild( child );
    }
  }
  EXPECT_INT( 16 * n / 2, rel.GetCount() );

  // Now table contains parents 'a'-'p', and only oddly numbered children

  // Find all children's parents
  for( char c = 'a'; c < 'q'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    for( int i = 1; i < n; i += 2 )
    {
      MojoId child = MakeId( group, i );
      const MojoSet< MojoId >* parents = rel.FindParents( child );
      EXPECT_TRUE( parents != NULL );
      if( parents )
      {
        EXPECT_INT( 1, parents->GetCount() );
        MojoId found_parent = parents->GetOnlyElement();
        EXPECT_STRING( parent.AsCString(), found_parent.AsCString() );
      }
    }
  }
  
  // Verify all even children are missing
  for( char c = 'a'; c < 'q'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    for( int i = 0; i < n; i += 2 )
    {
      MojoId child = MakeId( group, i );
      const MojoSet< MojoId >* parents = rel.FindParents( child );
      EXPECT_TRUE( parents == NULL );
    }
  }
  
  // Verify children of groups 'q'-'z' are gone as well
  for( char c = 'q'; c <= 'z'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    for( int i = 0; i < n; i += 1 )
    {
      MojoId child = MakeId( group, i );
      const MojoSet< MojoId >* parents = rel.FindParents( child );
      EXPECT_TRUE( parents == NULL );
    }
  }

  // Find children of parents
  for( char c = 'a'; c < 'q'; ++c )
  {
    char group[ 20 ];
    snprintf( group, sizeof group, "%c", c );
    MojoId parent = group;
    MojoId child;

    int count = 0;
    const MojoSet< MojoId >* children = rel.FindChildren( parent );
    if( children )
    {
      MojoForEachKey( *children, child )
      {
        count += 1;
        EXPECT_INT( c, child.AsCString()[ 0 ] );
      }
    }
    EXPECT_INT( n / 2, count );
  }

  rel.Destroy();
  EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoSetTestStatus, Config )
{
  // Test invalid arguments
  {
    MojoConfig config;
    config.m_BufferMinCount = 0;

    MojoSet< MojoHashable< uint32_t > > set;
    MojoStatus status = set.Create( __FUNCTION__, &config );
    EXPECT_INT( kMojoStatus_InvalidArguments, status );
    
    set.Destroy();
    EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
  }

  // Test overfilling the set
  {
    const int key_max_count = 1000;

    MojoConfig config;
    config.m_BufferMinCount = key_max_count;
    config.m_DynamicAlloc = false;

    MojoSet< MojoHashable< uint32_t > > set;
    MojoStatus status = set.Create( __FUNCTION__, &config );
    EXPECT_INT( kMojoStatus_Ok, status );
    
    // Now fill the whole table. This should work.
    int max_fit = key_max_count * kMojoTableGrowThreshold / 100;
    for( int i = 0; i < max_fit; ++i )
    {
      status = set.Insert( i + 1 );
      EXPECT_INT( kMojoStatus_Ok, status );
    }

    // More won't fit. Should not crash but report status.
    for( int i = max_fit; i < max_fit + 10; ++i )
    {
      status = set.Insert( i + 1 );
      EXPECT_INT( kMojoStatus_CouldNotAlloc, status );
    }

    // Verify that set is still functional.
    for( int i = 0; i < max_fit; ++i )
    {
      status = set.Remove( i + 1 );
      EXPECT_INT( kMojoStatus_Ok, status );
    }
    
    // Verify keys are really gone.
    for( int i = 0; i < max_fit; ++i )
    {
      EXPECT_FALSE( set.Contains( i + 1 ) );
    }
    
    set.Destroy();
    EXPECT_INT( 0, MyCountingAlloc.m_ActiveAlloc );
  }
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoIdTest, Id )
{
  // Test MojoId scoping and reference counting
  {
    MojoId s1 = "s1";
    MojoId s2 = "s2";
    MojoId s3 = s1; // Should bump ref count, not string count
    
    EXPECT_STRING( "s1", s1.AsCString() );
    EXPECT_STRING( "s2", s2.AsCString() );
    EXPECT_STRING( "s1", s3.AsCString() );
    EXPECT_INT( 2, g_MojoIdManager.GetCount() );
  }
  EXPECT_INT( 0, g_MojoIdManager.GetCount() );
  
  // Test direct reference counting
  uint64_t hash1 = 0;
  {
    MojoId s1 = "s1";
    hash1 = s1.AsUint64();
    MojoId::IncRefCount( hash1 );
    EXPECT_STRING( "s1", MojoId::FindCString( hash1 ) );
    EXPECT_INT( 1, g_MojoIdManager.GetCount() );
  }
  EXPECT_INT( 1, g_MojoIdManager.GetCount() );
  MojoId::DecRefCount( hash1 );
  EXPECT_INT( 0, g_MojoIdManager.GetCount() );
  EXPECT_STRING( NULL, MojoId::FindCString( hash1 ) );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoIdMapTest, Id )
{
  MojoMap< MojoId, int > map;
  map.Create( __FUNCTION__, 0 );

  const int key_max_count = 10;
  uint32_t keys[ key_max_count ];
  char transient_string[ 100 ];
  
  for( int i = 0; i < key_max_count; ++i )
  {
    keys[ i ] = Random();
  }
  
  // Verify insertion
  for( int i = 0; i < key_max_count; ++i )
  {
    snprintf( transient_string, sizeof( transient_string ), "String %08x", keys[ i ] );
    map.Insert( transient_string, i );
  }
  EXPECT_INT( key_max_count, map.GetCount() );
  
  // Verify that reinsertion doesn't change anything
  for( int i = 0; i < key_max_count; ++i )
  {
    snprintf( transient_string, sizeof( transient_string ), "String %08x", keys[ i ] );
    map.Insert( transient_string, i );
  }
  EXPECT_INT( key_max_count, map.GetCount() );
  
  // Verify that the map has correctly incremented the string ref count
  EXPECT_INT( key_max_count, g_MojoIdManager.GetCount() );
  
  for( int i = 0; i < key_max_count; ++i )
  {
    snprintf( transient_string, sizeof( transient_string ), "String %08x", keys[ i ] );
    EXPECT_INT( i, map.Find( transient_string ) );
  }
  
  map.Destroy();
  
  // Verify that the map has correctly decremented the string ref count
  EXPECT_INT( 0, g_MojoIdManager.GetCount() );
}

// ---------------------------------------------------------------------------------------------------------------

static int CountSet( const MojoAbstractSet< MojoId >* set )
{
  MojoArray< MojoId > output( "result" );
  set->Enumerate( MojoArrayCollector< MojoId >( &output ) );
  return output.GetCount();
}


class StringFilter final : public MojoAbstractSet< MojoId >
{
public:
  StringFilter( const char* test_str )
  : m_TestStr( test_str )
  {}
  virtual bool Contains( const MojoId& id ) const override
  {
    return NULL != strstr( id.AsCString(), m_TestStr );
  }
  
  virtual bool Enumerate( const MojoCollector< MojoId >&, const MojoAbstractSet< MojoId >* ) const override
  {
    return false;
  }
  virtual int _GetEnumerationCost() const override { return INT_MAX; }
  virtual int _GetChangeCount() const override { return 0; }
private:
  const char* m_TestStr;
};

REGISTER_UNIT_TEST( MojoBooleanTest, Boolean )
{
  MojoSet< MojoId > human_powered( "Human Powered" );
  MojoSet< MojoId > motorized( "Motorized" );
  MojoSet< MojoId > two_wheels( "2 Wheels" );
  MojoSet< MojoId > four_wheels( "4 Wheels" );
  MojoSet< MojoId > more_wheels( "More Wheels" );

  human_powered.Insert( "American Flyer" );
  human_powered.Insert( "Bicycle" );
  human_powered.Insert( "Kayak" );
  human_powered.Insert( "Rickshaw" );

  motorized.Insert( "Motorcycle" );
  motorized.Insert( "18 Wheeler" );
  motorized.Insert( "Yacht" );
  motorized.Insert( "Sedan" );
  motorized.Insert( "Helicopter" );
  motorized.Insert( "Pickup Truck" );
  motorized.Insert( "SUV" );
  motorized.Insert( "Golf Cart" );
  motorized.Insert( "Electric Scooter" );

  two_wheels.Insert( "Motorcycle" );
  two_wheels.Insert( "Bicycle" );
  two_wheels.Insert( "Rickshaw" );
  two_wheels.Insert( "Electric Scooter" );

  four_wheels.Insert( "American Flyer" );
  four_wheels.Insert( "Sedan" );
  four_wheels.Insert( "Pickup Truck" );
  four_wheels.Insert( "SUV" );
  four_wheels.Insert( "Golf Cart" );

  more_wheels.Insert( "18 Wheeler" );

  StringFilter string_filter_er( "er" );

  MojoUnion< MojoId > two_four_wheels_set( &four_wheels, &two_wheels );
  MojoUnion< MojoId > any_wheels_set( &two_wheels, &four_wheels, &more_wheels );
  MojoUnion< MojoId > all_vehicles_set( &human_powered, &motorized );
  MojoIntersection< MojoId > vehicles_with_er( &all_vehicles_set, &string_filter_er );
  MojoIntersection< MojoId > motorized_four_wheels( &motorized, &four_wheels );
  MojoIntersection< MojoId > motorized_two_wheels( &motorized, &two_wheels );
  MojoDifference< MojoId > motorized_no_wheels( &motorized, &any_wheels_set );
  MojoIntersection< MojoId > human_powered_four_wheels( &human_powered, &four_wheels );
  MojoIntersection< MojoId > human_powered_two_wheels( &human_powered, &two_wheels );
  MojoDifference< MojoId > human_powered_no_wheels( &human_powered, &any_wheels_set );
  MojoIntersection< MojoId > all_vehicles_four_wheels( &all_vehicles_set, &four_wheels );
  MojoIntersection< MojoId > all_vehicles_two_wheels( &all_vehicles_set, &two_wheels );
  MojoDifference< MojoId > all_vehicles_no_wheels( &all_vehicles_set, &any_wheels_set );

  EXPECT_INT( 4, CountSet( &motorized_four_wheels ) );      // "Sedan", "SUV", "Golf Cart", "Pickup Truck"
  EXPECT_INT( 2, CountSet( &motorized_two_wheels ) );       // "Electric Scooter", "Motorcycle"
  EXPECT_INT( 2, CountSet( &motorized_no_wheels ) );        // "Yacht", "Helicopter"
  EXPECT_INT( 1, CountSet( &human_powered_four_wheels ) );  // "American Flyer"
  EXPECT_INT( 2, CountSet( &human_powered_two_wheels ) );   // "Rickshaw", "Bicycle"
  EXPECT_INT( 1, CountSet( &human_powered_no_wheels ) );    // "Kayak"
  EXPECT_INT( 5, CountSet( &all_vehicles_four_wheels ) );   // "Sedan", "SUV", "American Flyer",
  // "Golf Cart", "Pickup Truck"
  EXPECT_INT( 4, CountSet( &all_vehicles_two_wheels ) );    // "Electric Scooter", "Rickshaw", "Bicycle",
  // "Motorcycle"
  EXPECT_INT( 3, CountSet( &all_vehicles_no_wheels ) );     // "Kayak", "Yacht", "Helicopter"
  EXPECT_INT( 4, CountSet( &vehicles_with_er ) );           // "American Flyer", "18 Wheeler", "Electric Scooter",
  // "Helicopter"
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoEqualityTest, Boolean )
{
  MojoSet< MojoId > first( "first" );
  MojoSet< MojoId > second( "second" );
  MojoSet< MojoId > equal_to_first( "equal_to_first" );
  MojoSet< MojoId > super_of_first( "super_of_first" );

  first.Insert( "one" );
  first.Insert( "two" );
  first.Insert( "three" );

  equal_to_first.Insert( "one" );
  equal_to_first.Insert( "two" );
  equal_to_first.Insert( "three" );

  super_of_first.Insert( "one" );
  super_of_first.Insert( "two" );
  super_of_first.Insert( "three" );
  super_of_first.Insert( "four" );

  second.Insert( "un" );
  second.Insert( "deux" );
  second.Insert( "trois" );

  EXPECT_TRUE( MojoAreEquivalent( &first, &first ) );
  EXPECT_TRUE( MojoAreEquivalent( &first, &equal_to_first ) );
  EXPECT_FALSE( MojoAreEquivalent( &first, &super_of_first ) );
  EXPECT_FALSE( MojoAreEquivalent( &super_of_first, &first ) );
  EXPECT_TRUE( MojoIsSubsetOf( &first, &super_of_first ) );
  EXPECT_TRUE( MojoIsSupersetOf( &super_of_first, &first ) );
  EXPECT_FALSE( MojoIsSubsetOf( &second, &super_of_first ) );
  EXPECT_FALSE( MojoIsSupersetOf( &super_of_first, &second ) );
}

// ---------------------------------------------------------------------------------------------------------------

REGISTER_UNIT_TEST( MojoFunctionTest, Function )
{
  MojoMultiMap< MojoId, MojoId > multi_map( "multi_map" );
  MojoSet< MojoId > input_set( "input_set" );
  MojoSet< MojoId > verify_deep( "verify_deep" );
  MojoSet< MojoId > verify_shallow( "verify_shallow" );

  MojoFunctionDeep< MojoId > fn_deep( &input_set, &multi_map );
  MojoFunction< MojoId, MojoId > fn_shallow( &input_set, &multi_map );

  multi_map.Insert( "A", "A1" );  // A -> (A1,A2,A3)
  multi_map.Insert( "A", "A2" );
  multi_map.Insert( "A", "A3" );

  multi_map.Insert( "B", "B1" );  // B -> (B1,B2,B3)
  multi_map.Insert( "B", "B2" );
  multi_map.Insert( "B", "B3" );

  multi_map.Insert( "C", "C1" );  // C -> (C1,C2,C3)
  multi_map.Insert( "C", "C2" );
  multi_map.Insert( "C", "C3" );

  multi_map.Insert( "A2", "A2x" );  // A2 -> (A2x)
  multi_map.Insert( "C3", "C3x" );  // C3 -> (C3x,C3y)
  multi_map.Insert( "C3", "C3y" );

  input_set.Insert( "A" );
  input_set.Insert( "C" );

  const char* all[] =
  { "A",   "B",   "C",   "A1", "A2", "A3", "B1",  "B2",  "B3",  "C1", "C2", "C3", "A2x", "C3x", "C3y" };
  bool result_deep[] =
  { false, false, false, true, true, true, false, false, false, true, true, true, true,  true,  true  };
  bool result_shallow[] =
  { false, false, false, true, true, true, false, false, false, true, true, true, false, false, false };

  fn_deep.Enumerate( MojoSetCollector< MojoId >( &verify_deep ) );
  fn_shallow.Enumerate( MojoSetCollector< MojoId >( &verify_shallow ) );

  for( int i = 0; i < ARRAY_SIZE( all ); ++i )
  {
    MojoId id = all[ i ];
    EXPECT_BOOL( result_deep[ i ],    verify_deep.Contains( id ) );
    EXPECT_BOOL( result_shallow[ i ], verify_shallow.Contains( id ) );
    EXPECT_BOOL( result_deep[ i ],    fn_deep.Contains( id ) );
    EXPECT_BOOL( result_shallow[ i ], fn_shallow.Contains( id ) );
  }
}

// ---------------------------------------------------------------------------------------------------------------

int main( int argc, const char** argv )
{
  int error_count = 0;
  
  g_MojoIdManager.Create();

  MojoConfig config;
  MojoAlloc::SetDefault( &MyCountingAlloc );
  MojoConfig::SetDefault( &config );

  MyCountingAlloc.m_TotalAlloc = 0;
  MyCountingAlloc.m_ActiveAlloc = 0;
  
  printf( "--------------------\n" );
  error_count += UnitTest::Run();
  printf( "--------------------\n" );
  printf( "%d issue%s found\n", error_count, error_count == 1 ? "" : "s" );

  g_MojoIdManager.Destroy();

  return error_count;
}

// ---------------------------------------------------------------------------------------------------------------
