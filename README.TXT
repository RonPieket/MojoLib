MojoLib is a collection of algorithms that I developed for Insomniac's
proprietary world editor.

MojoLib focuses on organizing data in memory. Its functionality can be roughly
divided into three areas:

- CONTAINERS Several variations on a hash table: a key-only set, a key-
  value map, a key-multiple-values map, a bi-directional child-to-parent
  relation table, and a dual ended growable array.

- STRING IDS 8-byte objects that behave as immutable strings, for use as
  computationally efficient and yet human-readable object IDs.

- SET EXPRESSIONS Set theory applied to create rule-based views of the data
  held in containers.

MojoLib is documented at: http://www.itshouldjustworktm.com/mojolib/
