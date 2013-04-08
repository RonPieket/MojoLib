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
#include <stddef.h>

/**
 \interface MojoAlloc
 \ingroup group_config
 Interface to forward memory management to your own memory manager.
 The internal default implementation uses malloc() and free().
 */
class MojoAlloc
{
public:
  virtual ~MojoAlloc() {}
  /**
   Implement malloc() equivalent
   \param[in] byte_count Number of bytes needed.
   \param[in] name Name of the object requesting memory.
   You may ignore this.
   \return Pointer to memory block. NULL if memory could not be allocated.
   */
  virtual void* Allocate( size_t byte_count, const char* name ) = 0;
  /**
   Implement free() equivalent
   \param[in] p Pointer to the memory block
   */
  virtual void  Free( void* p ) = 0;

  /**
   Get global default allocator.
   If a class that needs memory allocation is not passed a specific allocator, it will call MojoAlloc::GetDefault().
   You may change the allocator that is returned here by setting it through SetDefault().
   \return The global default allocator.
   */
  static MojoAlloc* GetDefault();

  /**
   Set global default allocator.
   This will set the allocator returned by MojoAlloc::GetDefault().
   \param[in] alloc The new global default allocator. If NULL, the internal allocator will be used instead. This simply
   patches through to global malloc() and free().
   */
  static void SetDefault( MojoAlloc* alloc );
  
private:
  static MojoAlloc* s_Default;
};

  