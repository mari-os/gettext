/* Abstract sequential list data type.
   Copyright (C) 2006-2013 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _GL_LIST_H
#define _GL_LIST_H

#include <stdbool.h>
#include <stddef.h>

_GL_INLINE_HEADER_BEGIN
#ifndef GL_LIST_INLINE
# define GL_LIST_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* gl_list is an abstract list data type.  It can contain any number of
   objects ('void *' or 'const void *' pointers) in any given order.
   Duplicates are allowed, but can optionally be forbidden.

   There are several implementations of this list datatype, optimized for
   different operations or for memory.  You can start using the simplest list
   implementation, GL_ARRAY_LIST, and switch to a different implementation
   later, when you realize which operations are performed the most frequently.
   The API of the different implementations is exactly the same; when
   switching to a different implementation, you only have to change the
   gl_list_create call.

   The implementations are:
     GL_ARRAY_LIST        a growable array
     GL_CARRAY_LIST       a growable circular array
     GL_LINKED_LIST       a linked list
     GL_AVLTREE_LIST      a binary tree (AVL tree)
     GL_RBTREE_LIST       a binary tree (red-black tree)
     GL_LINKEDHASH_LIST   a hash table with a linked list
     GL_AVLTREEHASH_LIST  a hash table with a binary tree (AVL tree)
     GL_RBTREEHASH_LIST   a hash table with a binary tree (red-black tree)

   The memory consumption is asymptotically the same: O(1) for every object
   in the list.  When looking more closely at the average memory consumed
   for an object, GL_ARRAY_LIST is the most compact representation, and
   GL_LINKEDHASH_LIST and GL_TREEHASH_LIST need more memory.

   The guaranteed average performance of the operations is, for a list of
   n elements:

   Operation                  ARRAY    LINKED    TREE    LINKEDHASH   TREEHASH
                              CARRAY                   with|without with|without
                                                         duplicates  duplicates

   gl_list_size                O(1)     O(1)     O(1)      O(1)         O(1)
   gl_list_node_value          O(1)     O(1)     O(1)      O(1)         O(1)
   gl_list_node_set_value      O(1)     O(1)     O(1)      O(1)    O((log n)²)/O(1)
   gl_list_next_node           O(1)     O(1)   O(log n)    O(1)       O(log n)
   gl_list_previous_node       O(1)     O(1)   O(log n)    O(1)       O(log n)
   gl_list_get_at              O(1)     O(n)   O(log n)    O(n)       O(log n)
   gl_list_set_at              O(1)     O(n)   O(log n)    O(n)    O((log n)²)/O(log n)
   gl_list_search              O(n)     O(n)     O(n)    O(n)/O(1)    O(log n)/O(1)
   gl_list_search_from         O(n)     O(n)     O(n)    O(n)/O(1) O((log n)²)/O(log n)
   gl_list_search_from_to      O(n)     O(n)     O(n)    O(n)/O(1) O((log n)²)/O(log n)
   gl_list_indexof             O(n)     O(n)     O(n)      O(n)       O(log n)
   gl_list_indexof_from        O(n)     O(n)     O(n)      O(n)    O((log n)²)/O(log n)
   gl_list_indexof_from_to     O(n)     O(n)     O(n)      O(n)    O((log n)²)/O(log n)
   gl_list_add_first         O(n)/O(1)  O(1)   O(log n)    O(1)    O((log n)²)/O(log n)
   gl_list_add_last            O(1)     O(1)   O(log n)    O(1)    O((log n)²)/O(log n)
   gl_list_add_before          O(n)     O(1)   O(log n)    O(1)    O((log n)²)/O(log n)
   gl_list_add_after           O(n)     O(1)   O(log n)    O(1)    O((log n)²)/O(log n)
   gl_list_add_at              O(n)     O(n)   O(log n)    O(n)    O((log n)²)/O(log n)
   gl_list_remove_node         O(n)     O(1)   O(log n)  O(n)/O(1) O((log n)²)/O(log n)
   gl_list_remove_at           O(n)     O(n)   O(log n)    O(n)    O((log n)²)/O(log n)
   gl_list_remove              O(n)     O(n)     O(n)    O(n)/O(1) O((log n)²)/O(log n)
   gl_list_iterator            O(1)     O(1)   O(log n)    O(1)       O(log n)
   gl_list_iterator_from_to    O(1)     O(n)   O(log n)    O(n)       O(log n)
   gl_list_iterator_next       O(1)     O(1)   O(log n)    O(1)       O(log n)
   gl_sortedlist_search      O(log n)   O(n)   O(log n)    O(n)       O(log n)
   gl_sortedlist_search_from O(log n)   O(n)   O(log n)    O(n)       O(log n)
   gl_sortedlist_indexof     O(log n)   O(n)   O(log n)    O(n)       O(log n)
   gl_sortedlist_indexof_fro O(log n)   O(n)   O(log n)    O(n)       O(log n)
   gl_sortedlist_add           O(n)     O(n)   O(log n)    O(n)    O((log n)²)/O(log n)
   gl_sortedlist_remove        O(n)     O(n)   O(log n)    O(n)    O((log n)²)/O(log n)
 */

