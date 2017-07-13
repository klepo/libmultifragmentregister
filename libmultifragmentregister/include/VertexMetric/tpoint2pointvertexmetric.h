#ifndef TPOINT2POINTVERTEXMETRIC_H
#define TPOINT2POINTVERTEXMETRIC_H

/**
 * @file        tpoint2pointvertexmetric.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 June 2017
 *
 * @brief       The header file containing the TPoint2PointVertexMetric class declaration.
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

#ifndef TSIMPLEVERTEXMETRIC_H
#define TSIMPLEVERTEXMETRIC_H

#include "VertexMetric/tvertexmetric.h"

/**
 * @brief Point to point vertex metric
 */
class TPoint2PointVertexMetric : public TVertexMetric
{
public:
    TPoint2PointVertexMetric();
    ~TPoint2PointVertexMetric();

    virtual void setMesh(SSIMRenderer::Mesh * mesh);

    virtual float * getValues(const QVector<QVector<bool> > & masks, const QVector<QVector3D> & pts);
    virtual float * getTargetValues();

    virtual int valuesCount() const;

protected:


};

#endif // TSIMPLEVERTEXMETRIC_H


#endif // TPOINT2POINTVERTEXMETRIC_H
