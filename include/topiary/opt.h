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

#ifndef TOPIARY_OPT_H
#define TOPIARY_OPT_H 1

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct topiary_opt_t TOPIARY_OPT_T;

    int topiary_opt_new(TOPIARY_OPT_T **optptr);
    int topiary_opt_free(TOPIARY_OPT_T **optptr);
    int topiary_opt_set_fileglob(TOPIARY_OPT_T *opt, char *fileglob);
    int topiary_opt_set_master(TOPIARY_OPT_T *opt, char *master);

    typedef struct topiary_module_t TOPIARY_MODULE_T;
    int topiary_opt_set_module(TOPIARY_OPT_T *opt, TOPIARY_MODULE_T *module);

#define TOPIARY_OPT_FREE(b) topiary_opt_free(&b)

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_OPT_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
