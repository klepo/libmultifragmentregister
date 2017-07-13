/**
 * @file        txrayview.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TXRayView class template declaration.
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

#ifndef TXRAYVIEW_H
#define TXRAYVIEW_H

#include <QObject>
#include <QVector>
#include <QRectF>
#include "ssimrenderer.h"
#include "ImageMetric/timagemetric.h"
#include "Observer/tobserver.h"

/**
 * @brief Class template representing single radiograph
 */
template <class MetricType>
class TXRayView
{
public:
    TXRayView(SSIMRenderer::OffscreenRenderer * renderer = NULL);
    ~TXRayView();

    SSIMRenderer::OffscreenRenderer * getRenderer();
    MetricType * getMetric();

    void setObserver(TObserver * observer);
    void resizeVertexMasks(int n);

    void updateMask(float * v, int vn);
    inline QVector<bool> getMask();

    QVector<QVector<bool> > myPlusMasks;
    QVector<QVector<bool> > myMinusMasks;

    void setOpenGLCrop(const QRectF & OpenGLCrop);
    QRectF getOpenGLCrop() const;
    void setAngle(double angle);
    double getAngle() const;

private:
    QVector<bool> myMask;

    SSIMRenderer::OffscreenRenderer * myRenderer;
    MetricType * myImageMetric;
    TObserver  * myObserver;
    QRectF myOpenGLCrop;
    double myAngle;
};

#include "txrayview.hpp"

#endif // TXRAYVIEW_H
