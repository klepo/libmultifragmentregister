/**
 * @file        tobserver.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TObserver class.
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

#include <QDebug>
#include <QImage>
#include "Observer/tobserver.h"

/**
 * @brief Constructor of TObserver class
 * @param parent Parent object
 */
TObserver::TObserver(QObject *parent) :
    QObject(parent),
    myImages(false)
{
}

/**
 * @brief Destructor of TObserver class
 */
TObserver::~TObserver()
{

}

/**
 * @brief This method is called when the shape model is moved
 * @param translations Vector of fragments translations
 */
void TObserver::translationsChanged(const QVector<QVector3D> & translations)
{
    Q_UNUSED(translations);
    //qDebug() << "TObserver::translationsChanged";
}

/**
 * @brief This method is called when the shape model is rotated
 * @param rotations Vector of fragments rotations
 */
void TObserver::rotationsChanged(const QVector<QVector3D> & rotations)
{
    Q_UNUSED(rotations);
    //qDebug() << "TObserver::rotationsChanged";
}

/**
 * @brief Method is called when the shape of the model is changed
 * @param shape Vector of non-standardized shape parameters
 */
void TObserver::shapeChanged(const QVector<float> & shape)
{
    Q_UNUSED(shape);
    //qDebug() << "TObserver::shapeChanged";
}

/**
 * @brief The method is called when new iteration of the optimization is finished
 * @param i The number of iteration
 * @param value The value of the optimized metrics
 */
void TObserver::iteration(int i, float value)
{
    Q_UNUSED(i);
    Q_UNUSED(value);
    // qDebug() << "TObserver::iteration(" << i << ", " << value << ")";
}

/**
 * @brief The method is called immediately before rendering virtual radiograph
 */
void TObserver::beforeRendering()
{
    // qDebug() << "TObserver::beforeRendering()";
}

/**
 * @brief The method is called immediately after rendering virtual radiograph
 */
void TObserver::afterRendering()
{
    // qDebug() << "TObserver::afterRendering()";
}

/**
 * @brief The method is called immediately before metric is computed
 */
void TObserver::beforeMetric()
{
    // qDebug() << "TObserver::beforeMetric()";
}

/**
* @brief The method is called immediately after metric is computed
 */
void TObserver::afterMetric()
{
    // qDebug() << "TObserver::afterMetric()";
}

/**
 * @brief The method is called before the optimization start
 */
void TObserver::beforeRegistration()
{
    // qDebug() << "TObserver::beforeRegistration()";
}

/**
 * @brief The method is called after the optimization finish
 */
void TObserver::afterRegistration()
{
    // qDebug() << "TObserver::afterRegistration()";
}

/**
 * @brief The method is called when new virtual radiographs are available after a new iteration is finished
 * @param images Vector of virtual radiographs
 */
void TObserver::downloadImages(const QVector<QImage> & images)
{
    Q_UNUSED(images);

    // qDebug() << "TObserver::getImage()";
}
