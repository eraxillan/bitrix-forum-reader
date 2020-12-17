/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifndef __BFR_EXPORT_H__
#define __BFR_EXPORT_H__

#ifdef HAVE_QX_ORM
#ifdef _BUILDING_BFR
#define BFR_DLL_EXPORT QX_DLL_EXPORT_HELPER
#else
#define BFR_DLL_EXPORT QX_DLL_IMPORT_HELPER
#endif
#else
#define BFR_DLL_EXPORT
#endif

#ifdef HAVE_QX_ORM
#ifdef _BUILDING_BFR
#define QX_REGISTER_HPP_BFR QX_REGISTER_HPP_EXPORT_DLL
#define QX_REGISTER_CPP_BFR QX_REGISTER_CPP_EXPORT_DLL
#define QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR QX_REGISTER_COMPLEX_CLASS_NAME_HPP_EXPORT_DLL
#define QX_REGISTER_COMPLEX_CLASS_NAME_CPP_BFR QX_REGISTER_COMPLEX_CLASS_NAME_CPP_EXPORT_DLL
#else
#define QX_REGISTER_HPP_BFR QX_REGISTER_HPP_IMPORT_DLL
#define QX_REGISTER_CPP_BFR QX_REGISTER_CPP_IMPORT_DLL
#define QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR QX_REGISTER_COMPLEX_CLASS_NAME_HPP_IMPORT_DLL
#define QX_REGISTER_COMPLEX_CLASS_NAME_CPP_BFR QX_REGISTER_COMPLEX_CLASS_NAME_CPP_IMPORT_DLL
#endif
#endif

#endif // __BFR_EXPORT_H__

