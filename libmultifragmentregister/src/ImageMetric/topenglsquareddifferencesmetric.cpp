/**
 * @file        topenglsquareddifferencesmetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TOpenGLSquaredDifferencesMetric class.
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

#include "ImageMetric/topenglsquareddifferencesmetric.h"

/**
 * @brief Constructor of TOpenGLSquaredDifferencesMetric class
 * @param renderer Pointer to the shared parent renderer
 */
TOpenGLSquaredDifferencesMetric::
TOpenGLSquaredDifferencesMetric(SSIMRenderer::OffscreenRenderer * renderer):
    TImageMetric(renderer),
    mySSDComputing(new SSIMRenderer::SSDComputingOpenGL(renderer))
{
    myRefData = 0;
}

/**
 * @brief Sets the original radiograph image
 * @param image Radiograph image
 */
void TOpenGLSquaredDifferencesMetric::setImage(const QImage & image)
{
    mySSDComputing->setInputImage(image);
}

/**
 * @brief Get current value of SSD metric
 * @return SSD metric value
 */
float * TOpenGLSquaredDifferencesMetric::getValues()
{
    if(myObserver != NULL)
        myObserver->beforeMetric();

    myData = mySSDComputing->compute();

    if(myObserver != NULL)
        myObserver->afterMetric();

    return &myData;
}

/**
 * @brief Gets target values for the SSD metric
 * @return SSD target value
 *
 * In case of SSD metric the target value is equal to 0
 *
 */
float * TOpenGLSquaredDifferencesMetric::getTargetValues()
{
    return &myRefData;
}

/**
 * @brief Gets the value count for SSD metric
 * @return Values count
 *
 * In case of SSD metric the values count is equal to 1
 *
 */
int TOpenGLSquaredDifferencesMetric::valuesCount() const
{
    return 1;
}

/**
 * @brief Desctructor of TOpenGLSquaredDifferencesMetric class
 */
TOpenGLSquaredDifferencesMetric::~TOpenGLSquaredDifferencesMetric()
{
}
