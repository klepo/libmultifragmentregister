/**
 * @file        timagemetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TImageMetric class.
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

#include "ImageMetric/timagemetric.h"

/**
 * @brief Default constructor of TImageMetric class
 * @param renderer Pointer to the shared parent renderer
 */
TImageMetric::TImageMetric(SSIMRenderer::OffscreenRenderer *renderer):
    myRenderer(renderer),
    myObserver(0)
{    
}

/**
 * @brief Default destructor of TImageMetric class
 */
TImageMetric::~TImageMetric()
{

}

/**
 * @brief Sets the histogram bins count for the NMI metric
 * @param count Bins count
 */
void TImageMetric::setHistogramBinsCount(int count)
{
}

/**
 * @brief Sets a mask of ignored points
 * @param mask
 */
void TImageMetric::setMask(const QImage & mask)
{
}
