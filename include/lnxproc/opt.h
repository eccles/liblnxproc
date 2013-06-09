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

#ifndef LIBLNXPROC_OPT_H
#define LIBLNXPROC_OPT_H 1

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct lnxproc_opt_t LNXPROC_OPT_T;

    int lnxproc_opt_new(LNXPROC_OPT_T ** optptr);
    int lnxproc_opt_free(LNXPROC_OPT_T ** optptr);
    int lnxproc_opt_set_fileglob(LNXPROC_OPT_T * opt, char *fileglob);
    int lnxproc_opt_set_master(LNXPROC_OPT_T * opt, char *master);

    typedef struct lnxproc_module_t LNXPROC_MODULE_T;
    int lnxproc_opt_set_module(LNXPROC_OPT_T * opt, LNXPROC_MODULE_T * module);

#define LNXPROC_OPT_FREE(b) lnxproc_opt_free(&b)

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_OPT_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
