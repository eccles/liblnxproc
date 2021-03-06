/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>              // printf
#include <string.h>             // strcmp
#include <topiary/topiary.h>

#include "testing.h"
/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    char *module = NULL;

    if (argc > 2) {
        module = argv[2];
    }

    if (argc < 2) {
        printf("First option is unspecified\n");
        return 1;
    }
    else if (!strcmp(argv[1], "time")) {
        testing(0,module);
    }
    else if (!strcmp(argv[1], "test")) {
        testing(1,module);
    }
    else {
        printf("Illegal option %s\n", argv[1]);
        return 1;
    }

    return 0;
}