/* -------------------------- gl_list_t Data Type -------------------------- */

/* Type of function used to compare two elements.
   NULL denotes pointer comparison.  */
typedef bool (*gl_listelement_equals_fn) (const void *elt1, const void *elt2);

/* Type of function used to compute a hash code.
   NULL denotes a function that depends only on the pointer itself.  */
typedef size_t (*gl_listelement_hashcode_fn) (const void *elt);

/* Type of function used to dispose an element once it's removed from a list.
   NULL denotes a no-op.  */
typedef void (*gl_listelement_dispose_fn) (const void *elt);

struct gl_list_impl;
/* Type representing an entire list.  */
typedef struct gl_list_impl * gl_list_t;

struct gl_list_node_impl;
/* Type representing the position of an element in the list, in a way that
   is more adapted to the list implementation than a plain index.
   Note: It is invalidated by insertions and removals!  */
typedef struct gl_list_node_impl * gl_list_node_t;

struct gl_list_implementation;
/* Type representing a list datatype implementation.  */
typedef const struct gl_list_implementation * gl_list_implementation_t;

#if 0 /* Unless otherwise specified, these are defined inline below.  */

/* Create an empty list.
   IMPLEMENTATION is one of GL_ARRAY_LIST, GL_CARRAY_LIST, GL_LINKED_LIST,
   GL_AVLTREE_LIST, GL_RBTREE_LIST, GL_LINKEDHASH_LIST, GL_AVLTREEHASH_LIST,
   GL_RBTREEHASH_LIST.
   EQUALS_FN is an element comparison function or NULL.
   HASHCODE_FN is an element hash code function or NULL.
   DISPOSE_FN is an element disposal function or NULL.
   ALLOW_DUPLICATES is false if duplicate elements shall not be allowed in
   the list. The implementation may verify this at runtime.  */
/* declared in gl_xlist.h */
extern gl_list_t gl_list_create_empty (gl_list_implementation_t implementation,
                                       gl_listelement_equals_fn equals_fn,
                                       gl_listelement_hashcode_fn hashcode_fn,
                                       gl_listelement_dispose_fn dispose_fn,
                                       bool allow_duplicates);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_t gl_list_nx_create_empty (gl_list_implementation_t implementation,
                                          gl_listelement_equals_fn equals_fn,
                                          gl_listelement_hashcode_fn hashcode_fn,
                                          gl_listelement_dispose_fn dispose_fn,
                                          bool allow_duplicates);

/* Create a list with given contents.
   IMPLEMENTATION is one of GL_ARRAY_LIST, GL_CARRAY_LIST, GL_LINKED_LIST,
   GL_AVLTREE_LIST, GL_RBTREE_LIST, GL_LINKEDHASH_LIST, GL_AVLTREEHASH_LIST,
   GL_RBTREEHASH_LIST.
   EQUALS_FN is an element comparison function or NULL.
   HASHCODE_FN is an element hash code function or NULL.
   DISPOSE_FN is an element disposal function or NULL.
   ALLOW_DUPLICATES is false if duplicate elements shall not be allowed in
   the list. The implementation may verify this at runtime.
   COUNT is the number of initial elements.
   CONTENTS[0..COUNT-1] is the initial contents.  */
