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
#include "MojoIdManager.h"

// -- Mojo
#include "MojoMap.h"
#include "MojoUtil.h"
#include "MojoConfig.h"

MojoIdManager g_MojoIdManager;

MojoIdManager::MojoIdManager()
: m_Status( kMojoStatus_NotInitialized )
{}

void MojoIdManager::Create( const MojoConfig* config, MojoAlloc* alloc )
{
  if( !config )
  {
    config = MojoConfig::GetDefault();
  }
  if( !alloc )
  {
    alloc = MojoAlloc::GetDefault();
  }

  m_Alloc = alloc;
  m_HashMap.Create( "MojoIdManager", Entry(), config );
  m_Status = kMojoStatus_Ok;
}

void MojoIdManager::Destroy()
{
  m_HashMap.Destroy();
  m_Status = kMojoStatus_NotInitialized;
}

uint64_t MojoIdManager::Insert( const char* c_string )
{
  uint64_t hash_code = MojoFnv64( c_string );
  if( hash_code && !m_Status )
  {
    Entry entry = m_HashMap.Find( hash_code );
    if( !entry.m_CString )
    {
      size_t length = strlen( c_string );
      char* string_mem = ( char * )m_Alloc->Allocate( length + 1, "MojoId string" );
      memcpy( string_mem, c_string, length );
      string_mem[ length ] = 0;
      entry.m_RefCount = 0;
      entry.m_CString = string_mem;
      m_HashMap.Insert( hash_code, entry );
    }
    IncRefCount( hash_code );
  }
  return hash_code;
}

void MojoIdManager::DecRefCount( uint64_t hash_code )
{
  if( hash_code && !m_Status )
  {
    Entry* entry_ptr = m_HashMap.FindForImmediateChange( hash_code );
    if( entry_ptr )
    {
      entry_ptr->m_RefCount -= 1;
      if( entry_ptr->m_RefCount == 0 )
      {
        m_Alloc->Free( ( void* )entry_ptr->m_CString );
        m_HashMap.Remove( hash_code );
      }
    }
  }
}

void MojoIdManager::IncRefCount( uint64_t hash_code )
{
  if( hash_code && !m_Status )
  {
    Entry* entry_ptr = m_HashMap.FindForImmediateChange( hash_code );
    if( entry_ptr )
    {
      entry_ptr->m_RefCount += 1;
    }
  }
}

const char* MojoIdManager::Find( uint64_t hash_code ) const
{
  if( hash_code && !m_Status )
  {
    Entry entry = m_HashMap.Find( hash_code );
    return entry.m_CString;
  }
  return NULL;
}

// ---------------------------------------------------------------------------------------------------------------
