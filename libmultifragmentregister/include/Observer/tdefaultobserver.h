/**
 * @file        tdefaultobserver.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TDefaultObserver class declaration.
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

#ifndef TDEFAULTOBSERVER_H
#define TDEFAULTOBSERVER_H

#include "Observer/tobserver.h"
#include <QObject>
#include <QTime>
#include <QVector3D>
#include <QImage>
#include <QTextStream>

/**
 * @brief Default registration observer class
 */
class TDefaultObserver : public TObserver
{
    Q_OBJECT
public:
    TDefaultObserver();
    ~TDefaultObserver();

    virtual void beforeRendering();
    virtual void afterRendering();

    virtual void beforeMetric();
    virtual void afterMetric();

    virtual void beforeRegistration();
    virtual void afterRegistration();

    virtual void iteration(int i, float value);
    virtual void downloadImages(const QVector<QImage> & images);

    virtual void translationsChanged(const QVector<QVector3D> & translations);
    virtual void rotationsChanged(const QVector<QVector3D> & rotations);
    virtual void shapeChanged(const QVector<float> & shape);

    inline void setVerbose(bool verbose) { myVerbose = verbose; }
    inline void setRefImages(const QVector<QImage> & images) { myRefImages = images; }
    inline void setCrops(const QVector<QRect> & crops) { myCrops = crops; }
    inline void setWholeBones(bool wholeBones) { myWholeBones = wholeBones; }

    inline const  QList<float> & getValues() const { return myValues; }
    void setImagesPath(QString path) { myPath = path; }
    void setStream(QTextStream * stream) { myStream = stream; }

    inline int    getIterations()       const { return myIterations; }
    inline double getRenderingTime()    const { return myRenderingTime / 1000.0; }
    inline double getMetricTime()       const { return myMetricTime    / 1000.0; }
    inline double getRegistrationTime() const { return myRegistrationTime / 1000.0; }
    inline double getRenderedCount()    const { return myRenderedImages;  }
    inline double getMetricCount()      const { return myMetricsComputed; }
    inline double getTimePerRender()    const { return myRenderingTime / 1000.0 / myRenderedImages;  }
    inline double getTimePerMetric()    const { return myMetricTime    / 1000.0 / myMetricsComputed; }


signals:
    void sigTranslationsChanged(const QVector<QVector3D> & translations);
    void sigRotationsChanged(const QVector<QVector3D> & rotations);
    void sigShapeChanged(const QVector<float> & shape);

protected:
    QTextStream * myStream;

    QTime myRenderingTimer;
    QTime myMetricTimer;
    QTime myRegistrationTimer;

    int myRegistrationTime;
    int myRenderingTime;
    int myMetricTime;

    int myRenderedImages;
    int myMetricsComputed;
    bool myVerbose;
    bool myWholeBones;

    QList<float> myValues;
    int myIterations;
    QVector<QImage> myRefImages;
    QVector<QRect> myCrops;

    QString myPath;
};

#endif // TDEFAULTOBSERVER_H
