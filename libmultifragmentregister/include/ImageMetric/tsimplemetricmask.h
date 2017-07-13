/**
 * @file        tsimplemetricmask.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        3 July 2017
 *
 * @brief       The header file containing the TSimpleMetricMask class declaration.
 *
 * @copyright   Copyright (C) 2017 Ondrej Klima, Petr Kleparnik. All Rights Reserved.
 *
 * @license     This file may be used, distributed and modified under the terms of the LGPL version 3
 *              open source license. A copy of the LGPL license should have
 *              been recieved with this file. Otherwise, it can be found at:
 *              http://www.gnu.org/copyleft/lesser.html
 *              This file has been created as a part of the Traumatech project:
 *              http://www.fit.vutbr.cz/research/grants/index.php.en?id=733.
 *
 */

#ifndef TSIMPLEMETRICMASK_H
#define TSIMPLEMETRICMASK_H

#include "timagemetric.h"
#include <QSize>

/**
 * @brief Simple image similarity class metric
 */
class TSimpleMetricMask : public TImageMetric
{
public:
    TSimpleMetricMask(SSIMRenderer::OffscreenRenderer * renderer);
    ~TSimpleMetricMask();

    float * getValues();
    float * getTargetValues();
    int valuesCount() const;
    void setImage(const QImage & image);
    void setMask(const QImage & image);

private:
    QImage myImage;
    QImage myMask;
    float * myData;
    float * myOriginalData;
    float * myRefData;
    bool * myMaskData;
    int myN;
    QSize myOriginalSize;
};


#endif // TSIMPLEMETRICMASK_H
