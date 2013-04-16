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

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "template.h"

struct lnxproc_template_t {
    const char *filename;
    LNXPROC_ERROR_CALLBACK callback;
};

const char *
lnxproc_template_filename(LNXPROC_TEMPLATE_T * temp)
{
    LNXPROC_DEBUG("Template %p\n", temp);
    const char *filename = NULL;

    if (temp) {
        filename = temp->filename;
    }

    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    return filename;
}

int
lnxproc_template_print(LNXPROC_TEMPLATE_T * temp)
{
    LNXPROC_DEBUG("Template %p\n", temp);

    if (temp) {
        printf("Filename %s\n", temp->filename);
    }

    LNXPROC_DEBUG("WARNING: Template is null\n");
    return LNXPROC_ERROR_TEMPLATE_NULL;
}

LNXPROC_TEMPLATE_T *
lnxproc_template_init(const char *filename, LNXPROC_ERROR_CALLBACK callback)
{
    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    LNXPROC_DEBUG("callback %p\n", callback);

    LNXPROC_TEMPLATE_T *temp = malloc(sizeof(LNXPROC_TEMPLATE_T));

    if (!temp) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_TEMPLATE_MALLOC, "Malloc temp\n");
        return temp;
    }

    temp->filename = filename;
    temp->callback = callback;
    LNXPROC_DEBUG("Successful\n");
    return temp;
}

LNXPROC_TEMPLATE_T *
lnxproc_template_free(LNXPROC_TEMPLATE_T * temp)
{
    LNXPROC_DEBUG("Template %p\n", temp);

    if (temp) {
        LNXPROC_DEBUG("Free Template\n");
        free(temp);
        temp = NULL;
    }

    return temp;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
