/**
 * @file        xmlparser.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The header file containing the xmlparser class.
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

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QString>
#include <QVector>
#include <QImage>
#include <QRect>
#include <QPoint>

#include "ssimrenderer.h"

/**
 * @brief The xmlparser class
 */
class xmlparser
{
public:
    xmlparser(QString mpath, QString fileName);
    ~xmlparser();

    QString shapeFileName() const;
    QString densityFileName() const;
    QString meshFileName() const;
    QString stlFileName() const;
    QString posesFileName() const;

    QVector<SSIMRenderer::Pyramid> perspective;
    QVector<QImage> image;
    QVector<QImage> mask;
    QVector<QRect> crop;
    QVector<QRect> vertexCrop;
    QVector<QPoint> point1;
    QVector<QPoint> point2;

    QVector<QVector3D> rotation;
    QVector<QVector3D> translation;

    QVector<QVector<bool> > overflow;

    bool verbose;
    bool hausdorff;
    bool vertexMetric;
    bool rotate;
    bool mirror;
    bool manualInit;
    bool refLength;
    bool length;
    int views;
    int fragments;    

    QString method;
    QString algorithm;
    QString path;
    QString imagesPath;
    QString measurementFile;
    QString refModelFileName;
    bool saveImages;
    bool saveMeasurement;

    bool outCrop;
    bool transform;
    bool lengthFix;
    bool cutPlanes;

private:
    bool read(QIODevice *device);

    QString shapeModel;
    QString densityModel;
    QString tetraModel;
    QString stl;
    QString poseFile;
};

#endif // XMLPARSER_H
