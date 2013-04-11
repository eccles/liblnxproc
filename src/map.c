/*
This file is part of liblnxproc.

 liblnxproc is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 liblnxproc is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2013 Paul Hewlett, phewlett76@gmail.com

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "limits.h"
#include "array.h"

int
lnxproc_split(LNXPROC_ARRAY_T *map,
              LNXPROC_ERROR_CALLBACK callback,
              LNXPROC_LIMITS_T limits[], int dim, char *lines, int nbytes)
{

    if (nbytes > 0) {
        char *c = lines;
        char *d = c + nbytes;

        char *saveptr = c;

        if (dim > 0) {
            size_t idx[dim];

            memset(idx, 0, dim * sizeof(int));

            while (c < d) {
                size_t i;
                int increment = 1;

                for (i = 0; i < dim; i++) {
                    if (lnxproc_chr(limits + i, *c)) {
                        *c = '\0';

                        int ret = lnxproc_array_set_last(map, idx, i, saveptr);

                        if (ret) {
                            return ret;
                        }

                        while ((++c < d) && lnxproc_chr(limits + i, *c));

                        saveptr = c;
                        idx[i]++;
                        int j = i + 1;

                        if (dim > j) {
                            memset(idx + j, 0, (dim - j) * sizeof(int));
                        }

                        increment = 0;
                    }
                }
                if (increment) {
                    c++;
                }
            }
        }
        else {
            while (c < d) {
                if (strchr("\n", *c)) {
                    *c = '\0';
                    break;
                }
                c++;
            }
        }
    }

    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
