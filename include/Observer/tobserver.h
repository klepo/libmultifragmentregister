/**
 * @file        tobserver.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TObserver class declaration.
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

#ifndef TOBSERVER_H
#define TOBSERVER_H

#include <QObject>

/**
 * @brief Base class for observer classes
 */
class TObserver : public QObject
{
    Q_OBJECT
public:
    explicit TObserver(QObject *parent = 0);
    ~TObserver();

    virtual void beforeRendering();
    virtual void afterRendering();

    virtual void beforeMetric();
    virtual void afterMetric();

    virtual void beforeRegistration();
    virtual void afterRegistration();

    virtual void translationsChanged(const QVector<QVector3D> & translations);
    virtual void rotationsChanged(const QVector<QVector3D> & rotations);
    virtual void shapeChanged(const QVector<float> & shape);

    virtual void downloadImages(const QVector<QImage> & images);

    inline bool images() const {
        return myImages;
    }

    void enableImages(bool enable) {
        myImages = enable;
    }

    virtual void iteration(int i, float value);

signals:

public slots:

private:
    bool myImages;
};

#endif // TOBSERVER_H
