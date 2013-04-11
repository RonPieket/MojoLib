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
#include "UnitTest.h"

// -- Standard Libs
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

UnitTest::UnitTest( const char* name, const char* group )
{
  m_Name = name;
  m_Group = group;
  m_Done = false;
  m_Next = Anchor( this );
}

int UnitTest::Run( const char* group )
{
  int error_count = 0;
  if( group )
  {
    error_count += RunAllInGroup( group );
  }
  else
  {
    group = FindNextGroup( NULL );
    while( group )
    {
      error_count += RunAllInGroup( group );
      group = FindNextGroup( group );
    }
  }
  return error_count;
}

UnitTest* UnitTest::Anchor( UnitTest* next )
{
  static UnitTest* first = NULL;
  UnitTest* return_value = first;
  if( next )
  {
    first = next;
  }
  return return_value;
}

int UnitTest::RunAllInGroup( const char* group )
{
  printf( "GROUP %s\n", group );
  int error_count = 0;
  const char dashes[] = "------------------------------";
  
  // Because of (naughty) reliance of static constructor to register tests, the order in the list is out of my
  // control. A little extra code to make them run in alphabetical order.
  
  UnitTest* test = FindNext( group, NULL );
  while( test )
  {
    size_t offset = strlen( test->m_Name );
    if( offset >= sizeof( dashes ) )
    {
      offset = sizeof( dashes ) - 1;
    }
    printf( "  %s %s ", test->m_Name, dashes + offset );
    test->m_ErrorCount = 0;
    test->Test();
    if( test->m_ErrorCount == 0 )
    {
      printf( "pass\n" );
    }
    error_count += test->m_ErrorCount;
    test->m_Done = true;
    test = FindNext( group, test );
  }
  return error_count;
}

UnitTest* UnitTest::FindNext( const char* group, const UnitTest* last_done )
{
  UnitTest* best = NULL;
  UnitTest* test = Anchor();
  while( test )
  {
    if( !test->m_Done )
    {
      if( !group || !strcmp( group, test->m_Group ) )
      {
        bool higher_than_last = !last_done || strcmp( test->m_Name, last_done->m_Name ) > 0;
        bool lower_than_best = !best || strcmp( test->m_Name, best->m_Name ) < 0;
        if( higher_than_last && lower_than_best )
        {
          best = test;
        }
      }
    }
    test = test->m_Next;
  }
  return best;
}

const char* UnitTest::FindNextGroup( const char* last_group )
{
  const char* best = NULL;
  UnitTest* UnitTest = Anchor();
  while( UnitTest )
  {
    if( !UnitTest->m_Done )
    {
      bool higher_than_last = !last_group || strcmp( UnitTest->m_Group, last_group ) > 0;
      bool lower_than_best = !best || strcmp( UnitTest->m_Group, best ) < 0;
      if( higher_than_last && lower_than_best )
      {
        best = UnitTest->m_Group;
      }
    }
    UnitTest = UnitTest->m_Next;
  }
  return best;
}

void UnitTest::Error( const char* format, ... )
{
  if( m_ErrorCount == 0 )
  {
    printf( "FAIL\n" );
  }
  if( m_ErrorCount == 5 )
  {
    printf( "    ...\n" );
  }
  if( m_ErrorCount < 5 )
  {
    va_list args;
    va_start( args, format );
    vprintf( format, args );
    va_end( args );
  }

  m_ErrorCount += 1;
}

void UnitTest::ExpectString( const char* expect, const char* got, int line )
{
  bool success;
  if( !expect && !got )
  {
    success = true;
  }
  else if( !expect || !got )
  {
    success = false;
  }
  else
  {
    success = ( 0 == strcmp( expect, got ) );
  }
  if( !success )
  {
    if( !expect )
    {
      Error( "    %d: expected NULL but got \"%s\"\n", line, got );
    }
    else if( !got )
    {
      Error( "    %d: expected \"%s\" but got NULL\n", line, expect );
    }
    else
    {
      Error( "    %d: expected \"%s\" but got \"%s\"\n", line, expect, got );
    }
  }
}

void UnitTest::ExpectNotNull( const char* got, int line )
{
  bool success = ( got != NULL );
  if( !success )
  {
    Error( "    %d: expected a string but got NULL\n", line );
  }
}

