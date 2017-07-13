/**
 * @file        poseestimator.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The implementation file containing the PoseEstimator class.
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

#include "poseestimator.h"
#include <assert.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cmath>
#include <QDebug>

#define PI 3.14159265

float radian_to_degree(double  ENTER) {
  double Pi = 3.14159265;
  float degrees = (ENTER * 180) / Pi;
  return degrees;
}

/**
 * @brief PoseEstimator::PoseEstimator Default constructor.
 */
PoseEstimator::PoseEstimator()
{
}

/**
 * @brief PoseEstimator::PoseEstimator Constructor of the class. Provides the pose estimation.
 * @param images AP and LAT radiographic images with segmented bones
 * @param clbs Calibrations corresponding to the input images
 * @param points First points of the fracture lines
 * @param points2 Second points of the fracture lines
 * @param margin Margin for the translation estimation
 * @param angle An angle for the Z rotation, which is not estimated by this class
 * @param prox Proximal or distal fragment
 */
PoseEstimator::PoseEstimator(const QVector<QImage> & images,
              const QVector<SSIMRenderer::Pyramid> & clbs,
              const QVector<QPoint> & points,
              const QVector<QPoint> & points2,
              double margin,
              double angle,
              bool prox):
    myQImages(images),
    myCalibrations(clbs),
    myPoints(points)
{
    const int N = 2;

    assert(images.size() == N);
    assert(clbs.size() == N);
    assert(points.size() == N);

    myCvImages.resize(N);
    QVector<cv::Mat_<double> > planes(2);
    QVector<bool> firstIsHead(2);
    QVector<cv::Mat_<double> > fracPts(2);
    QVector<cv::Mat_<double> > fracPts2(2);
    QVector<cv::Mat_<double> > ms(2);
    size.resize(2);

    myNormal.resize(2);
    myD.resize(2);
    mySign.resize(2);

    for(int i = 0; i < N; i++)
    {
        myQImages[i] = myQImages.at(i).convertToFormat(QImage::Format_RGB888);
        myCvImages[i] = cv::Mat(myQImages.at(i).height(),
                                myQImages.at(i).width(),
                                CV_8UC3,
                                const_cast<uchar*>(myQImages.at(i).scanLine(0)),
                                myQImages.at(i).bytesPerLine());

        // Morphological Gradient
        // http://docs.opencv.org/2.4/doc/tutorials/imgproc/opening_closing_hats/opening_closing_hats.html

        cv::Mat channels[3];
        cv::split(myCvImages[i], channels);
        myCvImages[i] = channels[0];
        cv::threshold(myCvImages[i], myCvImages[i], 10, 255, cv::THRESH_BINARY);
        cv::Mat element = cv::Mat::ones(2,2, CV_8UC1);

        cv::morphologyEx(myCvImages[i], myCvImages[i], cv::MORPH_GRADIENT, element);

        std::vector<cv::Point> idx;
        cv::findNonZero(myCvImages[i], idx);

        cv::Mat_<double> pts(idx.size(), 3, 1);
        for(unsigned int j = 0; j < idx.size(); j++)
        {
            pts(j, 0) = idx.at(j).x;
            pts(j, 1) = idx.at(j).y;
        }

        // Prepocitani souradnic do 3D prostoru
        QVector3D a = clbs.at(i).getRightTop()   - clbs.at(i).getLeftTop();
        QVector3D b = clbs.at(i).getLeftBottom() - clbs.at(i).getLeftTop();
        QVector3D c = clbs.at(i).getLeftTop();

        a /= images.at(i).width();
        b /= images.at(i).height();

        cv::Mat_<double> m(3, 3);
        m(0, 0) = a.x(); m(0, 1) = a.y(); m(0, 2) = a.z();
        m(1, 0) = b.x(); m(1, 1) = b.y(); m(1, 2) = b.z();
        m(2, 0) = c.x(); m(2, 1) = c.y(); m(2, 2) = c.z();

        ms[i] = m;

        // obrysy fragmentu ve 3d
        cv::Mat_<double> pts3d = pts*m;

        // vertex crop bod ve 3d
        cv::Mat_<double> pt(1, 3, 1);
        pt(0, 0) = points.at(i).x();
        pt(0, 1) = points.at(i).y();
        cv::Mat_<double> pt3 = pt*m;

        // crop bod ve 3d
        cv::Mat_<double> pt_2(1, 3, 1);
        pt_2(0, 0) = points2.at(i).x();
        pt_2(0, 1) = points2.at(i).y();
        cv::Mat_<double> pt3_2 = pt_2*m;

        // PCA
        // Promitnuti bodu na osu fragmentu
        cv::PCA pca(pts3d, cv::noArray(), cv::PCA::DATA_AS_ROW, 0);        
        cv::Mat_<double> pri = pca.project(pts3d);
        cv::Mat_<double> pr = pca.project(pt3);
        cv::Mat_<double> pr_2 = pca.project(pt3_2);

        planes[i] = cv::Mat_<double>(4, 3);

        // nalezeni nejnizsiho a nejvyssiho bodu na PCA ose
        double min, max;
        cv::minMaxLoc(pri(cv::Range(0, pri.rows), cv::Range(0, 1)), &min, &max);

        cv::Mat_<double> scores = cv::Mat_<double>::zeros(4, 3);
        scores(0, 0) = min;
        scores(1, 0) = max;
        scores(2, 0) = pr(0, 0);
        scores(3, 0) = pr_2(0, 0);

        // je od bodu zlomu dal minimum? Pokud ano, minimum bod na hlavici
        // "first" znamena prvni radek v matici
        firstIsHead[i] = (pr(0, 0) - min)*(pr(0, 0) - min) > (pr(0, 0) - max)*(pr(0, 0) - max);

        pca.backProject(scores, planes[i]);
        fracPts[i] = cv::Mat_<double> (1,3);
        fracPts[i](0, 0) = planes[i](2, 0);
        fracPts[i](0, 1) = planes[i](2, 1);
        fracPts[i](0, 2) = planes[i](2, 2);

        fracPts2[i] = cv::Mat_<double> (1,3);
        fracPts2[i](0, 0) = planes[i](3, 0);
        fracPts2[i](0, 1) = planes[i](3, 1);
        fracPts2[i](0, 2) = planes[i](3, 2);

        planes[i](2, 0) = clbs.at(i).getEye().x();
        planes[i](2, 1) = clbs.at(i).getEye().y();
        planes[i](2, 2) = clbs.at(i).getEye().z();


        // rovina REZU je dana body: camera, fracPts, fracPts2
        cv::Point3_<double> p_camera(  planes[i](2,0), planes[i](2,1), planes[i](2,2));
        cv::Point3_<double> p_fracPts( pt3(0,0), pt3(0,1), pt3(0,2));
        cv::Point3_<double> p_fracPts2(pt3_2(0,0), pt3_2(0,1), pt3_2(0,2));


        cv::Point3_<double> p_n = (p_fracPts-p_camera).cross(p_fracPts2-p_camera);
        myNormal[i] = QVector3D(p_n.x, p_n.y, p_n.z);
        double d = -p_n.dot(p_camera);
        myD[i] = d;
        // Bod hlavice
        cv::Point3_<double> p_head;

        if(firstIsHead[i]) {
            p_head = cv::Point3_<double>(planes[i](0,0), planes[i](0,1), planes[i](0,2));
        }
        else {
            p_head = cv::Point3_<double>(planes[i](1,0), planes[i](1,1), planes[i](1,2));
        }

        mySign[i] = p_head.dot(p_n) + d;
    }

    // Vypocet roviny z plane[0]   
    cv::Point3_<double> p1(planes[0](0,0), planes[0](0,1), planes[0](0,2));
    cv::Point3_<double> p2(planes[0](1,0), planes[0](1,1), planes[0](1,2));
    cv::Point3_<double> p3(planes[0](2,0), planes[0](2,1), planes[0](2,2));
    cv::Point3_<double> n = (p3-p1).cross(p3-p2);

    cv::Point3_<double> pb1(planes[1](0,0), planes[1](0,1), planes[1](0,2));
    cv::Point3_<double> pb2(planes[1](1,0), planes[1](1,1), planes[1](1,2));
    cv::Point3_<double> pb3(planes[1](2,0), planes[1](2,1), planes[1](2,2));

    cv::Point3_<double> p1xyz = intersect(n, p1, pb1, pb3);
    cv::Point3_<double> p2xyz = intersect(n, p1, pb2, pb3);

    cv::Point3d frcPt(fracPts[0](0, 0), fracPts[0](0, 1), fracPts[0](0, 2));
    cv::Point3d frcN = (p3 - frcPt).cross(pb3 - frcPt);
    // LAT vertex crop dopocitany z AP vertex crop
    cv::Point3d frcPt2 = intersect(frcN, frcPt, pb1, pb2);
    cv::Mat_<double> mf(1,3);
    mf(0,0) = frcPt2.x;
    mf(0,1) = frcPt2.y;
    mf(0,2) = frcPt2.z;

    // Prevod do 2D a ulozeni
    cv::Mat_<double> mfi =  mf*ms.at(1).inv();
    QPoint res(mfi(0,0), mfi(0,1));
    myPoint = res;

    // Vertex crop lezici na 3D ose kosti
    cv::Point3d frc3d = intersect(frcN, frcPt, p2xyz, p1xyz);

    cv::Point3_<double> centroid;
    if(firstIsHead[0])
    {
        // vektor od hlavice k lomu standardizovany na 1 velikost
        cv::Point3_<double> v = (p2xyz - p1xyz) / cv::norm(p2xyz - p1xyz);
        // presunusti pocatku vektoru zpatky do hlavice
        centroid = v * margin + p1xyz;
        myLength = cv::norm(frc3d - p1xyz);
    }
    else
    {
        cv::Point3_<double> v = (p1xyz - p2xyz) / cv::norm(p1xyz - p2xyz);
        centroid = v * margin + p2xyz;
        myLength = cv::norm(frc3d - p2xyz);
    }

    // Pro proximalni fragment je potreba pocitat s delkou od point2!
    if(prox)
    {
        cv::Point3d frcPt_2(fracPts2[0](0, 0), fracPts2[0](0, 1), fracPts2[0](0, 2));
        cv::Point3d frcN_2 = (p3 - frcPt_2).cross(pb3 - frcPt_2);
        cv::Point3d frc3d_2 = intersect(frcN_2, frcPt_2, p2xyz, p1xyz);

        if(firstIsHead[0])
            myLength = cv::norm(frc3d_2 - p1xyz);
        else
            myLength = cv::norm(frc3d_2 - p2xyz);
    }

    myTranslation.setX(centroid.x);
    myTranslation.setY(centroid.y);
    myTranslation.setZ(centroid.z);


    cv::Mat_<double> Rz = (cv::Mat_<double>(3,3)
                  << cos(angle * PI / 180.0), -sin(angle * PI / 180.0), 0,
                     sin(angle * PI / 180.0),  cos(angle * PI / 180.0), 0,
                              0,           0, 1);

    cv::Mat_<double> mp1xyz = cv::Mat_<double>(p1xyz).t()*Rz;
    p1xyz.x = mp1xyz(0,0);
    p1xyz.y = mp1xyz(0,1);
    p1xyz.z = mp1xyz(0,2);

    cv::Mat_<double> mp2xyz = cv::Mat_<double>(p2xyz).t()*Rz;
    p2xyz.x = mp2xyz(0,0);
    p2xyz.y = mp2xyz(0,1);
    p2xyz.z = mp2xyz(0,2);

    cv::Point3_<double> v = (p1xyz - p2xyz) / cv::norm(p1xyz - p2xyz);

    myRotation[0] =  -radian_to_degree(acos(v.y)) + 90 + 360;
    myRotation[1] =  radian_to_degree(acos(v.x)) - 90 + 360;

    myRotation[2] = angle+360;

    if(myRotation[0] > 360) myRotation[0] -= 360;
    if(myRotation[1] > 360) myRotation[1] -= 360;
    if(myRotation[0] > 360) myRotation[0] -= 360;
    if(myRotation[1] > 360) myRotation[1] -= 360;
    if(myRotation[2] > 360) myRotation[2] -= 360;
    if(myRotation[2] > 360) myRotation[2] -= 360;

}

