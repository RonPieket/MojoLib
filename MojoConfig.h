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
   Capacity limit below which not to bother freeing memory.
   It may be desirable to hold on to unused memory if it is likely to be needed a little later. This will
   reduce the frequency of memory allocations.
  */
  int         m_AllocCountMin;
  /**
   Length of tables will not be reduced below this limit.
   Table shrinkage is intended to improve cache performance.
   <br>Keeping a minimum size will avoid unnecessary copy operations.
   */
  int         m_TableCountMin;
  /**
   Population percentage above which to grow the tables.
   Default is 80%.
   That means that when more than 80% of the entries in the table are used, the length is doubled, reducing the
   population density to about 40%.
   <br>Note that m_GrowThreshold must be more than twice the value of m_ShrinkThreshold.
   <br>Setting this parameter to 100 will disable all growth behavior.
   */
  int         m_GrowThreshold;
  /**
   Population percentage below which to shrink the tables.
   Default is 30%.
   That means that when less than 30% of the entries in the table are in use, the length is halved, increasing the
   population density to about 60%.
   <br>Note that m_shrinkThreshold must be less than half the value of m_GrowThreshold.
   <br>Setting this parameter to 0 will disable all shrinkage behavior.
   */
  int         m_ShrinkThreshold;
  /**
   Allow automatic growth behavior during Insert() operation.
   When enabled, and during an Insert() operation the population exceeds m_GrowThreshold, the table length
   will double.
   <br>If disabled, you must call Update() to trigger the resize.
   */
  bool        m_AutoGrow;
  /**
   Allow automatic shrinkage behavior during Remove() operation.
   When enabled, and during an Remove() operation the population falls below m_GrowThreshold, the table length
   will halved.
   <br>If disabled, you must call Update() to trigger the resize.
   */
  bool        m_AutoShrink;
  /**
   Control whether dynamic memory allocation is allowed.
   Note that the initial allocation, either in the Create() call or the use of a non-default constructor, is always
   allowed.
   */
  bool        m_DynamicAlloc;
  
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
   \param[in] config Pointer to the config to use. If null, the internal default config will be used. The config will
   be copied, so the object may live on the stack.
   */
  static void SetDefault( const MojoConfig* config );

private:
  MojoConfig( bool );
  static const MojoConfig* s_Default;
};

