/**
 * @file        TCPUNormalizedMutualInformationMetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TCPUNormalizedMutualInformationMetric class.
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

#include "ImageMetric/tcpunormalizedmutualinformationmetric.h"

/**
 * @brief Constructor of TCPUNormalizedMutualInformationMetric class
 * @param renderer Pointer to the shared parent renderer
 */
TCPUNormalizedMutualInformationMetric::
TCPUNormalizedMutualInformationMetric(SSIMRenderer::OffscreenRenderer * renderer):
    TImageMetric(renderer),
    nmiComputing(new SSIMRenderer::NMIComputingCPU(64, renderer))
{
    myRefData = 2;
}

/**
 * @brief Sets the histogram bins count for the NMI metric
 * @param count Bins count
 */
void TCPUNormalizedMutualInformationMetric::setHistogramBinsCount(int count)
{
    nmiComputing->setHistogramBinsCount(count);
}

/**
 * @brief Sets the original radiograph image
 * @param image Radiograph image
 */
void TCPUNormalizedMutualInformationMetric::setImage(const QImage & image)
{
    nmiComputing->setInputImage(image);
}

/**
 * @brief Get current value of NMI metric
 * @return NMI metric value
 */
float * TCPUNormalizedMutualInformationMetric::getValues()
{
    if(myObserver != NULL)
        myObserver->beforeMetric();

    myData = nmiComputing->compute();

    if(myObserver != NULL)
        myObserver->afterMetric();

    return &myData;
}

/**
 * @brief Gets target values for the NMI metric
 * @return NMI target value
 *
 * In case of NMI metric the target value is equal to 2
 *
 */
float * TCPUNormalizedMutualInformationMetric::getTargetValues()
{
    return &myRefData;
}

/**
 * @brief Gets the value count for NMI metric
 * @return Values count
 *
 * In case of NMI metric the values count is equal to 1
 *
 */
int TCPUNormalizedMutualInformationMetric::valuesCount() const
{
    return 1;
}

/**
 * @brief Destructor TCPUNormalizedMutualInformationMetric object
 */
TCPUNormalizedMutualInformationMetric::~TCPUNormalizedMutualInformationMetric()
{
}
