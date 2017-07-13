/**
 * @file        poseestimator.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The header file containing the PoseEstimator class.
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

#ifndef POSEESTIMATOR_H
#define POSEESTIMATOR_H

#include "ssimrenderer.h"
#include <QVector3D>
#include <QImage>
#include <opencv2/core.hpp>

/**
 * @brief The PoseEstimator class
 */
class PoseEstimator
{
public:
    PoseEstimator(const QVector<QImage> & images,
                  const QVector<SSIMRenderer::Pyramid> & clbs,
                  const QVector<QPoint> & points,
                  const QVector<QPoint> &points2,
                  double margin, double angle, bool prox);
    PoseEstimator();

    ~PoseEstimator();

    // Estimated rotation
    QVector3D rotation();
    // Estimated translation
    QVector3D translation();

    // Description of fracture planes
    QVector<QVector3D> myNormal;
    QVector<double> myD;
    QVector<double> mySign;

    // Fracture point reprojected from the AP image to the LAT image
    QPoint myPoint;
    // Estimated length of the fragment
    double myLength;

    QVector<QSize> size;

private:
    // Intersection of line and plane
    cv::Point3_<double> PoseEstimator::intersect(cv::Point3_<double> n,
                                                 cv::Point3_<double> v0,
                                                 cv::Point3_<double> p0,
                                                 cv::Point3_<double> p1);

    QVector<QImage> myQImages;
    QVector<cv::Mat> myCvImages;
    QVector<SSIMRenderer::Pyramid> myCalibrations;
    QVector<QPoint> myPoints;

    QVector3D myRotation;
    QVector3D myTranslation;
};

#endif // POSEESTIMATOR_H
