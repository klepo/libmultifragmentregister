/**
 * @file        tsimplemetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TSimpleMetric class.
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

#include "ImageMetric/tsimplemetric.h"
#include <vector>
#include <assert.h>

/**
 * @brief Constructor of TSimpleMetric class
 * @param renderer Pointer to the shared parent renderer
 */
TSimpleMetric::
TSimpleMetric(SSIMRenderer::OffscreenRenderer * renderer):
    TImageMetric(renderer),
    myData(0),
    myOriginalData(0),
    myRefData(0),
    myN(0)
{
}

/**
 * @brief Sets the original radiograph image
 * @param image Radiograph image
 */
void TSimpleMetric::setImage(const QImage & image)
{
    myOriginalSize = image.size();
    myImage = image; //.copy(myCrop);

    if(myData != 0)
    {
        delete[] myData;
        delete[] myOriginalData;
        delete[] myRefData;
    }

    myN = myImage.width() * myImage.height();
    myData    = new float[myN];
    //myOriginalData = new float[myOriginalSize.width() * myOriginalSize.height()];
    myRefData = new float[myN];

    QImage mirrored = myImage.mirrored();

    unsigned int i = 0;
    for(int row = 0; row < mirrored.height(); row++)
        for(int col = 0; col < mirrored.width(); col++)
            myRefData[i++] = qRed(mirrored.pixel(col, row)) / 255.0;
}

/**
 * @brief Get current value of simple metric
 * @return simple metric value
 *
 * In case of simple metric the values are equal to the rendered image mask
 * rearranged to a vector
 *
 */
float * TSimpleMetric::getValues()
{
    if(myObserver != NULL)
        myObserver->beforeMetric();

    myRenderer->getRenderedRedChannel(myOriginalData);

    int top    = 0; // myOriginalSize.height() - myCrop.bottomLeft().y() - 1;
    int bottom = myOriginalSize.height(); //myOriginalSize.height() - myCrop.topLeft().y();
    int left   = 0; //myCrop.topLeft().x();
    int right  = myOriginalSize.width(); //myCrop.topRight().x() + 1;

    unsigned int i = 0;
    for(int y = top; y < bottom; y++)
        for(int x = left; x < right; x++)
            myData[i++] = myOriginalData[y * myOriginalSize.width() + x];

    assert(i == myN);

    if(myObserver != NULL)
        myObserver->afterMetric();

    delete[] myOriginalData;
    return myData;
}

/**
 * @brief Gets target values for the simple metric
 * @return simple metric target values
 *
 * In case of simple metric the target values are equal to original
 * radiographs binary masks rearranged to a 1D vector
 *
 */
float * TSimpleMetric::getTargetValues()
{
    return myRefData;
}

/**
 * @brief Gets the value count for the simple metric
 * @return Values count
 *
 * In case of simple metric the values count is equal
 * to width*height of the original radiograph
 *
 */
int TSimpleMetric::valuesCount() const
{
    return myN;
}

/**
 * @brief Destructor of TSimpleMetric class
 */
TSimpleMetric::~TSimpleMetric()
{
}