/* declared in gl_xlist.h */
extern gl_list_t gl_list_create (gl_list_implementation_t implementation,
                                 gl_listelement_equals_fn equals_fn,
                                 gl_listelement_hashcode_fn hashcode_fn,
                                 gl_listelement_dispose_fn dispose_fn,
                                 bool allow_duplicates,
                                 size_t count, const void **contents);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_t gl_list_nx_create (gl_list_implementation_t implementation,
                                    gl_listelement_equals_fn equals_fn,
                                    gl_listelement_hashcode_fn hashcode_fn,
                                    gl_listelement_dispose_fn dispose_fn,
                                    bool allow_duplicates,
                                    size_t count, const void **contents);

/* Return the current number of elements in a list.  */
extern size_t gl_list_size (gl_list_t list);

/* Return the element value represented by a list node.  */
extern const void * gl_list_node_value (gl_list_t list, gl_list_node_t node);

/* Replace the element value represented by a list node.  */
/* declared in gl_xlist.h */
extern void gl_list_node_set_value (gl_list_t list, gl_list_node_t node,
                                    const void *elt);
/* Likewise.  Return 0 upon success, -1 upon out-of-memory.  */
extern int gl_list_node_nx_set_value (gl_list_t list, gl_list_node_t node,
                                      const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Return the node immediately after the given node in the list, or NULL
   if the given node is the last (rightmost) one in the list.  */
extern gl_list_node_t gl_list_next_node (gl_list_t list, gl_list_node_t node);

/* Return the node immediately before the given node in the list, or NULL
   if the given node is the first (leftmost) one in the list.  */
extern gl_list_node_t gl_list_previous_node (gl_list_t list, gl_list_node_t node);

/* Return the element at a given position in the list.
   POSITION must be >= 0 and < gl_list_size (list).  */
extern const void * gl_list_get_at (gl_list_t list, size_t position);

/* Replace the element at a given position in the list.
   POSITION must be >= 0 and < gl_list_size (list).
   Return its node.  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_list_set_at (gl_list_t list, size_t position,
                                      const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_list_nx_set_at (gl_list_t list, size_t position,
                                         const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Search whether an element is already in the list.
   Return its node if found, or NULL if not present in the list.  */
extern gl_list_node_t gl_list_search (gl_list_t list, const void *elt);

/* Search whether an element is already in the list,
   at a position >= START_INDEX.
   Return its node if found, or NULL if not present in the list.  */
extern gl_list_node_t gl_list_search_from (gl_list_t list, size_t start_index,
                                           const void *elt);

/* Search whether an element is already in the list,
   at a position >= START_INDEX and < END_INDEX.
   Return its node if found, or NULL if not present in the list.  */
extern gl_list_node_t gl_list_search_from_to (gl_list_t list,
                                              size_t start_index,
                                              size_t end_index,
                                              const void *elt);

/* Search whether an element is already in the list.
   Return its position if found, or (size_t)(-1) if not present in the list.  */
extern size_t gl_list_indexof (gl_list_t list, const void *elt);

/* Search whether an element is already in the list,
   at a position >= START_INDEX.
   Return its position if found, or (size_t)(-1) if not present in the list.  */
extern size_t gl_list_indexof_from (gl_list_t list, size_t start_index,
                                    const void *elt);

/* Search whether an element is already in the list,
   at a position >= START_INDEX and < END_INDEX.
   Return its position if found, or (size_t)(-1) if not present in the list.  */
extern size_t gl_list_indexof_from_to (gl_list_t list,
                                       size_t start_index, size_t end_index,
                                       const void *elt);

/* Add an element as the first element of the list.
   Return its node.  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_list_add_first (gl_list_t list, const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_list_nx_add_first (gl_list_t list, const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Add an element as the last element of the list.
   Return its node.  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_list_add_last (gl_list_t list, const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_list_nx_add_last (gl_list_t list, const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Add an element before a given element node of the list.
   Return its node.  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_list_add_before (gl_list_t list, gl_list_node_t node,
                                          const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_list_nx_add_before (gl_list_t list,
                                             gl_list_node_t node,
                                             const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Add an element after a given element node of the list.
   Return its node.  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_list_add_after (gl_list_t list, gl_list_node_t node,
                                         const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_list_nx_add_after (gl_list_t list, gl_list_node_t node,
                                            const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Add an element at a given position in the list.
   POSITION must be >= 0 and <= gl_list_size (list).  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_list_add_at (gl_list_t list, size_t position,
                                      const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_list_nx_add_at (gl_list_t list, size_t position,
                                         const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Remove an element from the list.
   Return true.  */
extern bool gl_list_remove_node (gl_list_t list, gl_list_node_t node);

/* Remove an element at a given position from the list.
   POSITION must be >= 0 and < gl_list_size (list).
   Return true.  */
extern bool gl_list_remove_at (gl_list_t list, size_t position);

/* Search and remove an element from the list.
   Return true if it was found and removed.  */
extern bool gl_list_remove (gl_list_t list, const void *elt);

/* Free an entire list.
   (But this call does not free the elements of the list.)  */
extern void gl_list_free (gl_list_t list);

#endif /* End of inline and gl_xlist.h-defined functions.  */

/* --------------------- gl_list_iterator_t Data Type --------------------- */

/* Functions for iterating through a list.  */

/* Type of an iterator that traverses a list.
   This is a fixed-size struct, so that creation of an iterator doesn't need
   memory allocation on the heap.  */
typedef struct
{
  /* For fast dispatch of gl_list_iterator_next.  */
  const struct gl_list_implementation *vtable;
  /* For detecting whether the last returned element was removed.  */
  gl_list_t list;
  size_t count;
  /* Other, implementation-private fields.  */
  void *p; void *q;
  size_t i; size_t j;
} gl_list_iterator_t;

#if 0 /* These are defined inline below.  */

/* Create an iterator traversing a list.
   The list contents must not be modified while the iterator is in use,
   except for replacing or removing the last returned element.  */
extern gl_list_iterator_t gl_list_iterator (gl_list_t list);

/* Create an iterator traversing the element with indices i,
   start_index <= i < end_index, of a list.
   The list contents must not be modified while the iterator is in use,
   except for replacing or removing the last returned element.  */
extern gl_list_iterator_t gl_list_iterator_from_to (gl_list_t list,
                                                    size_t start_index,
                                                    size_t end_index);

/* If there is a next element, store the next element in *ELTP, store its
   node in *NODEP if NODEP is non-NULL, advance the iterator and return true.
   Otherwise, return false.  */
extern bool gl_list_iterator_next (gl_list_iterator_t *iterator,
                                   const void **eltp, gl_list_node_t *nodep);

/* Free an iterator.  */
extern void gl_list_iterator_free (gl_list_iterator_t *iterator);

#endif /* End of inline functions.  */

/* ---------------------- Sorted gl_list_t Data Type ---------------------- */

/* The following functions are for lists without duplicates where the
   order is given by a sort criterion.  */

/* Type of function used to compare two elements.  Same as for qsort().
   NULL denotes pointer comparison.  */
typedef int (*gl_listelement_compar_fn) (const void *elt1, const void *elt2);

#if 0 /* Unless otherwise specified, these are defined inline below.  */

/* Search whether an element is already in the list.
   The list is assumed to be sorted with COMPAR.
   Return its node if found, or NULL if not present in the list.
   If the list contains several copies of ELT, the node of the leftmost one is
   returned.  */
extern gl_list_node_t gl_sortedlist_search (gl_list_t list,
                                            gl_listelement_compar_fn compar,
                                            const void *elt);

/* Search whether an element is already in the list.
   The list is assumed to be sorted with COMPAR.
   Only list elements with indices >= START_INDEX and < END_INDEX are
   considered; the implementation uses these bounds to minimize the number
   of COMPAR invocations.
   Return its node if found, or NULL if not present in the list.
   If the list contains several copies of ELT, the node of the leftmost one is
   returned.  */
extern gl_list_node_t gl_sortedlist_search_from_to (gl_list_t list,
                                                    gl_listelement_compar_fn compar,
                                                    size_t start_index,
                                                    size_t end_index,
                                                    const void *elt);

/* Search whether an element is already in the list.
   The list is assumed to be sorted with COMPAR.
   Return its position if found, or (size_t)(-1) if not present in the list.
   If the list contains several copies of ELT, the position of the leftmost one
   is returned.  */
extern size_t gl_sortedlist_indexof (gl_list_t list,
                                     gl_listelement_compar_fn compar,
                                     const void *elt);

/* Search whether an element is already in the list.
   The list is assumed to be sorted with COMPAR.
   Only list elements with indices >= START_INDEX and < END_INDEX are
   considered; the implementation uses these bounds to minimize the number
   of COMPAR invocations.
   Return its position if found, or (size_t)(-1) if not present in the list.
   If the list contains several copies of ELT, the position of the leftmost one
   is returned.  */
extern size_t gl_sortedlist_indexof_from_to (gl_list_t list,
                                             gl_listelement_compar_fn compar,
                                             size_t start_index,
                                             size_t end_index,
                                             const void *elt);

/* Add an element at the appropriate position in the list.
   The list is assumed to be sorted with COMPAR.
   Return its node.  */
/* declared in gl_xlist.h */
extern gl_list_node_t gl_sortedlist_add (gl_list_t list,
                                         gl_listelement_compar_fn compar,
                                         const void *elt);
/* Likewise.  Return NULL upon out-of-memory.  */
extern gl_list_node_t gl_sortedlist_nx_add (gl_list_t list,
                                            gl_listelement_compar_fn compar,
                                            const void *elt)
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
  ;

/* Search and remove an element from the list.
   The list is assumed to be sorted with COMPAR.
   Return true if it was found and removed.
   If the list contains several copies of ELT, only the leftmost one is
   removed.  */
extern bool gl_sortedlist_remove (gl_list_t list,
                                  gl_listelement_compar_fn compar,
                                  const void *elt);

#endif /* End of inline and gl_xlist.h-defined functions.  */

/* ------------------------ Implementation Details ------------------------ */

struct gl_list_implementation
{
  /* gl_list_t functions.  */
  gl_list_t (*nx_create_empty) (gl_list_implementation_t implementation,
                                gl_listelement_equals_fn equals_fn,
                                gl_listelement_hashcode_fn hashcode_fn,
                                gl_listelement_dispose_fn dispose_fn,
                                bool allow_duplicates);
  gl_list_t (*nx_create) (gl_list_implementation_t implementation,
                          gl_listelement_equals_fn equals_fn,
                          gl_listelement_hashcode_fn hashcode_fn,
                          gl_listelement_dispose_fn dispose_fn,
                          bool allow_duplicates,
                          size_t count, const void **contents);
  size_t (*size) (gl_list_t list);
  const void * (*node_value) (gl_list_t list, gl_list_node_t node);
  int (*node_nx_set_value) (gl_list_t list, gl_list_node_t node,
                            const void *elt);
  gl_list_node_t (*next_node) (gl_list_t list, gl_list_node_t node);
  gl_list_node_t (*previous_node) (gl_list_t list, gl_list_node_t node);
  const void * (*get_at) (gl_list_t list, size_t position);
  gl_list_node_t (*nx_set_at) (gl_list_t list, size_t position,
                               const void *elt);
  gl_list_node_t (*search_from_to) (gl_list_t list, size_t start_index,
                                    size_t end_index, const void *elt);
  size_t (*indexof_from_to) (gl_list_t list, size_t start_index,
                             size_t end_index, const void *elt);
  gl_list_node_t (*nx_add_first) (gl_list_t list, const void *elt);
  gl_list_node_t (*nx_add_last) (gl_list_t list, const void *elt);
  gl_list_node_t (*nx_add_before) (gl_list_t list, gl_list_node_t node,
                                   const void *elt);
  gl_list_node_t (*nx_add_after) (gl_list_t list, gl_list_node_t node,
                                  const void *elt);
  gl_list_node_t (*nx_add_at) (gl_list_t list, size_t position,
                               const void *elt);
  bool (*remove_node) (gl_list_t list, gl_list_node_t node);
  bool (*remove_at) (gl_list_t list, size_t position);
  bool (*remove_elt) (gl_list_t list, const void *elt);
  void (*list_free) (gl_list_t list);
  /* gl_list_iterator_t functions.  */
  gl_list_iterator_t (*iterator) (gl_list_t list);
  gl_list_iterator_t (*iterator_from_to) (gl_list_t list,
                                          size_t start_index,
                                          size_t end_index);
  bool (*iterator_next) (gl_list_iterator_t *iterator,
                         const void **eltp, gl_list_node_t *nodep);
  void (*iterator_free) (gl_list_iterator_t *iterator);
  /* Sorted gl_list_t functions.  */
  gl_list_node_t (*sortedlist_search) (gl_list_t list,
                                       gl_listelement_compar_fn compar,
                                       const void *elt);
  gl_list_node_t (*sortedlist_search_from_to) (gl_list_t list,
                                               gl_listelement_compar_fn compar,
                                               size_t start_index,
                                               size_t end_index,
                                               const void *elt);
  size_t (*sortedlist_indexof) (gl_list_t list,
                                gl_listelement_compar_fn compar,
                                const void *elt);
  size_t (*sortedlist_indexof_from_to) (gl_list_t list,
                                        gl_listelement_compar_fn compar,
                                        size_t start_index, size_t end_index,
                                        const void *elt);
  gl_list_node_t (*sortedlist_nx_add) (gl_list_t list,
                                       gl_listelement_compar_fn compar,
                                    const void *elt);
  bool (*sortedlist_remove) (gl_list_t list,
                             gl_listelement_compar_fn compar,
                             const void *elt);
};

struct gl_list_impl_base
{
  const struct gl_list_implementation *vtable;
  gl_listelement_equals_fn equals_fn;
  gl_listelement_hashcode_fn hashcode_fn;
  gl_listelement_dispose_fn dispose_fn;
  bool allow_duplicates;
};

/* Define all functions of this file as accesses to the
   struct gl_list_implementation.  */

GL_LIST_INLINE gl_list_t
gl_list_nx_create_empty (gl_list_implementation_t implementation,
                         gl_listelement_equals_fn equals_fn,
                         gl_listelement_hashcode_fn hashcode_fn,
                         gl_listelement_dispose_fn dispose_fn,
                         bool allow_duplicates)
{
  return implementation->nx_create_empty (implementation, equals_fn,
                                          hashcode_fn, dispose_fn,
                                          allow_duplicates);
}

GL_LIST_INLINE gl_list_t
gl_list_nx_create (gl_list_implementation_t implementation,
                   gl_listelement_equals_fn equals_fn,
                   gl_listelement_hashcode_fn hashcode_fn,
                   gl_listelement_dispose_fn dispose_fn,
                   bool allow_duplicates,
                   size_t count, const void **contents)
{
  return implementation->nx_create (implementation, equals_fn, hashcode_fn,
                                    dispose_fn, allow_duplicates, count,
                                    contents);
}

GL_LIST_INLINE size_t
gl_list_size (gl_list_t list)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->size (list);
}