/**
 * @brief PoseEstimator::intersect Computes intersection of the input plane and line
 * @param n Normal vector of the plane
 * @param v0 Vector inside the plane
 * @param p0 The first point of the line
 * @param p1 The second point of the line
 * @return Intersection point
 */
cv::Point3_<double> PoseEstimator::intersect(cv::Point3_<double> n,
                                             cv::Point3_<double> v0,
                                             cv::Point3_<double> p0,
                                             cv::Point3_<double> p1)
{
    cv::Point3_<double> u = p1 - p0;
    cv::Point3_<double> w = p0 - v0;
    double D = n.dot(u);
    double N = -n.dot(w);
    double sI = N / D;
    cv::Point3_<double> I = p0 + sI*u;
    return I;
}

/**
 * @brief PoseEstimator::rotation Returns the estimated rotation of the fragment
 * @return Vector of angles
 */
QVector3D PoseEstimator::rotation()
{
    return myRotation;
}

/**
 * @brief PoseEstimator::translation Returns the estimated translation of the fragment
 * @return Vector of distances
 */
QVector3D PoseEstimator::translation()
{
    return myTranslation;
}

/**
 * @brief PoseEstimator::~PoseEstimator Destructor of the class.
 */
PoseEstimator::~PoseEstimator()
{
}

