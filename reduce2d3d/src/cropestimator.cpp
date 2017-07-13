/**
 * @file        cropestimator.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The implementation file containing the cropestimator class.
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

#include "cropestimator.h"
#include <QDebug>

/**
 * @brief cropestimator::cropestimator
 * @param image Segmented input image
 * @param p1 First point of the fracture line
 * @param p2 Second point of the fracture line
 * @param overflow If the captured bone overflow from the image
 * @param fracTop Distal or proximal fragment
 */
cropestimator::cropestimator(QImage image, QPoint p1, QPoint p2, const QVector<bool> & overflow, bool fracTop)
{
    QPoint topLeft2(image.width() - 1, image.height() - 1);
    QPoint bottomRight2(0, 0);

    // Bounding box
    for(int x = 0; x < image.width(); x++)
    {
        for(int y = 0; y < image.height(); y++)
        {
            if(qRed(image.pixel(x,y)) > 2)
            {
                if(x < topLeft2.x()) topLeft2.setX(x);
                if(y < topLeft2.y()) topLeft2.setY(y);

                if(x > bottomRight2.x()) bottomRight2.setX(x);
                if(y > bottomRight2.y()) bottomRight2.setY(y);
            }
        }
    }

    // 20% padding
    int lpad = (bottomRight2.x() - topLeft2.x()) * 0.2;
    int rpad = (bottomRight2.x() - topLeft2.x()) * 0.2;
    int tpad = (bottomRight2.y() - topLeft2.y()) * 0.2;
    int bpad = (bottomRight2.y() - topLeft2.y()) * 0.2;

    if(overflow.at(0)) tpad = 0;
    if(overflow.at(1)) lpad = 0;
    if(overflow.at(2)) bpad = 0;
    if(overflow.at(3)) rpad = 0;

    topLeft2.setX(topLeft2.x() - lpad);
    if(topLeft2.x() < 0)
        topLeft2.setX(0);

    bottomRight2.setX(bottomRight2.x() + rpad);
    if(bottomRight2.x() > image.width() - 1)
        bottomRight2.setX(image.width() - 1);


    // proximalni fragment
    if(!fracTop)
    {
        topLeft2.setY(topLeft2.y() - tpad);
        if(topLeft2.y() < 0)
            topLeft2.setY(0);
        bottomRight2.setY(p2.y());
    }
    // distalni fragment
    else
    {
        topLeft2.setY(p2.y());

        bottomRight2.setY(bottomRight2.y() + bpad);
        if(bottomRight2.y() > image.height() - 1)
            bottomRight2.setY(image.height() - 1);
    }


    myCrop = QRect(topLeft2, bottomRight2);

    QPoint topLeft, bottomRight;

    topLeft.setX(-100000);
    bottomRight.setX(100000);

    // proximal fragment
    if(fracTop)
    {
        topLeft.setY(-100000);
        bottomRight.setY(p1.y());
    }
    // distal fragment
    else
    {
        bottomRight.setY(100000);
        // point p2 due to oblique fractures
        topLeft.setY(p2.y());
    }

    myOpenGLCrop = QRect(topLeft, bottomRight);
}
