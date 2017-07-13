/**
 * @file        tsimplevertexmetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TSimpleVertexMetric class.
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

#include "VertexMetric/tsimplevertexmetric.h"
#include <assert.h>
#include <QDebug>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

/**
 * @brief Default constructor of TSimpleVertexMetric class
 */
TSimpleVertexMetric::TSimpleVertexMetric()
{
}

/**
 * @brief Default destructor of TSimpleVertexMetric class
 */
TSimpleVertexMetric::~TSimpleVertexMetric()
{

}

/**
 * @brief Gets the vertex metric current values
 * @param masks Vector of masks of visible vertices from all radiographs
 * @return Vertex metric values
 */
float * TSimpleVertexMetric::getValues(const QVector<QVector<bool> > & masks, const QVector<QVector3D> & pts)
{
    Q_UNUSED(pts);

    const int n = myMesh->getNumberOfVertices();
    for(int i = 0; i < n; i++)
        myData[i] = 0;

    for(int i = 0; i < masks.size(); i++)
    {
        assert(masks.at(i).size() == n);
        for(int j = 0; j < n; j++)
            if(masks.at(i).at(j))
                myData[j] += 2;
    }

    myWrongVertices = 0;
    for(int i = 0; i < n; i++)
        if(myData[i] != myRefData[i])
            myWrongVertices++;

//    qDebug() << myWrongVertices;
/*
    int i = 0;
    for(; i < (n >> 1); i++)
        myData[i] = zp; //distance;
    for(; i < n; i++)
        myData[i] = l; //distance2;
*/

    return myData;
}

/**
 * @brief Gets the target values of the vertex metric
 * @return Target values of the vertex metric
 */
float * TSimpleVertexMetric::getTargetValues()
{
    return myRefData;
}

/**
 * @brief Sets the shape model mesh
 * @param mesh Shape model mesh
 */
void TSimpleVertexMetric::setMesh(SSIMRenderer::Mesh * mesh)
{
    myMesh = mesh;
    const int n = myMesh->getNumberOfVertices();
    myData    = new float[n]();
    myRefData = new float[n]();
    for(int i = 0; i < n; i++)
        myRefData[i] = myViewsNumber * 2;
}

/**
 * @brief Get the number of values produced by the vertex metric
 * @return Values count
 */
int TSimpleVertexMetric::valuesCount() const
{
    assert(myMesh != NULL);
    return myMesh->getNumberOfVertices();
}
