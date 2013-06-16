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

#include <Python.h>
#include <lnxproc/lnxproc.h>

static PyObject *LiblnxprocError;

static PyMethodDef LiblnxprocMethods[] = {
    {"new", liblnxproc_new, METH_VARARGS, "initialise lnxproc module"},
    {NULL, NULL, 0, NULL}       /* Sentinel */
};

PyMODINIT_FUNC
initliblnxproc(void)
{
    PyObject *m;

    m = Py_InitModule("liblnxproc", LiblnxprocMethods);
    if (m == NULL)
        return;

    LiblnxprocError = PyErr_NewException("liblnxproc.error", NULL, NULL);
    Py_INCREF(LiblnxprocError);
    PyModule_AddObject(m, "error", LiblnxprocError);
}

static PyObject *
liblnxproc_new(PyObject * self, PyObject * args)
{
    const int nmodules;
    int ret;

    if (!PyArg_ParseTuple(args, "i", &nmodules))
        return NULL;
    ret = lnxproc_new(nmodules);
    if (ret) {
        char buf[96];

    PyErr_SetString(LiblnxprocError, lnxproc_strerror(ret, buf, sizeof buf);
                        return NULL;}
                    return Py_BuildValue("i", ret);}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