GL_LIST_INLINE const void *
gl_list_node_value (gl_list_t list, gl_list_node_t node)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->node_value (list, node);
}

GL_LIST_INLINE int
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_node_nx_set_value (gl_list_t list, gl_list_node_t node,
                           const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->node_nx_set_value (list, node, elt);
}

GL_LIST_INLINE gl_list_node_t
gl_list_next_node (gl_list_t list, gl_list_node_t node)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->next_node (list, node);
}

GL_LIST_INLINE gl_list_node_t
gl_list_previous_node (gl_list_t list, gl_list_node_t node)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->previous_node (list, node);
}

GL_LIST_INLINE const void *
gl_list_get_at (gl_list_t list, size_t position)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->get_at (list, position);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_nx_set_at (gl_list_t list, size_t position, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->nx_set_at (list, position, elt);
}

GL_LIST_INLINE gl_list_node_t
gl_list_search (gl_list_t list, const void *elt)
{
  size_t size = ((const struct gl_list_impl_base *) list)->vtable->size (list);
  return ((const struct gl_list_impl_base *) list)->vtable
         ->search_from_to (list, 0, size, elt);
}

GL_LIST_INLINE gl_list_node_t
gl_list_search_from (gl_list_t list, size_t start_index, const void *elt)
{
  size_t size = ((const struct gl_list_impl_base *) list)->vtable->size (list);
  return ((const struct gl_list_impl_base *) list)->vtable
         ->search_from_to (list, start_index, size, elt);
}

