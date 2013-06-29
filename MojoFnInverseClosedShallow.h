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

#include "MojoConstants.h"
#include "MojoAbstractSet.h"
#include "MojoCollector.h"
#include "MojoUtil.h"

#include "MojoFnInverseOpenShallow.h"

/** \cond HIDE_FORWARD_REFERENCE */
template< typename child_key_T, typename parent_key_T > class MojoRelation;
/** \endcond */

/**
 \class MojoFnInverseClosedShallow
 \ingroup group_function
 Contains all children of the keys in the input set. The output set also contains all childless input keys.
 \image html Func-Inverse-Closed-Shallow.png
 \note Enumeration may result in duplicate entries. Enumeration into a MojoSetCollector is recommended.
 */
template< typename key_T >
class MojoFnInverseClosedShallow final : public MojoAbstractSet< key_T >
{
public:
  /**
   Construct from a MojoRelation and a MojoAbstractSet object.
   \param[in] relation The relation that defines the function.
   \param[in] set The set to be transformed by the function.
   */
  MojoFnInverseClosedShallow( const MojoRelation< key_T, key_T >* relation, const MojoAbstractSet< key_T >* set )
  : m_Childless( relation->GetParentSet() )
  , m_ChildlessInput( &m_Childless, set )
  , m_FnOpen( relation, set )
  , m_Union( &m_FnOpen, &m_ChildlessInput )
  {}
  
  virtual bool Contains( const key_T& key ) const override
  {
    return m_Union.Contains( key );
  }
  
  virtual bool Enumerate( const MojoCollector< key_T >& collector,
                         const MojoAbstractSet< key_T >* limit = NULL ) const override
  {
    return m_Union.Enumerate( collector, limit );
  }
  
  /** \private */
  virtual int _GetEnumerationCost() const override
  {
    return m_Union._GetEnumerationCost();
  }
  
  /** \private */
  virtual int _GetChangeCount() const override
  {
    return m_Union._GetChangeCount();
  }
  
private:

  MojoComplement< key_T >             m_Childless;
  MojoIntersection< key_T >           m_ChildlessInput;
  MojoFnInverseOpenShallow< key_T >   m_FnOpen;
  MojoUnion< key_T >                  m_Union;
};

// ---------------------------------------------------------------------------------------------------------------
