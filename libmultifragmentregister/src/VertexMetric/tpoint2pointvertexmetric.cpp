/**
 * @file        tsimplevertexmetric.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2017
 *
 * @brief       The implementation file containing the TPoint2PointVertexMetric class.
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

#include "VertexMetric/tpoint2pointvertexmetric.h"
#include <assert.h>
#include <QDebug>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

/**
 * @brief Default constructor of TSimpleVertexMetric class
 */
TPoint2PointVertexMetric::TPoint2PointVertexMetric()
{
}

/**
 * @brief Default destructor of TSimpleVertexMetric class
 */
TPoint2PointVertexMetric::~TPoint2PointVertexMetric()
{

}

/**
 * @brief Gets the vertex metric current values
 * @param masks Vector of masks of visible vertices from all radiographs
 * @return Vertex metric values
 */
float * TPoint2PointVertexMetric::getValues(const QVector<QVector<bool> > & masks, const QVector<QVector3D> & pts)
{
    /*
    qDebug() << "vertex metrika";
    qDebug() << pts.size();
    */

    //qDebug() << "body:" << pts.size();

    const int n = myMesh->getNumberOfVertices() / 100 * 2;
    for(int i = 0; i < n; i++)
        myData[i] = 0;

    double distance = (pts.at(2) - pts.at(0)).length();
    QVector3D p1 = pts.at(1);
    QVector3D p3 = pts.at(3);
    p1.setZ(0);
    p3.setZ(0);
    double distance2 = (p3 - p1).length();
    //qDebug() << distance;

    // Vypocet uhlu
    QVector3D v = pts.at(0);//-pts.at(1);
    QVector3D u = pts.at(2);//-pts.at(3);
    v.setZ(0);
    u.setZ(0);
    double cp = acos(QVector3D::dotProduct(v, u) / (u.length()*v.length())) * 180 / M_PI;
    double zp = ((QVector3D::dotProduct(v, u) / (u.length()*v.length())) - 1)*1000;
    //qDebug() << cp << zp;
    double l = pts.at(0).z()-pts.at(2).z();

    for(int i = 0; i < masks.size(); i++)
    {
        assert(masks.at(i).size() == n);
        for(int j = 0; j < n; j++)
            if(masks.at(i).at(j))
                myData[j] += 1;
    }

    myWrongVertices = 0;
    for(int i = 0; i < n; i++)
        if(myData[i] != myRefData[i])
            myWrongVertices++;

    int i = 0;
    for(; i < (n >> 1); i++)
        myData[i] = zp; //distance;
    for(; i < n; i++)
        myData[i] = l; //distance2;


    return myData;
}

/**
 * @brief Gets the target values of the vertex metric
 * @return Target values of the vertex metric
 */
float * TPoint2PointVertexMetric::getTargetValues()
{
    return myRefData;
}

/**
 * @brief Sets the shape model mesh
 * @param mesh Shape model mesh
 */
void TPoint2PointVertexMetric::setMesh(SSIMRenderer::Mesh * mesh)
{
    myMesh = mesh;
    const int n = myMesh->getNumberOfVertices() / 100 * 2;
    myData    = new float[n]();
    myRefData = new float[n]();
    for(int i = 0; i < n; i++)
        myRefData[i] = 0;//myViewsNumber * 1;
}

/**
 * @brief Get the number of values produced by the vertex metric
 * @return Values count
 */
int TPoint2PointVertexMetric::valuesCount() const
{
    assert(myMesh != NULL);
    return myMesh->getNumberOfVertices() / 100 * 2;
}
