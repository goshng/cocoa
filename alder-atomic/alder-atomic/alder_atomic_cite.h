/**
 * This file, alder_atomic_cite.h, is part of alder-atomic.
 *
 * Copyright 2013 by Sang Chul Choi
 *
 * alder-atomic is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * alder-atomic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with alder-atomic.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef alder_atomic_alder_atomic_cite_h
#define alder_atomic_alder_atomic_cite_h

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

void alder_atomic_cite();

__END_DECLS


#endif /* alder_atomic_alder_atomic_cite_h */
