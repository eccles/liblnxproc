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
/*
############################################################################
typical contents of /proc/cgroups file::

   subsys_name    hierarchy    num_cgroups    enabled
   cpuset                 1              4    1
   cpu                    2              4    1
   cpuacct                3              4    1
   memory                 4              4    1
   devices                5              4    1
   freezer                6              4    1
   blkio                  7              4    1
   perf_event             8              1    1

*/
/*
from pidtype import PidType
from base import Base
#----------------------------------------------------------------------------
class ProcCgroups(Base):
    '''
    Measures the /proc/cgroups file
    '''
    def __init__(self, **kwds):
        '''
        Initialises object fields
        '''
        kwds['filename'] = 'proc/cgroups'
        super(ProcCgroups, self).__init__(**kwds)

    def normalize(self, timestamp, lines):
        '''
        Converts to dictionary

        '''
        data = {}
        titles = lines[0].split()[1:]
        for rec in lines[1:]:
            fields = rec.split()
            key = fields[0].lstrip()
            vals = fields[1:]
            data[key] = dict(zip(titles, [ PidType(val) for val in vals]))
        
        return self.filter(data)

if __name__ == "__main__":
    from Test import Test
#    Test(ProcCgroups, include=r'^cpu+', exclude=r'^cpuset')
    Test(ProcCgroups)
*/

#include "array.h"
#include "proc_cgroups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int proccgroups_func(struct base_t *base, int i, int j, char *val) {
    printf(" %d,%d: '%s'\n", i, j, val);
    void *map = base_map(base);
    if( i == 0 ) {
        if( j == 0 ) {
            if ( !map ) {
                map = array_new(6,LNXPROC_ARRAY_TYPE_STRUCTPTR);
            }
            else {
                array_clear((struct array_t *)map);
            }
        }
        else {
        }
    }
    else {
        if( j == 0 ) {
        }
        else {
        }
    }
    return 0;
}

int lines_split(struct base_t *base, 
                char *line_limit,
                char *field_limit,
                int (*func)(struct base_t *, int , int , char *)) {

/* Conventional way of scanning 2-dimensionally 
    char *saveptr1;
    int i = 0;
    
    char *token = strtok_r(base->lines, "\n", &saveptr1);
    while( token ) {
        i++;

        int j = 0;
        char *saveptr2;
        char *subtoken = strtok_r(token, "\t", &saveptr2);
        while( subtoken ) {
            func(base,i,j,subtoken);
            j++;
            subtoken = strtok_r(NULL, "\t", &saveptr2);
        }

        token = strtok_r(NULL, "\n", &saveptr1);
    }
*/
/* more efficient - split in one pass */

    int n = base_nbytes(base);
    if( n > 0 ) {
        int i = 0;
        int j = 0;
    
        char *c = base_lines(base);
        char *d = c + n;
    
        char *saveptr1 = c;
        while( c < d-1 ) {
            if ( strchr(line_limit,*c) ) {
                *c = '\0';
                func(base,i,j,saveptr1);
                while( strchr( line_limit, *(++c)) );
                saveptr1 = c;
                i++;
                j = 0;
            }
            else if ( strchr(field_limit,*c) ) {
                *c = '\0';
                func(base,i,j,saveptr1);
                while( strchr( field_limit, *(++c)) );
                saveptr1 = c;
                j++;
            }
            else {
                c++;
            }
        }
        *c = '\0';
        func(base,i,j,saveptr1);
    }

    return 0; 
}

static int proccgroups_normalize(struct base_t *base) {
    lines_split(base,"\n","\t",proccgroups_func);
    return 0;
}

struct base_t *proccgroups_init(void) {
    return base_init("/proc/cgroups",
                     NULL,
                     proccgroups_normalize,
                     NULL,
                     &proccgroups_data);
}

