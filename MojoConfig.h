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

class MojoAlloc;


/**
 \struct MojoConfig
 \ingroup group_config
 Common parameter block for use with MojoLib's container classes, controlling their behavior.
 */
struct MojoConfig
{
  /**
   Default constructor will initialize parameters to the current default values.
   \see SetDefault(), GetDefault()
   */
  MojoConfig();
  /**
   The initial number of entries, and also the minimum number of allocate entries. If table becomes smaller than
   this, hang on to the memory.
  */
  int         m_BufferMinCount;
  /**
   Control whether dynamic memory allocation is allowed when elements are inserted or removed.
   */
  bool        m_DynamicAlloc;
  /**
   Control whether table size should be adjusted to optimize population density.
   */
  bool        m_DynamicTable;
  
  /**
   Get the current default config.
   */
  static const MojoConfig* GetDefault();
  
  /**
   Set a new default config.
   This will be used for every subsequent GetDefault() call, and for every default constructor.
   This mechanism allows you to control system-wide default behavior.
   To control per-instance behavior, classes that use MojoConfig accept a pointer to a config object
   during initialization.
   \param[in] config Pointer to the config to use. If null, the internal default config will be used. The config
   will be copied, so the object may live on the stack.
   */
  static void SetDefault( const MojoConfig* config );

private:
  MojoConfig( bool );
  static const MojoConfig* s_Default;
};

// ---------------------------------------------------------------------------------------------------------------
