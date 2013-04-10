Introduction and Overview {#mainpage}
=========================
MojoLib is a collection of algorithms that I developed for Insomniac's proprietary world editor. Its performance and memory behavior is quite configurable, and it should be a good fit in games as well as in tools. MojoLib's most important innovation is the implementation of \ref page_expression

What It Is
----------
MojoLib is a C++ library focusing on organizing data in memory. Its functionality can be divided into three areas:

- \ref group_container<br/>Several variations on a hash table: a key-only set, a key-value map, a key-multiple-values map, a bi-directional child-to-parent relation table, and a dual ended growable array.
- \ref group_id<br/>8-byte objects that behave as immutable strings, for use as computationally efficient and yet human-readable object IDs.
- \ref page_expression<br/>Applied set theory to create rule-based views of the data held in containers.

If you got here and have no idea what MojoLib is used for, the \ref page_example page will clue you in.

Where to Get
------------
MojoLib is hosted at [GitHub](https://github.com/RonPieket/MojoLib).

How to Use
----------
MojoLib is distributed as C++ source code that will compile into an executable that runs unit tests. To use MojoLib in your own projects, you can leave out any file that does not begin with Mojo.

Make sure the MojoLib is compiled with your own code. You only need to include MojoLib.h to have access to all functionality.

Example Code
------------
Most of the API is reasonably well documented. The \ref page_example page contains sample code. If you need more sample code, check out the extensive unit tests in main.cpp

------------------------------------------------------------------------------------------------------------------------
\author Ron Pieket \n<http://www.ItShouldJustWorkTM.com> \n<http://twitter.com/RonPieket>

\page page_expression Set Expressions

\ref group_container contain elements that can be added and removed by the program.

\ref group_boolean combine elements from the input sets.

\ref group_function allow us to do more complex things than simply combining input elements.

A Set Expression is a tree formed out of containers, boolean operations and set functions. The tree has containers at its leaves, and boolean operations and set functions at its nodes. There is no class for set expressions <i>per se</i>. Rather, it is the use of MojoAbstractSet as a base class for containers, operations and functions, and as inputs for operations and functions, that makes it possible to form these objects into a tree.

Once the tree has been set up, any change in the container objects at the leaves will propagate through the tree and and be evaluated when you next call \link MojoAbstractSet::Enumerate() Enumerate() \endlink or \link MojoAbstractSet::Contains() Contains()\endlink at the root.

Solving a real-world problem
----------------------------
The set expression system was developed to solve several specific problems in a general way. The first specific problem it solves is the organization of special rendering in the Insomniac world editor. The editor has a 3D view in which the user can place and manipulate objects, a list view of all objects, and a properties editing panel. The problem started simple enough: render a green outline in the 3D view around model instances that are currently selected. But soon after, the complexity of the special rendering code grew:

- Objects flagged as "mouse over" must be highlighted in orange
- If a "mouse over" object is member of a group, the group's other objects must be highlighted in dark orange
- Objects flagged as "selected" must be highlighted in green (unless already highlighted)
- Objects flagged as "pasted" must be highlighted in purple (unless already highlighted)
- Objects that are part of a prefab, take their highlighting from that prefab
- Highlight for a model uses an model outline rendering function
- Highlight for a sprite uses a sprite rendering function
- Highlighted sprites use a different render function than non-highlighted ones
- Don't render any sprites if the user has turned off sprite rendering
- But as long as a sprite is selected (possibly from the list view), render it anyway
- And always hide the sprite while an object is being positioned, as it gets in the way
- While editing the navigation data, don't bother drawing unrelated sprites, such as lights
- Don't render any objects that have the "hidden" flag set
- Don't render any objects that belong to a group that has the "hidden" flag set

...and so on. The list is long and continues to grow. There are bit flags to be tested, type enums to be compared, group memberships to be checked, user preferences to be observed, editing modes to be handled, and more. I'm embarrassed to say that the code for all this grew enormous and became quite impenetrable. It would be much cleaner and easier to maintain if we could express the rules for *what* needs to be rendered *how* in an unambiguous, readable and uniform way, and ideally in data form rather than in code.

Also, this was not the only place where special case code was spiraling out of control. Mouse object selection had a similar problem. Again, *what* could be selected *when* also depended on types and flags, modes and memberships, and I'd love to make this simple, rule based and data driven, too.

That's when I remembered Venn diagrams that I learned about in high school, and something called set logic. If you were out that day, here is [a quick Wikipedia refresher](http://en.wikipedia.org/wiki/Set_%28mathematics%29). The depth of set mathematics used in MojoLib is quite basic, and math geeks will be disappointed. MojoLib is a data organization tool and not a math library.

Everything is a set
-------------------
The idea of solving the problem with a set library came to me when I realized that flags, type enums, hierarchies and group memberships can be expressed as sets. One precondition was already met: every object in the world editor is identified by an ID, or, if you will, a handle.

The next step towards a set-based organization is to remove organizational state information from the objects. Instead of storing a flag in the object to indicate "selected" or "hidden", we insert the object's ID into the "selected_set" or "hidden_set" instead. If the "selected_set" contains an object's ID, the object is selected, otherwise it is not. (See MojoSet)

And instead of storing a type enum in the object to indicate whether an object is a "model" or a "light", we insert the object's ID into the "models_set" or "lights_set". In fact, we can go one step further. At some point, we need to find the pointer to the concrete object for rendering and so on. We can store the object's ID in a "models_map" or "lights_map" instead. A map associates an ID with an object or a pointer. But thanks to an abstract interface, a map also acts as a set. This kills the proverbial two birds with one stone: we have a set that identifies an object as a model or light by its ID, and we can also find the concrete model or light object by its ID. (See MojoMap)

Hierarchies and group memberships are a little trickier. This requires the addition of relations. Relations link one ID to an other ID, as a child to a parent. To make this work within the set-based organization, we make it so that a relation lookup returns a "set of parents" or a "set of children" (the latter when performing a reverse lookup). A child can have no more than one parent, so the "set of parents" either has one element or is empty. A parent can have any number of children, so the "set of children" can contain any number of elements or be empty. (See MojoRelation and \ref group_function)

Set Theory in Action
--------------------
This is where I will leave it, as far as the Insomniac world editor is concerned. To see an example of set theory in action, follow on to the \ref page_example page.

------------------------------------------------------------------------------------------------------------------------

\page page_example Example

This example will demostrate the use of containers, boolean operations and set functions to form a somewhat interesting set expression.

We'll try to solve a problem that cannot be solved with only boolean operations. The problem was inspired by a real-life problem in the Insomniac world editor. I'm using a made-up problem rather that the actual world editor problem in order to avoid having to explain distracting details of the editor's infrastructure and user interface.

<br/>
\image html Animal-Set.jpg
<br/>

We have a number of objects on the screen. As the user moves the mouse over the objects, we want to highlight the object directly under the mouse. This indicates which object would be selected if the mouse button is clicked. We want a secondary highlight on those objects that are *closely associated* with the "mouse over" object. This indicates which objects would be selected if the user double clicks.

This example is about that secondary highlight. We'll use a database of animal species. The secondary highlight should point out species that are in the same taxonomical class (mammal, bird, or fish) and also the same environment (land, water, or air). So if the user moves the mouse over the whale, the sea lion and the dolphin, elsewhere on the screen, need to be highlighted as well.

Let's assume that we have set up two relation objects, one that maps a species id to an environment id, and the other that maps a species id to its taxonomical class id.

Let's also assume that when the mouse moved over the whale, we added its id to a set that represents all "mouse over" objects.

The entire expression uses those three inputs: the "species→environment" relation, the "species→class" relation, and the "mouse_over" input set. Note, by the way, that in both relations, the species id is the child.

<br/>
\image html Func-Example.png
<br/>

Because the elements at the output set must satisfy two distinct criteria: same class and same environment, the expression that solves the problem is an intersection of two sub-expressions.

In the diagram, the first sub-expression is on the left. It chains two functions that both work with the "species→environment" relation. The input set (id:whale) is presented to a direct function that performs a child to parent lookup. It finds set (id:water), which is passed on to the second function. This is an inverse function, so it will perform a lookup with its input set (id:water) as the parent, and find the set of children (id:whale, id:sea lion, id:dolphin, id:shark, id:tuna). This is the set of species that satisfies the environment criterium.

The second sub-expression is very much the same, except that it uses the "species→class" relation. The direct function will perform a child to parent lookup with input set (id:whale), and find parent set (id:mammal). The inverse function will perform a parent to child lookup with (id:mammal) as its input set, and return output set (id:whale, id:sea lion, id:dolphin, id:elephant, id:bat).

The final part of the expression combines the two sub-expression with a boolean intersection operation, producing output set (id:whale, id:sea lion, id:dolphin). These are the elements that both sub-expression output sets have in common.

When we implement this expression with MojoLib, you will notice that only the input set is a container. The elements of the intermediate sets are never gathered in a container or even gathered in memory.

\code{.cpp}
#include <stdio.h>
#include "MojoLib.h"

// Create storage for our input data.

MojoSet< MojoId > g_MouseOverSet( "mouse_over_set" );
MojoRelation< MojoId > g_SpeciesToEnvironment( "species_to_environment" );
MojoRelation< MojoId > g_SpeciesToClass( "species_to_class" );

// Populate our input data.

void Populate()
{
  g_SpeciesToEnvironment.InsertChildParent( "id:whale",    "id:water"  );
  g_SpeciesToEnvironment.InsertChildParent( "id:sea lion", "id:water"  );
  g_SpeciesToEnvironment.InsertChildParent( "id:dolphin",  "id:water"  );
  g_SpeciesToEnvironment.InsertChildParent( "id:shark",    "id:water"  );
  g_SpeciesToEnvironment.InsertChildParent( "id:tuna",     "id:water"  );
  g_SpeciesToEnvironment.InsertChildParent( "id:elephant", "id:land"   );
  g_SpeciesToEnvironment.InsertChildParent( "id:ostrich",  "id:land"   );
  g_SpeciesToEnvironment.InsertChildParent( "id:bat",      "id:air"    );
  g_SpeciesToEnvironment.InsertChildParent( "id:sparrow",  "id:air"    );
  
  g_SpeciesToClass      .InsertChildParent( "id:whale",    "id:mammal" );
  g_SpeciesToClass      .InsertChildParent( "id:sea lion", "id:mammal" );
  g_SpeciesToClass      .InsertChildParent( "id:dolphin",  "id:mammal" );
  g_SpeciesToClass      .InsertChildParent( "id:shark",    "id:fish"   );
  g_SpeciesToClass      .InsertChildParent( "id:tuna",     "id:fish"   );
  g_SpeciesToClass      .InsertChildParent( "id:elephant", "id:mammal" );
  g_SpeciesToClass      .InsertChildParent( "id:ostrich",  "id:bird"   );
  g_SpeciesToClass      .InsertChildParent( "id:bat",      "id:mammal" );
  g_SpeciesToClass      .InsertChildParent( "id:sparrow",  "id:bird"   );
  
  g_MouseOverSet.Insert( "id:whale" );
}

// This is the expression itself. Compare to diagram.

MojoFnDirectOpenShallow< MojoId >  g_EnvironmentSubExpr1( &g_SpeciesToEnvironment, &g_MouseOverSet        );
MojoFnInverseOpenShallow< MojoId > g_EnvironmentSubExpr2( &g_SpeciesToEnvironment, &g_EnvironmentSubExpr1 );
MojoFnDirectOpenShallow< MojoId >  g_ClassSubExpr1      ( &g_SpeciesToClass,       &g_MouseOverSet        );
MojoFnInverseOpenShallow< MojoId > g_ClassSubExpr2      ( &g_SpeciesToClass,       &g_ClassSubExpr1       );
MojoIntersection< MojoId >         g_OutputSet          ( &g_EnvironmentSubExpr2,  &g_ClassSubExpr2       );

// Now enumerate the set expression, and see what we get.

int main( int argc, const char* argv[] )
{
  g_MojoIdManager.Create();
  Populate();

  MojoArray< MojoId > result( "result" );
  g_OutputSet.Enumerate( MojoArrayCollector< MojoId >( &result ) );
  for( int i = 0; i < result.GetCount(); ++i )
  {
    printf( "%s\n", result[ i ].AsCString() );
  }

  g_MojoIdManager.Destroy();
  printf( "done\n" );
  return 0;
}
\endcode

The produces the following output:
<pre>
  id:whale
  id:sea lion
  id:dolphin
  done
</pre>

Part of the purpose of MojoLib was to solve this kind of problem in a data-driven manner. The example above looks rather hard-coded. But the expression can easily be defined in data, and the expression can be built at execution time. I won't do that here. In the Insomniac world editor, the data is in JSON format, and the code relies on a few libraries that are not part of MojoLib. But the data for the expression of this example would look like the JSON text below.

~~~~~~~~
{
  "output": {
    "type": "intersection",
    "input": [ "environment_sub_expr_2", "class_sub_expr_2" ]
  },
  "environment_sub_expr_2": {
    "type": "inverse_open_shallow",
    "input": [ "species_to_environment", "environment_sub_expr_1" ]
  },
  "environment_sub_expr_1": {
    "type": "direct_open_shallow",
    "input": [ "species_to_environment", "mouse_over_set" ]
  },
  "class_sub_expr_2": {
    "type": "inverse_open_shallow",
    "input": [ "species_to_class", "class_sub_expr_1" ]
  },
  "class_sub_expr_1": {
    "type": "direct_open_shallow",
    "input": [ "species_to_class", "mouse_over_set" ]
  }
}
~~~~~~~~

------------------------------------------------------------------------------------------------------------------------

\page abstract_sets Abstract Sets
MojoAbstractSet is an interface class template that provides two functions:

\code{.cpp}
void Enumerate( const MojoCollector< key_T >& ) const = 0;
void Contains( const key_T& ) = 0;
\endcode

An abstract set is not a container, although a container can be an abstract set. An abstract set *represents* a collection of unique keys. It does not *contain* them. How the keys are represented internally depends on the class that implements the interface.

The classes that implement MojoAbstractSet can be divided into three groups:
- <b>Containers</b>
  <br/>See \ref group_container documentation

- <b>Boolean Operations</b>
  <br/>See \ref group_boolean documentation

- <b>Set Functions</b>
  <br/>See \ref group_function documentation

For more details of how abstract sets are applied in the Insomniac world editor, see the page on \ref page_expression.

------------------------------------------------------------------------------------------------------------------------

\page page_configuration Configuration

Hash Table Density
------------------
MojoLib uses a linear probing hash table algorithm for all of its containers except MojoArray. See [this Wikipedia article](http://en.wikipedia.org/wiki/Hash_table#Open_addressing) for a quick reminder. This was chosen and modified with cache performance in mind. Its behavior can be configured in serveral ways.

The entire hash table is a single contiguous array. If a hash collision occurs, the algorithm searches forward from the calculated position to the next unoccupied slot. The probing interval is always 1, to make best use of the memory cache. When used with a good hash key generator, performance of this kind of algorithm is near constant until about 80% of the slots are occupied. Above that percentage, performance rapidly declines. Therefore, when this threshold is reached, a larger table is allocated and populated from the smaller table. The smaller table is freed.

Underpopulation can negatively impact performance. If only a few slots are occupied in a large, mostly empty table, cache misses are more likely. Therefore, when during removal of keys, the density falls below 30%, a smaller table is allocated and repopulated.

Growing and Shrinking
---------------------
This growing and shrinking behavior is designed to offer the best possible lookup performance. But memory (re-)allocation and data copying is not free. You may disable the growing and shrinking behavior. In a MojoConfig object, set m_DynamicTable to false, and set m_BufferMinCount to a size large enough to contain all the data you are going to store in the container. In the case of hash tables, only 80% of the table can be used, plan accordingly.

The next section will explain what you can do with the MojoConfig object.

Memory Management
-----------------
How memory management is handled is highly application dependent. And I know how persnickety games programmers can be about memory allocations. In MojoLib, you may specify *where* memory is allocated from, and *when* it is allocated. This can be configured per container, as well as globally.

Where to allocate from:
- **System heap** - do nothing. This is the default behavior. MojoLib will use malloc() and free()
- **Custom heap** - define a specialization of MojoAlloc, overriding Allocate() and Free() to defer to your own heap manager.

When to allocate:
- **As needed** - do nothing. This is the default behavior
- **At initialization only** - in a MojoConfig object, set m_DynamicAlloc to false, and set m_BufferMinCount to a size large enough to contain all the data you are going to store in the container. In the case of hash tables, only 80% of the table can be used, plan accordingly.
- **Compile time only** - declare workspace arrays at compile time, pass a pointer to the array to the \link MojoSet::MojoSet(const char*,const MojoConfig*,MojoAlloc*,key_T*,int) initializing constructor\endlink or to \link MojoSet::Create() Create()\endlink

Both the MojoAlloc object and the MojoConfig object may be passed as an argument to the \link MojoSet::MojoSet(const char*,const MojoConfig*,MojoAlloc*,key_T*,int) initializing constructor\endlink or to \link MojoSet::Create() Create()\endlink

You may make a MojoAlloc or MojoConfig object the global default by calling MojoAlloc::SetDefault() of MojoConfig::SetDefault() respectively. This will store a pointer to your object, so make sure it is not on the stack.

m_DynamicTable vs. m_DynamicAlloc
---------------------------------
A hash table is stored in a buffer. The table may be smaller than the buffer, but obviously not larger.

As noted above, MojoConfig::m_DynamicTable controls whether or not the table is resized dynamically. MojoConfig::m_DynamicAlloc controls whether the buffer can be resized. Resizing the *table* can be done without resizing the *buffer*, as long as the table size does not exceed the buffer size. Shrinking and growing the table within the buffer will improve lookup performance (by keeping the table dense, thus memory cache friendly), without calling out to the heap manager.

\image html drinkme.png

------------------------------------------------------------------------------------------------------------------------

\page page_caching Caching Intermediate Results

Normally, a set expression is evaluated when Contains() or Enumerate() is called. The result is not stored, but returned to the caller. This ensures that Contains() and Enumerate() always return results based on the current state of the containers at the expression leaves. MojoLib tries to optimize evaluation in several ways. However, if you have an expression or sub-expression that:

- is complex, and
- is frequently accessed, and
- is often unchanged between acccesses

...then this on-demand evaluation scheme will cause some unnecessary processing and memory access. *If* this is a concern, MojoCacheSet can help. A cache set is initialized with a pointer to a set to be cached. When \ref MojoCacheSet::Update() is called, it will enumerate the input set into its internal set. This internal set is used when the MojoCacheSet is accessed through Contains() and Enumerate().

------------------------------------------------------------------------------------------------------------------------

\defgroup group_container Containers
\brief Containers allow insertion and removal of elements.

MojoLib uses a linear probing hash table algorithm for all of its containers except the array. See [this Wikipedia article](http://en.wikipedia.org/wiki/Hash_table#Open_addressing) for a quick reminder. This was chosen and modified with cache performance in mind. See the \ref page_optimization page for the details.

------------------------------------------------------------------------------------------------------------------------

\defgroup group_boolean Boolean Operations
\brief A boolean operation combines the elements from the inputs sets.

Intersection
------------
The output set has all the elements that the input sets have in common. See also MojoIntersection.

`S.Contains(key) = S1.Contains(key) AND S2.Contains(key) AND S3.Contains(key) ...`

`S = S1 ∩ S2 ∩ S3 ...`

\image html Set-Intersection.png

Union
-----
The output set has all elements of all input sets. See also MojoUnion.

`S.Contains(key) = S1.Contains(key) OR S2.Contains(key) OR S3.Contains(key) ...`

`S = S1 ∪ S2 ∪ S3 ...`

\image html Set-Union.png

Difference
----------
The output set has all the elements of the first set, that are not in the other sets. See also MojoDifference.

`S.Contains(key) = S1.Contains(key) AND NOT S2.Contains(key) AND NOT S3.Contains(key) ...`

`S = S1 - ( S2 ∪ S3 ... )`

\image html Set-Difference.png

Complement
----------
The output set has all the elements that are not in any of the input sets. See also MojoComplement.

`S.Contains(key) = NOT S1.Contains(key) AND NOT S2.Contains(key) AND NOT S3.Contains(key) ...`

`S = ~( S1 ∪ S2 ∪ S3 ... )`

\image html Set-Complement.png

------------------------------------------------------------------------------------------------------------------------

\defgroup group_function Set Functions
\brief A set function is a function whose inputs and outputs are sets.

In MojoLib, a set function takes an input set, and for each element computes an output set. The output of the function is the union of all computed sets.

Set functions allow us to do more complex things that simply combining input elements. Set functions introduce relations. *For each element at the input, a set function will perform a lookup in a relation, and produce any number of output elements.*

For an example of set functions in use, see the \ref page_example page.

A set function in MojoLib is initialized with a pointer to a MojoAbstractSet, and a pointer to a MojoRelation. For each element in the input set, the function will perform a lookup in the relation, and add the result to the output.

What this means in practice is that for every child at the input, a parent appears at the output. Or, in the case of an inverse function, for every parent, its children are added to the output.

A set function in MojoLib can perform the lookup in the MojoRelation in different ways. There are three aspects with two variations each, giving a total of eight set function variations. The three aspects are:
- **Direct vs Inverse**<br/>The Direct variation uses child→parent relations. The Inverse variation uses parent→child relations.
- **Open vs Closed**<br/>In an Open variation, keys that have no parent (when using child→parent relations, Direct) or no children (when using parent→child relations, Inverse), do not contribute to the output set. In a Closed variation, the key itself will be included in the output set. Compare the Open and Closed variations in the images below: if a key has no outgoing arrow, the Closed variation adds a red arrow pointing back to itself.
- **Shallow vs Deep**<br/>A Shallow variation resolves at most one level of the relation. A Deep variation resolves relations recursively. In the case of child→parent relations, the Deep variation will look for a grandparent, a great-grandparent, and so on. In the case of parent→child relations, it will look for grandchildren, great-grandchildren, and so forth. In the images below, a deep relation is indicated by a blue arrow.

In the table below, note that the original relation (left most box) and the input set (left most oval) are always the same. Only the interpretation (right most box) varies, resulting in a different output (right most oval).

Direct, Open, Shallow
---------------------
The output set contains all parents of the keys in the input set. Input keys without a parent have no effect. See also MojoFnDirectOpenShallow.
\image html Func-Direct-Open-Shallow.png

Direct, Open, Deep
------------------
The output set contains all parents of the keys in the input set, and recursively all parents of keys in the *output* set. Input keys without a parent have no effect. See also MojoFnDirectOpenDeep.
\image html Func-Direct-Open-Deep.png

Direct, Closed, Shallow
-----------------------
The output set contains all parents of the keys in the input set. The output set also contains all parentless input keys. See also MojoFnDirectClosedShallow.
\image html Func-Direct-Closed-Shallow.png

Direct, Closed, Deep
--------------------
The output set contains all parents of the keys in the input set, and recursively all parents of keys in the *output* set. The output set also contains all parentless input keys. See also MojoFnDirectClosedDeep.
\image html Func-Direct-Closed-Deep.png

Inverse, Open, Shallow
----------------------
The output set contains all children of the keys in the input set. Input keys with no children have no effect. See also MojoFnInverseOpenShallow.
\image html Func-Inverse-Open-Shallow.png

Inverse, Open, Deep
-------------------
The output set contains all children of the keys in the input set, and recursively all children of keys in the *output* set. Input keys with no children have no effect. See also MojoFnInverseOpenDeep.
\image html Func-Inverse-Open-Deep.png

Inverse, Closed, Shallow
------------------------
The output set contains all children of the keys in the input set. The output set also contains all childless input keys. See also MojoFnInverseClosedShallow.
\image html Func-Inverse-Closed-Shallow.png

Inverse, Closed, Deep
---------------------
The output set contains all children of the keys in the input set, and recursively all children of keys in the *output* set. The output set also contains all childless input keys. See also MojoFnInverseClosedDeep.
\image html Func-Inverse-Closed-Deep.png

------------------------------------------------------------------------------------------------------------------------

\defgroup group_id String Ids
\brief 8-byte objects that behave as immutable strings.

A MojoId can be used as a computationally and space efficient object ID. It is initialized with a C-string, and backed by a reference counted string dictionary. A MojoId can be dereferenced as a C-string, assigned a new value from a C-string, implicitly constructed from a C-string, and tested for equality with a C-string.

A MojoId is especially suitable for use as a key in \ref group_container.

------------------------------------------------------------------------------------------------------------------------

\defgroup group_util Utilities
\brief Bits and pieces that make hashing easier.

------------------------------------------------------------------------------------------------------------------------

\defgroup group_set_common Common Types and Base Classes 
\brief Common types and base classes for \ref group_container, \ref group_boolean, and \ref group_function.

------------------------------------------------------------------------------------------------------------------------

\defgroup group_config Configuration
\brief Used to control behavior of containers.

------------------------------------------------------------------------------------------------------------------------
