/*
 * This file is part of liblnxproc.
 *
 *  liblnxproc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  liblnxproc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#ifndef LIBLNXPROC_MAP_H
#define LIBLNXPROC_MAP_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "map_limits.h"
#include "base.h"

    int lnxproc_map_split(LNXPROC_BASE_T *base,
                          int *arraydims,
                          LNXPROC_MAP_LIMITS_T limit[], size_t limitlen);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_MAP_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
