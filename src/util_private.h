/*
 * This file is part of topiary.
 *
 *  topiary is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  topiary is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with topiary.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

/* Common macros and utility functions
 */

#ifndef TOPIARY_UTIL_PRIVATE_H
#define TOPIARY_UTIL_PRIVATE_H 1

#include <topiary/util.h>

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

#ifndef WARN_FORMAT
#define WARN_FORMAT(mmm,nnn) __attribute__((format(printf,mmm,nnn)))
#endif

#endif                          // TOPIARY_UTIL_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
