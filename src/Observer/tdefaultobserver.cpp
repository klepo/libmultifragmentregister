/**
 * @file        tdefaultobserver.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the TDefaultObserver class.
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

#include "Observer/tdefaultobserver.h"
#include <QTime>
#include <QDebug>
#include <iostream>
#include <QDir>
#include <QImage>

/**
 * @brief Constructor of TDefaultObserver class
 */
TDefaultObserver::TDefaultObserver():
    myRenderingTime(0),
    myMetricTime(0),
    myRenderedImages(0),
    myMetricsComputed(0),
    myRegistrationTime(0),
    myVerbose(true),
    myWholeBones(true),
    myIterations(0),
    myStream(NULL)
{
}

/**
 * @brief Destructor of TDefaultObserver class
 */
TDefaultObserver::~TDefaultObserver()
{
}

/**
 * @brief Emits the signal the position of shape model has changed
 * @param translations Vector of fragments positions
 */
void TDefaultObserver::translationsChanged(const QVector<QVector3D> & translations)
{
    emit sigTranslationsChanged(translations);
}

/**
 * @brief Emits the signal the rotation of the shape model has changed
 * @param rotations Vector of fragments rotations
 */
void TDefaultObserver::rotationsChanged(const QVector<QVector3D> & rotations)
{
    emit sigRotationsChanged(rotations);
}

/**
 * @brief Emits the signal the shape has changed
 * @param shape Vector of non-standardized shape parameters
 */
void TDefaultObserver::shapeChanged(const QVector<float> & shape)
{
    emit sigShapeChanged(shape);
}

/**
 * @brief Starts the rendering timer and increases renderings counter
 */
void TDefaultObserver::beforeRendering()
{
    myRenderingTimer.start();
    myRenderedImages++;
}

/**
 * @brief Stops the rendering timer and adds the value to global accumulator
 */
void TDefaultObserver::afterRendering()
{
    myRenderingTime += myRenderingTimer.elapsed();
}

/**
 * @brief Starts the metric timer and increases the metric counter
 */
void TDefaultObserver::beforeMetric()
{
    myMetricTimer.start();
    myMetricsComputed++;
}

/**
 * @brief Stops the metric timer and adds the value to global accumulator
 */
void TDefaultObserver::afterMetric()
{
    myMetricTime += myMetricTimer.elapsed();
}

/**
 * @brief TDefaultObserver::beforeRegistration
 */
void TDefaultObserver::beforeRegistration()
{
    myRegistrationTimer.start();
    //qDebug() << "beforeRegistration";
}

/**
 * @brief TDefaultObserver::afterRegistration
 */
void TDefaultObserver::afterRegistration()
{
    myRegistrationTime += myRegistrationTimer.elapsed();
}

/**
 * @brief Prints out the number of current iteration and current value of metric
 * @param i Iteration number
 * @param value Value of the metric
 */
void TDefaultObserver::iteration(int i, float value)
{
    if(myVerbose)
        std::cout << "iteration: " << i << "   objective: " << value << std::endl;

    if(myStream != 0)
        (*myStream) << value << ";";

    myValues << value;
    myIterations++;
}

/**
 * @brief Downloads and saves the virtual radiographs after the iteration is finished
 * @param images Vector containing rendered radiographs
 */
void TDefaultObserver::downloadImages(const QVector<QImage> & images)
{
    //qDebug() << "observer::downloadImages";
    //for(int i = 0; i < images.size() / 2; i++)
    for(int i = 0; i < images.size(); i++)
    {
        QImage result(images.at(i).size(), images.at(i).format());
        result.fill(0);
        /*
        for(int row = 0; row < result.height() / 2; row++)
            for(int col = 0; col < result.width(); col++)
                result.setPixel(col, row, qRgb(64, 64, 64));
                */

        for(int row = 0; row < result.height(); row++)
            for(int col = 0; col < result.width(); col++)
            {
                QRgb pr = myRefImages.at(i).pixel(col, row);
                QRgb p1 = images.at(i).pixel(col, row);
                //QRgb p2 = images.at(i + 2).pixel(col, row);

                int red = qRed(pr);
                int green = 0;
                int blue = 0;

                //if(myCrops.at(i).contains(col, row) || myWholeBones)
                    green = qGreen(p1);

                //if(myCrops.at(i + 2).contains(col, row) || myWholeBones)
                 //   blue = qBlue(p2);

                //if(red > 0 || green > 0 || blue >0)
                if(red > 0 || green > 0)
                    result.setPixel(col, row, qRgb(red, green, blue));
            }

        QStringList list;
        list << myPath << "/" << QString::number(i) << "_" << QString::number(myIterations) << ".png";
        result.save(list.join(""));
        //QTransform tr;
        //tr.rotate(180);
        //result.transformed(tr).save(list.join(""));

    }
    /*
    QDir(".").mkdir("images");
    for(int i = 0; i < images.size(); i++)
    {
        QImage image1 = images.at(i).copy(myCrops.at(i));
        QImage image2 = myRefImages.at(i).copy(myCrops.at(i));

        QImage img(image1.size(), image1.format());
        img.fill(0);
        for(int row = 0; row < image1.height(); row++)
        {
            for(int col = 0; col < image1.width(); col++)
            {
                QRgb p1 = image1.pixel(col, row);
                QRgb p3 = image2.pixel(col, row);

                if(qRed(p1) != 0 && qRed(p3) != 0)
                    img.setPixel(col, row, qRgb(qRed(p3), qRed(p1), qRed(p1)));
                else if(qRed(p1) != 0)
                    img.setPixel(col, row, qRgb(0,qRed(p1),qRed(p1)));
                else if(qRed(p3) != 0)
                    img.setPixel(col, row, qRgb(qRed(p3),0,0));
            }
        }


        QStringList list;
        list << myPath << "/" << QString::number(i) << "_" << QString::number(myIterations) << ".png";
        img.save(list.join(""));
    }
    */
}
