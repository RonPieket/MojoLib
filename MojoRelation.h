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
#include "MojoKeyValue.h"

/**
 \class MojoRelation
 \ingroup group_container
 Defines a many-to-one relation, such as child to parent. Each child can have only one parent. A parent can have
 any number of children.
 Also implements the MojoAbstractSet interface. As a MojoAbstractSet, the children are considered the elements.
 \see MojoForEachChildOfParent
 \tparam key_T Key type. Must be hashable.
 */
template< typename child_key_T, typename parent_key_T >
class MojoRelation final : public MojoAbstractSet< child_key_T >
{
public:
  /**
   Shorthand specialization of MojoKeyValue.
   */
  typedef MojoKeyValue< child_key_T, parent_key_T > KeyValue;
  
  /**
   Default constructor. You must call Create() before the map is ready for use.
   */
  MojoRelation()
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
  MojoRelation( const char* name, const child_key_T& child_not_found_value = child_key_T(),
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
  MojoStatus InsertChildParent( const child_key_T& child, const parent_key_T& parent );
  
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

  const MojoSetItor< parent_key_T > GetParentItor( const child_key_T& child, const parent_key_T& backup_key = parent_key_T() ) const;

  /**
   Find set of children of given parent
   \param[in] parent Parent to look for.
   \return const MojoSet< child_key_T >*, or NULL.
   */
  const MojoSet< child_key_T >* FindChildren( const parent_key_T& parent ) const;

  const MojoSetItor< child_key_T > GetChildItor( const parent_key_T& parent, const child_key_T& backup_key = child_key_T() ) const;

  /**
   Test presence of a child. If it is present, it means the child has a parent.
   \param[in] child Child to look for.
   \return true if child has a parent.
   */
  virtual bool Contains( const child_key_T& child ) const override;
  
  /**
   Test presence of a parent. If it is present, it means the parent has at least one child.
   \param[in] parent Parent to look for.
   \return true if parent has a child.
   */
  bool ContainsParent( const parent_key_T& parent ) const;
  
  /**
   Square bracket operator is an alias for FindParent()
   */
  parent_key_T operator[]( const child_key_T& child ) const { return FindParent( child ); }
  
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
  
  /**
   Get index of first occupied slot in table. This is used for the ForEach... macros. It must be declared public
   to work with the macros, but should be considered private.
   \private
   */
  int _GetFirstIndex() const;

  /**
   Get index of next occupied slot in table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  int _GetNextIndex( int index ) const;

  /**
   Verify that table index is in range. This is used for the ForEach... macros. It must be declared public to work
   with the macros, but should be considered private.
   \private
   */
  bool _IsIndexValid( int index ) const;
  
  /**
   Get key at a specific index in the table. This is used for the ForEach... macros. It must be declared public to
   work with the macros, but should be considered private.
   \private
   */
  child_key_T _GetKeyAt( int index ) const;

  virtual bool Enumerate( const MojoCollector< child_key_T >& collector,
                         const MojoAbstractSet< child_key_T >* limit = NULL ) const override;
  /** \private */
  virtual int _GetEnumerationCost() const override;
  /** \private */
  virtual int _GetChangeCount() const override;

  virtual ~MojoRelation();


  const MojoAbstractSet< parent_key_T >* GetParentSet() const { return &m_ParentToChild; }
  const MojoAbstractSet< child_key_T >* GetChildSet() const { return &m_ChildToParent; }

private:
  
  const char*                               m_Name;
  MojoMap< child_key_T, parent_key_T >      m_ChildToParent;  // A child may have only one parent
  MojoMultiMap< parent_key_T, child_key_T > m_ParentToChild;  // A parent may have multiple children

  void Init();
};

// ---------------------------------------------------------------------------------------------------------------
// Inline implementations

template< typename child_key_T, typename parent_key_T >
void MojoRelation< child_key_T, parent_key_T >::Init()
{
  m_Name = NULL;
}

template< typename child_key_T, typename parent_key_T >
MojoStatus MojoRelation< child_key_T, parent_key_T >::GetStatus() const
{
  MojoStatus status = m_ParentToChild.GetStatus();
  if( !status )
  {
    status = m_ChildToParent.GetStatus();
  }

  return status;
}

template< typename child_key_T, typename parent_key_T >
MojoStatus MojoRelation< child_key_T, parent_key_T >::Create( const char* name,
                                         const child_key_T& child_not_found_value,
                                         const parent_key_T& parent_not_found_value,
                                         const MojoConfig* config, MojoAlloc* alloc )
{
  m_Name = name;
  m_ParentToChild.Create( name, child_not_found_value, config, alloc );
  m_ChildToParent.Create( name, parent_not_found_value, config, alloc );

  return GetStatus();
}

template< typename child_key_T, typename parent_key_T >
MojoRelation< child_key_T, parent_key_T >::~MojoRelation()
{
  Destroy();
}

template< typename child_key_T, typename parent_key_T >
void MojoRelation< child_key_T, parent_key_T >::Destroy()
{
  m_ParentToChild.Destroy();
  m_ChildToParent.Destroy();
}

template< typename child_key_T, typename parent_key_T >
MojoStatus MojoRelation< child_key_T, parent_key_T >::Clear()
{
  MojoStatus status1 = m_ParentToChild.Clear();
  MojoStatus status2 = m_ChildToParent.Clear();
  return status1 ? status1 : status2;
}

template< typename child_key_T, typename parent_key_T >
MojoStatus MojoRelation< child_key_T, parent_key_T >::InsertChildParent( const child_key_T& child, const parent_key_T& parent )
{
  MojoStatus status;
  if( parent.IsHashNull() )
  {
    status = RemoveChild( child );
  }
  else if( child.IsHashNull() )
  {
    status = kMojoStatus_InvalidArguments;
  }
  else
  {
    RemoveChild( child );
    status = m_ChildToParent.Insert( child, parent );
    if( !status )
    {
      status = m_ParentToChild.Insert( parent, child );
    }
  }
  return status;
}

template< typename child_key_T, typename parent_key_T >
MojoStatus MojoRelation< child_key_T, parent_key_T >::RemoveChild( const child_key_T& child )
{
  if( !child.IsHashNull() )
  {
    parent_key_T old_parent = m_ChildToParent.Remove( child );
    if( !old_parent.IsHashNull() )
    {
      return m_ParentToChild.Remove( old_parent, child );
    }
  }
  return kMojoStatus_NotFound;
}

template< typename child_key_T, typename parent_key_T >
MojoStatus MojoRelation< child_key_T, parent_key_T >::RemoveParent( const parent_key_T& parent )
{
  if( !parent.IsHashNull() )
  {
    const MojoSet< child_key_T >* children = FindChildren( parent );
    if( children )
    {
      child_key_T child;
      MojoForEachKey( *children, child )
      {
        m_ChildToParent.Remove( child );
      }
    }
    return m_ParentToChild.Remove( parent );
  }
  return kMojoStatus_NotFound;
}

template< typename child_key_T, typename parent_key_T >
bool MojoRelation< child_key_T, parent_key_T >::ContainsParent( const parent_key_T& parent ) const
{
  return m_ParentToChild.Contains( parent );
}

template< typename child_key_T, typename parent_key_T >
bool MojoRelation< child_key_T, parent_key_T >::Contains( const child_key_T& child ) const
{
  return m_ChildToParent.Contains( child );
}

template< typename child_key_T, typename parent_key_T >
int MojoRelation< child_key_T, parent_key_T >::GetCount() const
{
  return m_ChildToParent.GetCount();
}

template< typename child_key_T, typename parent_key_T >
int MojoRelation< child_key_T, parent_key_T >::_GetFirstIndex() const
{
  return m_ChildToParent._GetFirstIndex();
}

template< typename child_key_T, typename parent_key_T >
int MojoRelation< child_key_T, parent_key_T >::_GetNextIndex( int index ) const
{
  return m_ChildToParent._GetNextIndex( index );
}

template< typename child_key_T, typename parent_key_T >
bool MojoRelation< child_key_T, parent_key_T >::_IsIndexValid( int index ) const
{
  return m_ChildToParent._IsIndexValid( index );
}

template< typename child_key_T, typename parent_key_T >
child_key_T MojoRelation< child_key_T, parent_key_T >::_GetKeyAt( int index ) const
{
  return m_ChildToParent._GetKeyAt( index );
}

template< typename child_key_T, typename parent_key_T >
bool MojoRelation< child_key_T, parent_key_T >::Enumerate( const MojoCollector< child_key_T >& collector,
                                      const MojoAbstractSet< child_key_T >* limit ) const
{
  bool more = true;
  if( limit )
  {
    for( int i = _GetFirstIndex(); more && _IsIndexValid( i ); i = _GetNextIndex( i ) )
    {
      child_key_T key = _GetKeyAt( i );
      if( limit->Contains( key ) )
      {
        more = collector.Push( key );
      }
    }
  }
  else
  {
    for( int i = _GetFirstIndex(); more && _IsIndexValid( i ); i = _GetNextIndex( i ) )
    {
      more = collector.Push( _GetKeyAt( i ) );
    }
  }
  return more;
}

template< typename child_key_T, typename parent_key_T >
int MojoRelation< child_key_T, parent_key_T >::_GetEnumerationCost() const
{
  return GetCount();
}

template< typename child_key_T, typename parent_key_T >
int MojoRelation< child_key_T, parent_key_T >::_GetChangeCount() const
{
  return m_ChildToParent._GetChangeCount();
}

template< typename child_key_T, typename parent_key_T >
parent_key_T MojoRelation< child_key_T, parent_key_T >::FindParent( const child_key_T& child ) const
{
  return m_ChildToParent.Find( child );
}

template< typename child_key_T, typename parent_key_T >
const MojoSetItor< parent_key_T > MojoRelation< child_key_T, parent_key_T >::GetParentItor( const child_key_T& child, const parent_key_T& backup_key ) const
{
  parent_key_T parent = FindParent( child );
  if( !parent.IsHashNull() )
  {
    return MojoSetItor< parent_key_T > ( parent );
  }
  else
  {
    return MojoSetItor< parent_key_T > ( backup_key );
  }
}

template< typename child_key_T, typename parent_key_T >
const MojoSet< child_key_T >* MojoRelation< child_key_T, parent_key_T >::FindChildren( const parent_key_T& parent ) const
{
  return m_ParentToChild.Find( parent );
}

template< typename child_key_T, typename parent_key_T >
const MojoSetItor< child_key_T > MojoRelation< child_key_T, parent_key_T >::GetChildItor( const parent_key_T& parent, const child_key_T& backup_key ) const
{
  const MojoSet< child_key_T >* children = FindChildren( parent );
  if( children )
  {
    return MojoSetItor< child_key_T > ( children );
  }
  else
  {
    return MojoSetItor< child_key_T > ( backup_key );
  }
}

// ---------------------------------------------------------------------------------------------------------------
