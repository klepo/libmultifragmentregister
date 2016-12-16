/**
 * @file        libmultifragmentregister_global.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       This is include file for shared library export.
 *
 * @copyright   Copyright (C) 2016 Ondrej Klima, Petr Kleparnik. All Rights Reserved.
 *
 * @license     This file may be used, distributed and modified under the terms of the LGPL version 3
 *              open source license. A copy of the LGPL license should have
 *              been recieved with this file. Otherwise, it can be found at:
 *              http://www.gnu.org/copyleft/lesser.html
 *              This file has been created as a part of the Traumatech project:
 *              http://www.fit.vutbr.cz/research/grants/index.php.en?id=733.
 *
 */


#ifndef LIBMULTIFRAGMENTREGISTER_GLOBAL_H
#define LIBMULTIFRAGMENTREGISTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SHARED_LIBREGISTER_LIBRARY)
#   if defined(SHARED_LIBRARY_EXPORT)
#       define SHARED_EXPORT Q_DECL_EXPORT
#   else
#       define SHARED_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define SHARED_EXPORT
#endif

#endif // LIBMULTIFRAGMENTREGISTER_GLOBAL_H
