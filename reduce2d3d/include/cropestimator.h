/**
 * @file        cropestimator.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The header file containing the cropestimator class.
 *
 * @copyright   Copyright (C) 2017 Ondrej Klima, Petr Kleparnik. All Rights Reserved.
 *
 * @license     This file may be used, distributed and modified under the terms of the LGPL version 3
 *              open source license. A copy of the LGPL license should have
 *              been recieved with this file. Otherwise, it can be found at:
 *              http://www.gnu.org/copyleft/lesser.html
 *              This file has been created as a part of the Traumatech project:
 *              http://www.fit.vutbr.cz/research/grants/index.php.en?id=733.
 *
 */

#ifndef CROPESTIMATOR_H
#define CROPESTIMATOR_H

#include <QImage>
#include <QPoint>
#include <QRect>

/**
 * @brief The cropestimator class
 */
class cropestimator
{
public:
    cropestimator(QImage image, QPoint p1, QPoint p2, const QVector<bool> &overflow, bool fracTop);
    QRect myCrop;
    QRect myOpenGLCrop;
};

#endif // CROPESTIMATOR_H
