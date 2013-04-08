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

#ifndef LIBLNXPROC_ERROR_H
#define LIBLNXPROC_ERROR_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stddef.h>

    enum lnxproc_error_t {
        LNXPROC_OK = 0,
        LNXPROC_ERROR_SYSTEM,
        LNXPROC_ERROR_ARRAY_MALLOC_HEADER,
        LNXPROC_ERROR_ARRAY_MALLOC_DATA,
        LNXPROC_ERROR_ARRAY_NULL,
        LNXPROC_ERROR_ARRAY_REALLOC_DATA,
        LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE,
        LNXPROC_ERROR_BASE_NULL,
        LNXPROC_ERROR_BASE_MALLOC_BASE,
        LNXPROC_ERROR_BASE_MALLOC_BUFFER,
        LNXPROC_ERROR_BASE_MALLOC_MAPLIMITS,
        LNXPROC_ERROR_BASE_MAPLIMITS_NULL,
        LNXPROC_ERROR_BASE_MALLOC_ARRAYDIMS,
        LNXPROC_ERROR_BASE_MALLOC_MAPLIMITS_ENTRY,
    };
    typedef enum lnxproc_error_t LNXPROC_ERROR_T;

    typedef void (*LNXPROC_ERROR_CALLBACK) (const char *filename,
                                            int lineno,
                                            const char *funcname,
                                            LNXPROC_ERROR_T err);

    char *lnxproc_strerror(LNXPROC_ERROR_T err, char *buf, size_t buflen);

    void lnxproc_error_print_callback(const char *filename,
                                      int lineno,
                                      const char *funcname,
                                      LNXPROC_ERROR_T err);

    void lnxproc_set_error(LNXPROC_ERROR_CALLBACK callback,
                           const char *filename,
                           int lineno,
                           const char *funcname, LNXPROC_ERROR_T err);
    void lnxproc_system_error(LNXPROC_ERROR_CALLBACK callback,
                              const char *filename,
                              int lineno, const char *funcname, int err);

#define LNXPROC_SET_ERROR(c,e) lnxproc_set_error(c,__FILE__,__LINE__,__func__,e)

#define LNXPROC_SYSTEM_ERROR(c,e) \
                            lnxproc_system_error(c,__FILE__,__LINE__,__func__,e)

#ifdef DEBUG
    void lnxproc_debug(const char *filename,
                       int lineno, const char *funcname, char *fmt, ...
        );

#define LNXPROC_DEBUG(fmt, args...) \
                          lnxproc_debug(__FILE__,__LINE__,__func__, fmt, ##args)

#define LNXPROC_ERROR_DEBUG(s, fmt, args...) {\
    char buf[32];\
    char *c = lnxproc_strerror(s,buf,sizeof buf);\
    char buf1[64];\
    snprintf(buf1,sizeof buf1,"Error : %d %s",s,c);\
    char buf2[64];\
    snprintf(buf2,sizeof buf2,fmt, ##args);\
    lnxproc_debug(__FILE__,__LINE__,__func__, "%s: %s\n", buf1, buf2);\
}

#else
#define LNXPROC_DEBUG(fmt, args...)
#define LNXPROC_ERROR_DEBUG(s, fmt, args...)
#endif

#define eprintf(format, args...) fprintf (stderr, format , ##args)
#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_ERROR_H
