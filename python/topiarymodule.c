/*
This file is part of libtopiary.

 libtopiary is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 libtopiary is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with topiary.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2013 Paul Hewlett, phewlett76@gmail.com

 Experimental code : this does not work!!
*/

#include <Python.h>
#include <topiary/topiary.h>

static PyObject *LibtopiaryError;

static PyMethodDef LibtopiaryMethods[] = {
    {"new", libtopiary_new, METH_VARARGS, "initialise topiary module"},
    {NULL, NULL, 0, NULL}       /* Sentinel */
};

PyMODINIT_FUNC
initlibtopiary(void)
{
    PyObject *m;

    m = Py_InitModule("libtopiary", LibtopiaryMethods);
    if (m == NULL)
        return;

    LibtopiaryError = PyErr_NewException("libtopiary.error", NULL, NULL);
    Py_INCREF(LibtopiaryError);
    PyModule_AddObject(m, "error", LibtopiaryError);
}

static PyObject *
libtopiary_new(PyObject * self, PyObject * args)
{
    const int nmodules;
    int ret;

    if (!PyArg_ParseTuple(args, "i", &nmodules))
        return NULL;

    ret = topiary_new(nmodules);
    if (ret) {
        char buf[96];

        PyErr_SetString(LibtopiaryError,
                        topiary_strerror(ret, buf, sizeof buf));
        return NULL;
    }
    return Py_BuildValue("i", ret);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
