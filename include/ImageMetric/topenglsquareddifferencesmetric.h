/**
 * @file        topenglsquareddifferencesmetric.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TOpenGLSquaredDifferencesMetric class declaration.
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

#ifndef TOPENGLSQUAREDDIFFERENCESMETRIC_H
#define TOPENGLSQUAREDDIFFERENCESMETRIC_H

#include "timagemetric.h"
#include "ssimrenderer.h"

/**
 * @brief Wrapper for SSD metric computed using GPU
 */
class TOpenGLSquaredDifferencesMetric : public TImageMetric
{
public:
    TOpenGLSquaredDifferencesMetric(SSIMRenderer::OffscreenRenderer * renderer);
    ~TOpenGLSquaredDifferencesMetric();

    float * getValues();
    float * getTargetValues();
    int valuesCount() const;
    void setImage(const QImage & image);

private:
    SSIMRenderer::SSDComputingOpenGL * mySSDComputing;
    float myData;
    float myRefData;
};

#endif // TGPUSQUAREDDIFFERENCESMETRIC_H
