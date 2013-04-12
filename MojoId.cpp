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
#include "MojoId.h"

// -- Mojo
#include "MojoIdManager.h"

const MojoId MojoId::s_Null = MojoId();

// copy constructor
MojoId::MojoId( const MojoId& other )
{
  m_HashValue = other.m_HashValue;
  if( m_HashValue )
  {
    IncRefCount( m_HashValue );
  }
}

MojoId::MojoId( const char* c_string )
{
  m_HashValue = g_MojoIdManager.Insert( c_string );
}

MojoId& MojoId::operator= ( const MojoId& other )
{
  if( m_HashValue != other.m_HashValue )
  {
    if( !other.IsNull() ) IncRefCount( other.m_HashValue );
    if( !IsNull() )       DecRefCount( m_HashValue );
    m_HashValue = other.m_HashValue;
  }
  return *this;
}

MojoId& MojoId::operator= ( const char* c_string )
{
  uint64_t old_hash_value = m_HashValue;
  m_HashValue = g_MojoIdManager.Insert( c_string );
  DecRefCount( old_hash_value );
  return *this;
}

void MojoId::SetNull()
{
  if( m_HashValue )
  {
    DecRefCount( m_HashValue );
  }
  m_HashValue = 0;
}

const char* MojoId::AsCString() const
{
  return FindCString( m_HashValue );
}

const char* MojoId::FindCString( uint64_t hash_value )
{
  return g_MojoIdManager.Find( hash_value );
}

void MojoId::IncRefCount( uint64_t hash_value )
{
  g_MojoIdManager.IncRefCount( hash_value );
}

void MojoId::DecRefCount( uint64_t hash_value )
{
  g_MojoIdManager.DecRefCount( hash_value );
}

// ---------------------------------------------------------------------------------------------------------------
