/**
 * This file, alder_node_item_uc_edge_source.h, is part of alder-cmacro.
 *
 * Copyright 2013 by Sang Chul Choi
 *
 * alder-cmacro is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * alder-cmacro is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with alder-cmacro.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Use the following in the source code. */
#if 0
#define ALDER_BASE_UC_EDGE_NODE
#include "alder_node_memory_template_on.h"
#include "alder_node_uc_edge_source.h"
#include "alder_node_memory_source.h"
#include "alder_node_memory_template_off.h"
#undef  ALDER_BASE_UC_EDGE_NODE
#endif

#define ALDER_BASE_UC_EDGE_NODE
#include "alder_node_memory_template_on.h"

#ifndef alder_cmacro_alder_node_item_uc_edge_source_h
#define alder_cmacro_alder_node_item_uc_edge_source_h

#include <stdint.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif


__BEGIN_DECLS

/* is an edge type in the union-copy structure. This is also an element of
 * a union-find structure.
 * b - begin, start, source, and the like.
 * e - end, finish, destination, and the like.
 */
typedef struct TYPE_STRUCT
{
    bool use;
    alder_uc_node_t *b;
    alder_uc_node_t *e;
    struct TYPE_STRUCT *next;
} TYPE;

/* is a memory management for the union-copy's node.
 */
struct MEMORY_TYPE {
    TYPE **head;
    TYPE *current;
    TYPE *free;
    int size;
    size_t nhead;
};

__END_DECLS


#endif /* alder_cmacro_alder_node_item_uc_edge_source_h */

#include "alder_node_memory_template_off.h"
#undef  ALDER_BASE_UC_EDGE_NODE
