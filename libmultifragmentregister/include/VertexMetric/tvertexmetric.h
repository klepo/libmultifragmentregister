/**
 * @file        tvertexnetric.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TVertexMetric class declaration.
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

#ifndef TVERTEXMETRIC_H
#define TVERTEXMETRIC_H

#include <QVector>
#include "input/mesh.h"

/**
 * @brief Base class for implementing vertex metrics
 */
class TVertexMetric
{
public:
    TVertexMetric();
    ~TVertexMetric();

    virtual void setMesh(SSIMRenderer::Mesh *mesh);
    virtual void setViewsNumber(int views);

    virtual float * getValues(const QVector<QVector<bool> > & masks, const QVector<QVector3D> & points) = 0;
    virtual float * getTargetValues() = 0;

    inline int getWrongVerticesCount() const
    {
        return myWrongVertices;
    }

    virtual int valuesCount() const = 0;

protected:
    SSIMRenderer::Mesh * myMesh;
    int myViewsNumber;
    float * myData;
    float * myRefData;
    int myWrongVertices;
};

#endif // TVERTEXMETRIC_H
