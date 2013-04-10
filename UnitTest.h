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

#include <string.h>

/**
 Unit testing is not part of MojoLib.
 \private
 */
class UnitTest
{
public:
  UnitTest( const char* name, const char* group = "misc" );
  virtual ~UnitTest() {}
  
  static int Run( const char* group = NULL );
  virtual void Test() = 0;

protected:
  void ExpectString( const char* expect, const char* got, int line );
  void ExpectNotNull( const char* got, int line );
  void ExpectFloat( float expect, float got, int line );
  void ExpectEpsilon( float expect, float got, int line );
  void ExpectInt( int expect, int got, int line );
  void ExpectBool( bool expect, bool got, int line );
  
private:
  void Error( const char* format, ... );

  static UnitTest* Anchor( UnitTest* next = NULL );
  static int RunAllInGroup( const char* group );
  static UnitTest* FindNext( const char* group, const UnitTest* last_done );
  static const char* FindNextGroup( const char* last_group );
  
  UnitTest*   m_Next;
  const char* m_Name;
  const char* m_Group;
  bool        m_Done;
  int         m_ErrorCount;
};

#define EXPECT_STRING( expect, got ) ExpectString( expect, got, __LINE__ );
#define EXPECT_NOT_NULL( got ) ExpectNotNull( got, __LINE__ );
#define EXPECT_FLOAT( expect, got ) ExpectFloat( expect, got, __LINE__ );
#define EXPECT_EPSILON( expect, got ) ExpectEpsilon( expect, got, __LINE__ );
#define EXPECT_INT( expect, got ) ExpectInt( expect, got, __LINE__ );
#define EXPECT_BOOL( expect, got ) ExpectBool( expect, got, __LINE__ );
#define EXPECT_TRUE( got ) ExpectBool( true, got, __LINE__ );
#define EXPECT_FALSE( got ) ExpectBool( false, got, __LINE__ );

#define REGISTER_UNIT_TEST( name, group ) \
class name final : public UnitTest        \
{                                         \
public:                                   \
name() : UnitTest( #name, #group ) {}     \
virtual void Test() override;             \
};                                        \
name g_##name;                            \
void name::Test()

//--------------------------------------------------------------------------------------------------
/**
 Class to test correct constructor/destructor usage in unit tests
 \private
 */
class RefCountedInt
{
public:
  ~RefCountedInt();
  RefCountedInt();
  RefCountedInt( int value );
  RefCountedInt( const RefCountedInt& other );
  RefCountedInt& operator= ( const RefCountedInt& other );
  operator int() const { return GetValue(); }
  int GetValue() const;
  bool IsNull() const;

  static void ClearInfo();

  static int s_InfoConstructedCount;
  static int s_InfoAssignedCount;
  static const char* s_InfoErrorMessage;

  static bool s_UseAssert;

private:
  enum State
  {
    kState_Default    = 0x5beb0aed,  // Default constructor called, no value assigned
    kState_Assigned   = 0x11f7c79d,  // Value assigned (either through non-default constructor, copy constructor, or assignment operator)
    kState_Destructed = 0xd1264cb6,  // Destructor called
  };
  int     m_Value;
  State   m_State;
  
  static void Error( const char* error_message );
};

// ---------------------------------------------------------------------------------------------------------------
