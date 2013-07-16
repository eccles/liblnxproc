#ifndef TOPIARY_OPT_H
#define TOPIARY_OPT_H 1

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup opt The topiary optional API.
 *
 * The topiary module interface has an optional data structure argument. This
 * section descrobes how to construct a TOPIARY_OPT_T object.
 *
 * @section topiary_opt Optional data object.
 *
 * @{
 */

/** A convenient typedef for the TOPIARY_OPT_T object. */
    typedef struct topiary_opt_t TOPIARY_OPT_T;

/**
 * @brief Create an TOPIARY_OPT_T object.
 *
 * @param[in]  optptr  Address of pointer to object - must contain NULL if
 *                         a new object is required.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_opt_new(TOPIARY_OPT_T **optptr);

/**
 * @brief Free an object.
 *
 * @param[in]  optptr   Address of pointer to object - object reference
 *                         count is decremented\. If reference count is 0 the 
 *                         object is freed.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_opt_free(TOPIARY_OPT_T **optptr);

/**
 * @brief Set fileglob attribute for object.
 *
 * @param[in]  fileglob   File globbing pattern.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 *
 */
    int topiary_opt_set_fileglob(TOPIARY_OPT_T *opt, char *fileglob);

/**
 * @brief Set master attribute for object.
 *
 * @param[in]  master   Master task character string.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 *
 */
    int topiary_opt_set_master(TOPIARY_OPT_T *opt, char *master);

/** A convenient typedef for the TOPIARY_MODULE_T object. */
    typedef struct topiary_module_t TOPIARY_MODULE_T;

/**
 * @brief Set submodule attribute for object.
 *
 * @param[in]  module   Submodule object.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 *
 */
    int topiary_opt_set_module(TOPIARY_OPT_T *opt, TOPIARY_MODULE_T *module);

/** @} ******************************************************************/

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_OPT_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
