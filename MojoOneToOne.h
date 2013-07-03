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
#include "MojoUtil.h"
#include "MojoAbstractSet.h"
#include "MojoMap.h"
#include "MojoMultiMap.h"

/**
 \class MojoOneToOne
 \ingroup group_container
 Defines a many-to-one relation, such as child to parent. Each child can have only one parent. A parent can have
 any number of children.
 Also implements the MojoAbstractSet interface. As a MojoAbstractSet, the children are considered the elements.
 \see MojoForEachChildOfParent
 \tparam key_T Key type. Must be hashable.
 */
template< typename parent_key_T, typename child_key_T >
class MojoOneToOne final
{
public:
  /**
   Default constructor. You must call Create() before the map is ready for use.
   */
  MojoOneToOne()
  {
    Init();
  }
  
  /**
   Initializing constructor. No need to call Create().
   \param[in] name The name of the map. Will also be used for internal memory allocation.
   \param[in] child_not_found_value The value to return if child was not found.
   \param[in] parent_not_found_value The value to return if parent was not found.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global default will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   */
  MojoOneToOne( const char* name, const child_key_T& child_not_found_value = child_key_T(),
               const parent_key_T& parent_not_found_value = parent_key_T(), const MojoConfig* config = NULL,
               MojoAlloc* alloc = NULL )
  {
    Init();
    Create( name, child_not_found_value, parent_not_found_value, config, alloc );
  }
  
  /**
   Create after default constructor or Destroy().
   \param[in] name The name of the map. Will also be used for internal memory allocation.
   \param[in] child_not_found_value The value to return if child was not found.
   \param[in] parent_not_found_value The value to return if parent was not found.
   \param[in] config Config to use. If omitted, the global default will be used. See documentation for MojoConfig
   for details on how to set a global default.
   \param[in] alloc Allocator to use. If omitted, the global default will be used. See documentation for MojoAlloc
   for details on how to set the global default.
   \param[in] fixed_array You may provide an array that the map will use for storage. If specified, no memory
   allocation will be used.
   \param[in] fixed_array_count Number of entries in the array.
   \return Status code.
   */
  MojoStatus Create( const char* name, const child_key_T& child_not_found_value = child_key_T(),
                    const parent_key_T& parent_not_found_value = parent_key_T(), const MojoConfig* config = NULL,
                   MojoAlloc* alloc = NULL );
  
  /**
   Remove all entries and free all allocated buffers.
   */
  void Destroy();
  
  /**
   Remove all entries.
   */
  MojoStatus Clear();
  
  /**
   Insert relation. If child key already exists in map, its parent will be replaced.
   \param[in] child Child of the parent-child relation to insert.
   \param[in] parent Parent of the parent-child relation to insert.
   \return Status code.
   */
  MojoStatus InsertParentChild( const parent_key_T& parent, const child_key_T& child );
  
  /**
   Remove all child's relation with parent. There is never more than one.
   \param[in] child Child of the parent-child relation to remove.
   \return Status code.
   */
  MojoStatus RemoveChild( const child_key_T& child );
  
  /**
   Remove all relations where given key is child.
   \param[in] parent Parent to be removed.
   \return Status code.
   */
  MojoStatus RemoveParent( const parent_key_T& parent );
  
  /**
   Find parent of given child.
   \param[in] child Child to look for.
   \return Parent of the child, or not_found_value.
   */
  parent_key_T FindParent( const child_key_T& child ) const;

  /**
   Find set of children of given parent
   \param[in] parent Parent to look for.
   \return const MojoSet< child_key_T >*, or NULL.
   */
  child_key_T FindChild( const parent_key_T& parent ) const;

  /**
   Test presence of a child. If it is present, it means the child has a parent.
   \param[in] child Child to look for.
   \return true if child has a parent.
   */
  bool ContainsChild( const child_key_T& child ) const;
  
  /**
   Test presence of a parent. If it is present, it means the parent has at least one child.
   \param[in] parent Parent to look for.
   \return true if parent has a child.
   */
  bool ContainsParent( const parent_key_T& parent ) const;

  /**
   Return table status state. This is the only way to find out if something went wrong in the default constructor.
   If Create() was used, the returned status code will be the same.
   \return Status code.
   */
  MojoStatus GetStatus() const;
  
  /**
   Get number of relations in the container.
   \return Number of relations.
   */
  int GetCount() const;
  
  /**
   Return name of the container.
   \return Given name.
   */
  const char* GetName() const { return m_Name; }
  
  virtual ~MojoOneToOne();

  const MojoMap< parent_key_T, child_key_T >* GetParentToChildMap() const { return &m_ParentToChild; }
  const MojoMap< parent_key_T, child_key_T >* GetChildToParentMap() const { return &m_ChildToParent; }

  int _GetChangeCount() const { return m_ChildToParent._GetChangeCount() + m_ParentToChild._GetChangeCount(); }

private:

