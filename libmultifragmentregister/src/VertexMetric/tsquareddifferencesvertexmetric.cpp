/**
 * @file        tsquareddifferencesvertexmetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TSquaredDifferencesVertexMetric class.
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

#include "VertexMetric/tsquareddifferencesvertexmetric.h"
#include <assert.h>
#include <QDebug>

/**
 * @brief Default constructor of TSquaredDifferencesVertexMetric class
 */
TSquaredDifferencesVertexMetric::TSquaredDifferencesVertexMetric()
{

}

/**
 * @brief Default destructor of TSimpleVertexMetric class
 */
TSquaredDifferencesVertexMetric::~TSquaredDifferencesVertexMetric()
{

}

/**
 * @brief Gets the vertex metric current values
 * @param masks Vector of masks of visible vertices from all radiographs
 * @return Vertex metric values
 */
float * TSquaredDifferencesVertexMetric::getValues(const QVector<QVector<bool> > & masks, const QVector<QVector3D> &points)
{
    Q_UNUSED(points);

    const int n = myMesh->getNumberOfVertices();
    for(int i = 0; i < n; i++)
        myMask[i] = 0;

    for(int i = 0; i < masks.size(); i++)
    {
        assert(masks.at(i).size() == n);
        for(int j = 0; j < n; j++)
            if(masks.at(i).at(j))
                myMask[j] += 1;
    }

    *myData = 0;

    int square;
    int wrongVertices = 0;
    for(int i = 0; i < n; i++)
    {
        square = myMask[i] - myViewsNumber;
        if(square != 0)
            wrongVertices++;

        square *= square;
        *myData += square;
    }

    //qDebug() << wrongVertices;

    //*myData *= 5;
    //qDebug() << *myData;
    return myData;
}

/**
 * @brief Gets the target values of the vertex metric
 * @return Target values of the vertex metric
 */
float * TSquaredDifferencesVertexMetric::getTargetValues()
{
    return myRefData;
}

/**
 * @brief Sets the shape model mesh
 * @param mesh Shape model mesh
 */
void TSquaredDifferencesVertexMetric::setMesh(SSIMRenderer::Mesh * mesh)
{
    myMesh = mesh;
    const int n = myMesh->getNumberOfVertices();
    myData    = new float[1]();
    myRefData = new float[1]();

    myMask = new int[n]();

    *myRefData = 0;
}

/**
 * @brief Get the number of values produced by the vertex metric
 * @return Values count
 */
int TSquaredDifferencesVertexMetric::valuesCount() const
{
    assert(myMesh != NULL);
    return 1;
}
