/**
 * @file        tbonefragment.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TBoneFragment class template declaration.
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

#ifndef TBONEFRAGMENT_H
#define TBONEFRAGMENT_H

#include <dlib/matrix.h>
#include "txrayview.h"
#include "Observer/tobserver.h"

/**
 * @brief Class template representing single bone fragment
 */
template <class MetricType>
class TBoneFragment
{    
public:
    explicit TBoneFragment(int ViewCount);
    ~TBoneFragment();

    void initValuesCount();

    void setShapeModel(  SSIMRenderer::MatStatisticalDataFile * shapeFile);
    void setDensityModel(SSIMRenderer::MatStatisticalDataFile * densityFile);
    void setPoseEps(double eps);

    void setStandardizedShapeParams(  const QVector<float> & shapeParams);
    void setStandardizedDensityParams(const QVector<float> & densityParams);
    void setShapeParams(  const QVector<float> & shapeParams);
    void setDensityParams(const QVector<float> & densityParams);

    QVector<float> getStandardizedShapeParams();
    QVector<float> getShapeParams();

    void setTranslation(const QVector3D & translation);
    void setRotation(const QVector3D & rotation);
    void setPoints(const QVector<QVector3D> & points);
    QVector<QVector3D> getTransformedPoints() const;

    QVector3D getTranslation() const;
    QVector3D getRotation() const;

    typedef QVector3D (TBoneFragment<MetricType>::*getPoseType)() const;
    typedef void (TBoneFragment<MetricType>::*setPoseType)(const QVector3D & pose);

    dlib::matrix<float> shapeGradient(unsigned int ParamCount = 0);
    dlib::matrix<float> poseGradient(setPoseType setPose, getPoseType getPose);
    dlib::matrix<float> rotationGradient();
    dlib::matrix<float> translationGradient();    

    void updateMasks();

    inline int getValuesCount() const
    {
        return myValuesCount;
    }

    inline QVector<SSIMRenderer::OffscreenRenderer *> getRenderers()
    {
        return myRenderers;
    }

    inline QVector<MetricType *> getMetrics()
    {
        return myMetrics;
    }

    inline QVector<TXRayView<MetricType> *> getXRayViews()
    {
        return myXRayViews;
    }

    void setObserver(TObserver * observer);
    QVector<QVector<QVector3D> > myPlusPoints;
    QVector<QVector<QVector3D> > myMinusPoints;


signals:

public slots:

private:
    QVector<QVector3D> myPoints;
    QVector<TXRayView<MetricType> *> myXRayViews;
    QVector<SSIMRenderer::OffscreenRenderer *> myRenderers;
    QVector<MetricType *> myMetrics;
    int myValuesCount;

    TObserver * myObserver;
    double myPoseEps;
};

#include "tbonefragment.hpp"

#endif // TBONEFRAGMENT_H