  const char*                           m_Name;
  MojoMap< child_key_T, parent_key_T >  m_ChildToParent;  // A child may have only one parent
  MojoMap< parent_key_T, child_key_T >  m_ParentToChild;  // A parent may have multiple children

  void Init();
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename parent_key_T, typename child_key_T >
void MojoOneToOne< parent_key_T, child_key_T >::Init()
{
  m_Name = NULL;
}

template< typename parent_key_T, typename child_key_T >
MojoStatus MojoOneToOne< parent_key_T, child_key_T >::GetStatus() const
{
  MojoStatus status = m_ParentToChild.GetStatus();
  if( !status )
  {
    status = m_ChildToParent.GetStatus();
  }
  return status;
}

template< typename parent_key_T, typename child_key_T >
MojoStatus MojoOneToOne< parent_key_T, child_key_T >::Create( const char* name,
                                         const child_key_T& child_not_found_value,
                                         const parent_key_T& parent_not_found_value,
                                         const MojoConfig* config, MojoAlloc* alloc )
{
  m_Name = name;
  m_ParentToChild.Create( name, child_not_found_value, config, alloc );
  m_ChildToParent.Create( name, parent_not_found_value, config, alloc );

  return GetStatus();
}

template< typename parent_key_T, typename child_key_T >
MojoOneToOne< parent_key_T, child_key_T >::~MojoOneToOne()
{
  Destroy();
}

template< typename parent_key_T, typename child_key_T >
void MojoOneToOne< parent_key_T, child_key_T >::Destroy()
{
  m_ParentToChild.Destroy();
  m_ChildToParent.Destroy();
}

template< typename parent_key_T, typename child_key_T >
MojoStatus MojoOneToOne< parent_key_T, child_key_T >::Clear()
{
  MojoStatus status1 = m_ParentToChild.Clear();
  MojoStatus status2 = m_ChildToParent.Clear();
  return status1 ? status1 : status2;
}

template< typename parent_key_T, typename child_key_T >
MojoStatus MojoOneToOne< parent_key_T, child_key_T >::InsertParentChild( const parent_key_T& parent, const child_key_T& child )
{
  MojoStatus status = kMojoStatus_Ok;
  if( !child.IsHashNull() )
  {
    parent_key_T old_parent = m_ChildToParent.Find( child );
    if( !old_parent.IsHashNull() )
    {
      m_ParentToChild.Remove( old_parent );
    }
  }
  if( !parent.IsHashNull() )
  {
    child_key_T old_child = m_ParentToChild.Find( parent );
    if( !old_child.IsHashNull() )
    {
      m_ChildToParent.Remove( old_child );
    }
  }

  if( !child.IsHashNull() && !parent.IsHashNull() )
  {
    status = m_ParentToChild.Insert( parent, child );
    status = m_ChildToParent.Insert( child, parent );
  }
  return status;
}

template< typename parent_key_T, typename child_key_T >
MojoStatus MojoOneToOne< parent_key_T, child_key_T >::RemoveChild( const child_key_T& child )
{
  if( !child.IsHashNull() )
  {
    parent_key_T parent = m_ChildToParent.Remove( child );
    if( !parent.IsHashNull() )
    {
      m_ParentToChild.Remove( parent );
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename parent_key_T, typename child_key_T >
MojoStatus MojoOneToOne< parent_key_T, child_key_T >::RemoveParent( const parent_key_T& parent )
{
  if( !parent.IsHashNull() )
  {
    child_key_T child = m_ParentToChild.Remove( parent );
    if( !child.IsHashNull() )
    {
      m_ChildToParent.Remove( child );
      return kMojoStatus_Ok;
    }
  }
  return kMojoStatus_NotFound;
}

template< typename parent_key_T, typename child_key_T >
bool MojoOneToOne< parent_key_T, child_key_T >::ContainsParent( const parent_key_T& parent ) const
{
  return m_ParentToChild.Contains( parent );
}

template< typename parent_key_T, typename child_key_T >
bool MojoOneToOne< parent_key_T, child_key_T >::ContainsChild( const child_key_T& child ) const
{
  return m_ChildToParent.Contains( child );
}

template< typename parent_key_T, typename child_key_T >
int MojoOneToOne< parent_key_T, child_key_T >::GetCount() const
{
  return m_ChildToParent.GetCount();
}

template< typename parent_key_T, typename child_key_T >
parent_key_T MojoOneToOne< parent_key_T, child_key_T >::FindParent( const child_key_T& child ) const
{
  return m_ChildToParent.Find( child );
}

template< typename parent_key_T, typename child_key_T >
child_key_T MojoOneToOne< parent_key_T, child_key_T >::FindChild( const parent_key_T& parent ) const
{
  return m_ParentToChild.Find( parent );
}

// ---------------------------------------------------------------------------------------------------------------