GL_LIST_INLINE gl_list_node_t
gl_list_search_from_to (gl_list_t list, size_t start_index, size_t end_index,
                        const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->search_from_to (list, start_index, end_index, elt);
}

GL_LIST_INLINE size_t
gl_list_indexof (gl_list_t list, const void *elt)
{
  size_t size = ((const struct gl_list_impl_base *) list)->vtable->size (list);
  return ((const struct gl_list_impl_base *) list)->vtable
         ->indexof_from_to (list, 0, size, elt);
}

GL_LIST_INLINE size_t
gl_list_indexof_from (gl_list_t list, size_t start_index, const void *elt)
{
  size_t size = ((const struct gl_list_impl_base *) list)->vtable->size (list);
  return ((const struct gl_list_impl_base *) list)->vtable
         ->indexof_from_to (list, start_index, size, elt);
}

GL_LIST_INLINE size_t
gl_list_indexof_from_to (gl_list_t list, size_t start_index, size_t end_index,
                         const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->indexof_from_to (list, start_index, end_index, elt);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_nx_add_first (gl_list_t list, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->nx_add_first (list, elt);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_nx_add_last (gl_list_t list, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->nx_add_last (list, elt);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_nx_add_before (gl_list_t list, gl_list_node_t node, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->nx_add_before (list, node, elt);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_nx_add_after (gl_list_t list, gl_list_node_t node, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->nx_add_after (list, node, elt);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_list_nx_add_at (gl_list_t list, size_t position, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->nx_add_at (list, position, elt);
}

GL_LIST_INLINE bool
gl_list_remove_node (gl_list_t list, gl_list_node_t node)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->remove_node (list, node);
}

GL_LIST_INLINE bool
gl_list_remove_at (gl_list_t list, size_t position)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->remove_at (list, position);
}

GL_LIST_INLINE bool
gl_list_remove (gl_list_t list, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->remove_elt (list, elt);
}

GL_LIST_INLINE void
gl_list_free (gl_list_t list)
{
  ((const struct gl_list_impl_base *) list)->vtable->list_free (list);
}

GL_LIST_INLINE gl_list_iterator_t
gl_list_iterator (gl_list_t list)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->iterator (list);
}

GL_LIST_INLINE gl_list_iterator_t
gl_list_iterator_from_to (gl_list_t list, size_t start_index, size_t end_index)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->iterator_from_to (list, start_index, end_index);
}

GL_LIST_INLINE bool
gl_list_iterator_next (gl_list_iterator_t *iterator,
                       const void **eltp, gl_list_node_t *nodep)
{
  return iterator->vtable->iterator_next (iterator, eltp, nodep);
}

GL_LIST_INLINE void
gl_list_iterator_free (gl_list_iterator_t *iterator)
{
  iterator->vtable->iterator_free (iterator);
}

GL_LIST_INLINE gl_list_node_t
gl_sortedlist_search (gl_list_t list, gl_listelement_compar_fn compar, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->sortedlist_search (list, compar, elt);
}

GL_LIST_INLINE gl_list_node_t
gl_sortedlist_search_from_to (gl_list_t list, gl_listelement_compar_fn compar, size_t start_index, size_t end_index, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->sortedlist_search_from_to (list, compar, start_index, end_index,
                                      elt);
}

GL_LIST_INLINE size_t
gl_sortedlist_indexof (gl_list_t list, gl_listelement_compar_fn compar, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->sortedlist_indexof (list, compar, elt);
}

GL_LIST_INLINE size_t
gl_sortedlist_indexof_from_to (gl_list_t list, gl_listelement_compar_fn compar, size_t start_index, size_t end_index, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->sortedlist_indexof_from_to (list, compar, start_index, end_index,
                                       elt);
}

GL_LIST_INLINE gl_list_node_t
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  __attribute__ ((__warn_unused_result__))
#endif
gl_sortedlist_nx_add (gl_list_t list, gl_listelement_compar_fn compar, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->sortedlist_nx_add (list, compar, elt);
}

GL_LIST_INLINE bool
gl_sortedlist_remove (gl_list_t list, gl_listelement_compar_fn compar, const void *elt)
{
  return ((const struct gl_list_impl_base *) list)->vtable
         ->sortedlist_remove (list, compar, elt);
}

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _GL_LIST_H */
