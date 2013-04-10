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

// -- Mojo
#include "MojoMap.h"
#include "MojoUtil.h"
#include "MojoConfig.h"

/**
 \class MojoIdManager
 \ingroup group_id
 Dictionary that is the backing for MojoId. You must access this class through its singleton instance
 g_MojoIdManager.
 All that is needed is a call to Create() at program initialization, and Destroy() before exit.
 */
class MojoIdManager
{
public:

  MojoIdManager();

  /**
   Allocate and initialize manager.
   Call g_MojoIdManager.Create() once, at program initialization. Will make initial allocations.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global defualt will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   */
  void Create( const MojoConfig* config = NULL, MojoAlloc* alloc = NULL );

  /**
   Shut down and release memory.
   Call g_MojoIdManager.Destroy() before program exit. Will deallocate internal buffers.
   */
  void Destroy();

  /**
   Get number of entries in the table.
   \return Number of entries in the table.
   */
  int GetCount() const { return m_HashMap.GetCount(); }

private:
  struct Entry
  {
    Entry()
      : m_CString( NULL )
      , m_RefCount( 0 )
    {}

    const char* m_CString;
    int         m_RefCount;
  };

  uint64_t Insert( const char* c_string );
  void DecRefCount( uint64_t hash_code );
  void IncRefCount( uint64_t hash_code );
  const char* Find( uint64_t hash_code ) const;

  MojoMap< MojoHash< uint64_t >, Entry >  m_HashMap;
  MojoAlloc*                              m_Alloc;
  MojoStatus                               m_Status;

  friend class MojoId;
};

/**
 Global singleton instance of MojoIdManager.
 */
extern MojoIdManager g_MojoIdManager;

// ---------------------------------------------------------------------------------------------------------------
