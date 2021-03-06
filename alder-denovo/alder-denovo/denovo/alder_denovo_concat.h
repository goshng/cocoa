/**
 * This file, alder_denovo_concat.h, is part of alder-denovo.
 *
 * Copyright 2013 by Sang Chul Choi
 *
 * alder-denovo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * alder-denovo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with alder-denovo.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef alder_denovo_alder_denovo_concat_h
#define alder_denovo_alder_denovo_concat_h

#include "alder_fastq_concat.h"

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

alder_fasta_t * alder_denovo_concat(const char *fn1, const char *fn2);
alder_fasta_concat_t * alder_denovo_concat_init(const char *fn1, const char *fn2);

__END_DECLS


#endif /* alder_denovo_alder_denovo_concat_h */
