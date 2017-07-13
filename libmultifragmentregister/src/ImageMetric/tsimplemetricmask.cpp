/**
 * @file        tsimplemetricmask.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        3 July 2017
 *
 * @brief       The implementation file containing the TSimpleMetricMask class.
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

#include "ImageMetric/tsimplemetricmask.h"
#include <vector>
#include <assert.h>

/**
 * @brief Constructor of TSimpleMetric class
 * @param renderer Pointer to the shared parent renderer
 */
TSimpleMetricMask::
TSimpleMetricMask(SSIMRenderer::OffscreenRenderer * renderer):
    TImageMetric(renderer),
    myData(0),
    myOriginalData(0),
    myRefData(0),
    myMaskData(0),
    myN(0)
{
}

/**
 * @brief Sets the mask
 */
void TSimpleMetricMask::setMask(const QImage & mask)
{
    myMask = mask;
    if(myMaskData != 0)
    {
        delete[] myMaskData;
    }

    int n = mask.width() * mask.height();
    myMaskData = new bool[n];
    QImage mirrored = mask.mirrored();

    unsigned int i = 0;
    unsigned int k = 0;
    for(int row = 0; row < mirrored.height(); row++)
    {
        for(int col = 0; col < mirrored.width(); col++)
        {
            myMaskData[i] = qRed(mirrored.pixel(col, row)) > 0;
            if(!myMaskData[i])
            {
                k++;
            }
            i++;
        }
    }

    myN = k;
}

/**
 * @brief Sets the original radiograph image
 * @param image Radiograph image
 */
void TSimpleMetricMask::setImage(const QImage & image)
{
    myOriginalSize = image.size();
    myImage = image;

    if(myData != 0)
    {
        delete[] myData;
        delete[] myOriginalData;
        delete[] myRefData;
    }

    //myN = myImage.width() * myImage.height();
    myData    = new float[myN];
    myRefData = new float[myN];

    QImage mirrored = myImage.mirrored();

    unsigned int i = 0;
    unsigned int k = 0;
    for(int row = 0; row < mirrored.height(); row++)
    {
        for(int col = 0; col < mirrored.width(); col++)
        {
            if(!myMaskData[i++])
            {
                myRefData[k++] = qRed(mirrored.pixel(col, row)) / 255.0;
            }
        }
    }
}

/**
 * @brief Get current value of simple metric
 * @return simple metric value
 *
 * In case of simple metric the values are equal to the rendered image mask
 * rearranged to a vector
 *
 */
float * TSimpleMetricMask::getValues()
{
    if(myObserver != NULL)
        myObserver->beforeMetric();

    myRenderer->getRenderedRedChannel(myOriginalData);

    int top    = 0;
    int bottom = myOriginalSize.height();
    int left   = 0;
    int right  = myOriginalSize.width();

    unsigned int i = 0;
    unsigned int k = 0;
    for(int y = top; y < bottom; y++)
    {
        for(int x = left; x < right; x++)
        {
            if(!myMaskData[i++]) {
                myData[k++] = myOriginalData[y * myOriginalSize.width() + x];
            }
            /*
            else {
                myData[i] = myRefData[i];
            }*/
        }
    }

    assert(k == myN);

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
float * TSimpleMetricMask::getTargetValues()
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
int TSimpleMetricMask::valuesCount() const
{
    return myN;
}

/**
 * @brief Destructor of TSimpleMetricMask class
 */
TSimpleMetricMask::~TSimpleMetricMask()
{
}
