/**
 * @file        tsquareddifferencesvertexmetric.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TSquaredDifferencesVertexMetric class declaration.
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

#ifndef TSQUAREDDIFFERENCESVERTEXMETRIC_H
#define TSQUAREDDIFFERENCESVERTEXMETRIC_H

#include "VertexMetric/tvertexmetric.h"

/**
 * @brief Sum of squared differences vertex metric
 */
class TSquaredDifferencesVertexMetric : public TVertexMetric
{
public:
    TSquaredDifferencesVertexMetric();
    ~TSquaredDifferencesVertexMetric();

    virtual void setMesh(SSIMRenderer::Mesh * mesh);

    virtual float * getValues(const QVector<QVector<bool> > & masks);
    virtual float * getTargetValues();

    virtual int valuesCount() const;

private:
    int * myMask;

};

#endif // TSQUAREDDIFFERENCESVERTEXMETRIC_H
