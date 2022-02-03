Mistlib
=======
The goal of this library is to provide common data structures and algorithms while minimizing code
size. Mislib achieves this by basing most data structures and algorithms off of an array abstraction 
called a "range".

Background
----------
The premise for Mistlib is to use arrays for data storage because arrays can be abstracted to a
pointer, start and end indices, and the number of bytes per element (elemsize). This abstraction
forms a Range which provides generic functions which can operate on any array. Mislib uses the Range
to provide other data structures. For example,

	an Array is just a Range.
	a List is a Range that counts how many elements are inserted and fills the Range from the
		beginning.
	a Map is a sorted List.
	a Heap is a Range that counts how many elements are inserted and maintains the heap property.
	a Ring Buffer is a Range with a power of 2 size and write and read indices.

Furthermore, algorithms for sorting and searching all operate on Ranges. By extension, Mistlib 
sorting and searching algorithms can operate on most data structures because the data structures can
be converted to/from Ranges.

The last point is that most data structures contain a Key as the first element in their structs which
allows the data structures to be given an id and placed into and searched for in other data 
structures. For example, a Map of Maps.