void UnitTest::ExpectFloat( float expect, float got, int line )
{
  bool success = ( expect == got );
  if( !success )
  {
    Error( "    %d: expected %g but got %g\n", line, expect, got );
  }
}

void UnitTest::ExpectEpsilon( float expect, float got, int line )
{
  float expect_upper = expect * 1.0001f;
  float expect_lower = expect * 0.9999f;
  bool success = ( got >= expect_lower && got <= expect_upper );
  if( !success )
  {
    Error( "    %d: expected %g but got %g\n", line, expect, got );
  }
}

void UnitTest::ExpectInt( int expect, int got, int line )
{
  bool success = ( expect == got );
  if( !success )
  {
    Error( "    %d: expected %d but got %d\n", line, expect, got );
  }
}

void UnitTest::ExpectBool( bool expect, bool got, int line )
{
  bool success = ( expect == got );
  if( !success )
  {
    Error( "    %d: expected %s but got %s\n", line, expect ? "true" : "false", got ? "true" : "false" );
  }
}

RefCountedInt::~RefCountedInt()
{
  const char* error_message = NULL;
  switch( m_State )
  {
  case kState_Default:
    s_InfoConstructedCount -= 1;
    break;

  case kState_Assigned:
    s_InfoConstructedCount -= 1;
    s_InfoAssignedCount -= 1;
    break;

  case kState_Destructed:
    error_message = "Calling a destructor on an object that has already been destructed";
    break;

  default:
    error_message = "Calling a destructor on an object that was never constructed";
    break;
  }

  Error( error_message );

  m_State = kState_Destructed;
}

RefCountedInt::RefCountedInt()
  : m_Value( 0 )
  , m_State( kState_Default )
{
  s_InfoConstructedCount += 1;
}

RefCountedInt::RefCountedInt( int value )
  : m_Value( value )
  , m_State( kState_Assigned )
{
  s_InfoConstructedCount += 1;
  s_InfoAssignedCount += 1;
}

RefCountedInt::RefCountedInt( const RefCountedInt& other )
  : m_Value( other.m_Value )
  , m_State( other.m_State )
{
  const char* error_message = NULL;
  switch( m_State )
  {
  case kState_Default:
    s_InfoConstructedCount += 1;
    break;

  case kState_Assigned:
    s_InfoConstructedCount += 1;
    s_InfoAssignedCount += 1;
    break;

  case kState_Destructed:
    error_message = "Calling a copy constructor with an object that has already been destructed";
    break;

  default:
    error_message = "Calling a copy constructor with an object that was never constructed";
    break;
  }

  Error( error_message );
}

RefCountedInt& RefCountedInt::operator= ( const RefCountedInt& other )
{
  const char* error_message = NULL;
  switch( m_State )
  {
  case kState_Default:
    break;

  case kState_Assigned:
    s_InfoAssignedCount -= 1;
    break;

  case kState_Destructed:
    error_message = "Calling assignment operator on an object that has already been destructed";
    break;

  default:
    error_message = "Calling assignment operator on an object that was never constructed";
    break;
  }
  Error( error_message );

  m_Value = other.m_Value;
  m_State = other.m_State;

  error_message = NULL;
  switch( m_State )
  {
  case kState_Default:
    break;

  case kState_Assigned:
    s_InfoAssignedCount += 1;
    break;

  case kState_Destructed:
    error_message = "Calling assignment operator with an object that has already been destructed";
    break;

  default:
    error_message = "Calling assignment operator with an object that was never constructed";
    break;
  }
  Error( error_message );

  return *this;
}

int RefCountedInt::GetValue() const
{
  return m_Value;
}

bool RefCountedInt::IsNull() const
{
  return m_State == kState_Default;
}

void RefCountedInt::Error( const char* error_message )
{
  if( error_message )
  {
    if( !s_InfoErrorMessage )
    {
      s_InfoErrorMessage = error_message;
    }
    if( s_UseAssert )
    {
      Error( error_message );
    }
  }
}

void RefCountedInt::ClearInfo()
{
  s_InfoConstructedCount = 0;
  s_InfoAssignedCount = 0;
  s_InfoErrorMessage = NULL;
}


int RefCountedInt::s_InfoConstructedCount = 0;
int RefCountedInt::s_InfoAssignedCount = 0;
const char* RefCountedInt::s_InfoErrorMessage;

bool RefCountedInt::s_UseAssert = false;

// ---------------------------------------------------------------------------------------------------------------
