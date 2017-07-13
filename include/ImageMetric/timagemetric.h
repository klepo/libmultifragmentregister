/**
 * @file        timagemetric.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TImageMetric class declaration.
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

#ifndef TIMAGEMETRIC_H
#define TIMAGEMETRIC_H

#include <QObject>
#include <QVector>
#include <QRect>

#include "ssimrenderer.h"
#include "Observer/tobserver.h"

/**
 * @brief Base class for metric wrappers
 */
class TImageMetric
{
public:
    TImageMetric(SSIMRenderer::OffscreenRenderer * renderer = NULL);
    ~TImageMetric();

    virtual float * getValues() = 0;
    virtual float * getTargetValues() = 0;

    virtual int valuesCount() const = 0;
    virtual void setImage(const QImage & image) = 0;
    virtual void setMask(const QImage & mask);
    virtual void setHistogramBinsCount(int count);

    inline void setCrop(QRect crop)
    {
        myCrop = crop;
    }

    inline void setObserver(TObserver * observer)
    {
        myObserver = observer;
    }

protected:
    QRect myCrop;
    SSIMRenderer::OffscreenRenderer * myRenderer;
    TObserver * myObserver;
};

#endif // TIMAGEMETRIC_H
