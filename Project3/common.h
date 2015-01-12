/**************************************************
# Copyright (C) 2014 Raptis Dimos <raptis.dimos@yahoo.gr>
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# **************************************************/

/*
 *  common.h -- Basic definitions and declarations
 *  Copyright (C) 2010-2014, Computing Systems Laboratory (CSLab)
 */ 
#ifndef COMMON_H__
#define COMMON_H__

#include <stddef.h>
#define USEC_PER_SEC    1000000L

#undef  BEGIN_C_DECLS__
#undef  END_C_DECLS__

#if defined(__cplusplus) || defined(__CUDACC__)
#   define BEGIN_C_DECLS__  extern "C" {
#   define END_C_DECLS__    }
#else
#   define BEGIN_C_DECLS__
#   define END_C_DECLS__
#endif  /* __cplusplus || __CUDACC__ */

#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define ABS(a)      ((a) < 0 ? -(a) : (a))

static inline long lceil(long a, long b)
{
    return a / b + (a % b != 0);
}

static inline long lgcd(long a, long b)
{
    while (b) {
        long t = b;
        b = a % b;
        a = t;
    }

    return a;
}

static inline long llcm(long a, long b)
{
    return ABS(a*b)/lgcd(a, b);
}

#endif  /* COMMON_H__ */
