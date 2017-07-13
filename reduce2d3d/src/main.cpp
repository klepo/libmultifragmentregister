/**
 * @file        main.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The main file.
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

#define _USE_MATH_DEFINES
#include <cmath>

#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>

#include "libmultifragmentregister.h"
#include "ImageMetric/topenglnormalizedmutualinformationmetric.h"
#include "ImageMetric/topenglsquareddifferencesmetric.h"
#include "ImageMetric/tcpunormalizedmutualinformationmetric.h"
#include "ImageMetric/tcpusquareddifferencesmetric.h"
#include "ImageMetric/tsimplemetric.h"
#include "ImageMetric/tsimplemetricmask.h"

#include "Observer/TDefaultObserver.h"
#include "VertexMetric/tsquareddifferencesvertexmetric.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "poseestimator.h"
#include "xmlparser.h"
#include "cropestimator.h"
#include "hausdorffdistance.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath("plugins");

    //QString path = "D:/Traumatech/bug/selhalo";
    //QString path = "D:/Traumatech/bug/neselhalo";
    //QString path = "D:/Traumatech/bug/tibia";
    //QString file = "reduce.xml";

    //QString path = "D:/Traumatech/rsa/data/femur1c/reduce2D3D";
    //QString path = "D:/Traumatech/rsa/data/femur2b/reduce2D3D";
    //QString path = "D:/Traumatech/rsa/data/femur3/reduce2D3D";
    //QString path = "D:/Traumatech/rsa/data/humerus1/reduce2D3D";
    //QString path = "D:/Traumatech/rsa/data/humerus2/reduce2D3D";
    //QString path = "D:/Traumatech/rsa/data/humerus3/reduce2D3D";
    //QString path = "D:/Traumatech/rsa/data/tibia1b/reduce2D3D";

    QString file = "reduce.manual.xml";
    //QString file = "reduce.manual.length.xml";
    //QString file = "reduce.manual.reflength.xml";

    //QString path = "C:/Reduce2D3D_examples/femur/";
    QString path = "C:/Reduce2D3D_examples/tibia/";

    if(argc > 1 && argc != 3)
    {
        qDebug() << "Usage: reduce2d3d.exe <path> <file.xml>";
        exit(EXIT_FAILURE);
    }
    else if(argc == 3)
    {
        path = argv[1];
        file = argv[2];
    }

    xmlparser parser(path, file);
    SSIMRenderer::Lm6MeshFile * meshFile;
    SSIMRenderer::MatStatisticalDataFile * shapeFile;
    SSIMRenderer::MatStatisticalDataFile * densityFile; 

    try {
        meshFile         = new SSIMRenderer::Lm6MeshFile(parser.meshFileName());
        shapeFile        = new SSIMRenderer::MatStatisticalDataFile(parser.shapeFileName());
        densityFile      = new SSIMRenderer::MatStatisticalDataFile(parser.densityFileName());
    }
    catch (std::exception &e) {
        // Wrong file
        qFatal(e.what());
        exit(EXIT_FAILURE);
    }

    LibMultiFragmentRegisterAbstract * registration;

    typedef LibMultiFragmentRegister<TSimpleMetric> SimpleRegistration;
    typedef LibMultiFragmentRegister<TSimpleMetricMask> SimpleRegistrationMask;
    typedef LibMultiFragmentRegister<TOpenGLNormalizedMutualInformationMetric> OpenGLNmiRegistration;
    typedef LibMultiFragmentRegister<TOpenGLSquaredDifferencesMetric> OpenGLSsdRegistration;

    if(parser.method == "BW-PD")
    {
        registration = SimpleRegistration::New(parser.fragments, parser.views, new TSimpleVertexMetric);
    }
    else if(parser.method == "BW-PD-msk")
    {
        registration = SimpleRegistrationMask::New(parser.fragments, parser.views, new TSimpleVertexMetric);
    }
    else if(parser.method == "BW-SSD")
    {
        registration = OpenGLSsdRegistration::New(parser.fragments, parser.views, new TSquaredDifferencesVertexMetric);
        registration->enableDensity(false);
    }
    else
    {
        registration = OpenGLNmiRegistration::New(parser.fragments, parser.views, new TSimpleVertexMetric);
        registration->enableDensity(true);
    }

    registration->setMeshModel(meshFile);
    registration->setDensityModel(densityFile);
    registration->setShapeModel(shapeFile);
    registration->enableMirroring(parser.mirror);

    for(int i = 0; i < parser.image.size(); i++)
    {
        QVector3D hv = parser.perspective.at(i).getLeftTop() - parser.perspective.at(i).getLeftBottom();
        QVector3D wv = parser.perspective.at(i).getLeftTop() - parser.perspective.at(i).getRightTop();

        int h = hv.length() * 2; // pixel spacing = 0.5
        int w = wv.length() * 2;

        double hr = parser.image[i].height() / (double) h;
        double wr = parser.image[i].width()  / (double) w;

        parser.point1[i].setX(parser.point1[i].x() / wr);
        parser.point1[i].setY(parser.point1[i].y() / hr);

        parser.point2[i].setX(parser.point2[i].x() / wr);
        parser.point2[i].setY(parser.point2[i].y() / hr);

        parser.image[i] = parser.image[i].scaled(w, h);

        if(parser.method == "BW-PD-msk")
        {
            parser.mask[i] = parser.mask[i].scaled(w, h);
        }

        if(parser.manualInit)
        {
            QPoint topLeft = parser.crop[i].topLeft();
            QPoint bottomRight = parser.crop[i].bottomRight();

            topLeft.setX(topLeft.x() / wr);
            topLeft.setY(topLeft.y() / hr);
            bottomRight.setX(bottomRight.x() / wr);
            bottomRight.setY(bottomRight.y() / hr);

            parser.crop[i] = QRect(topLeft, bottomRight);

            topLeft = parser.vertexCrop[i].topLeft();
            bottomRight = parser.vertexCrop[i].bottomRight();

            topLeft.setX(topLeft.x() / wr);
            topLeft.setY(topLeft.y() / hr);
            bottomRight.setX(bottomRight.x() / wr);
            bottomRight.setY(bottomRight.y() / hr);

            parser.vertexCrop[i] = QRect(topLeft, bottomRight);
            parser.point2[i] = parser.point1[i];
        }
    }

    QVector<QSize> sizes(parser.image.size());
    for(int i = 0; i < sizes.size(); i++)
        sizes[i] = parser.image.at(i).size();
    registration->setSizes(sizes);


    QVector<QImage> image_prox(2), image_dist(2);
    QVector<SSIMRenderer::Pyramid> perspective_prox(2), perspective_dist(2);
    QVector<QPoint> pts_prox(2),  pts_dist(2);
    QVector<QPoint> pts2_prox(2), pts2_dist(2);

    qCopy(parser.image.begin(), parser.image.begin()+2, image_prox.begin());
    qCopy(parser.perspective.begin(), parser.perspective.begin()+2, perspective_prox.begin());
    qCopy(parser.point1.begin(), parser.point1.begin()+2, pts_prox.begin());
    qCopy(parser.point2.begin(), parser.point2.begin()+2, pts2_prox.begin());

    qCopy(parser.image.begin() + 2, parser.image.end(), image_dist.begin());
    qCopy(parser.perspective.begin() + 2, parser.perspective.end(), perspective_dist.begin());
    qCopy(parser.point1.begin() + 2, parser.point1.end(), pts_dist.begin());
    qCopy(parser.point2.begin() + 2, parser.point2.end(), pts2_dist.begin());

    PoseEstimator pose_prox(image_prox, perspective_prox, pts_prox, pts2_prox,  meshFile->getMaxVertex().z(), parser.rotation[0].z(), true);
    PoseEstimator pose_dist(image_dist, perspective_dist, pts_dist, pts2_dist, -meshFile->getMinVertex().z(), parser.rotation[1].z(), false);

    QVector<QPoint> point1 = parser.point1;
    if(!parser.manualInit)
    {
        point1[1] = pose_prox.myPoint;
        point1[3] = pose_dist.myPoint;
    }

    // OpenGL crops for mask computing
    QVector<QRectF> openGLCrops(parser.image.size());

    for(int i = 0; i < parser.image.size(); i++)
    {
        if(!parser.manualInit)
        {
            cropestimator crop(parser.image[i], point1[i], parser.point2[i], parser.overflow[i], i > 1);
            parser.crop[i] = crop.myCrop;
            parser.vertexCrop[i] = crop.myOpenGLCrop;
        }

        openGLCrops[i] = QRectF( 2 * parser.vertexCrop.at(i).x() / static_cast<double>(sizes.at(i).width()) - 1,
                                -2 *(parser.vertexCrop.at(i).y() + parser.vertexCrop.at(i).height()) / static_cast<double>(sizes.at(i).height()) + 1,
                                 2 * parser.vertexCrop.at(i).width() / static_cast<double>(sizes.at(i).width()),
                                 2 * parser.vertexCrop.at(i).height() / static_cast<double>(sizes.at(i).height()));
    }
    registration->setOpenGLCrops(openGLCrops);
    registration->setCrops(parser.crop);

    QVector<QImage> images(parser.image.size());
    if(parser.method == "BW-PD" || parser.method == "BW-PD-msk" || parser.method == "BW-SSD")
    {
        registration->enableDensity(false);
        for(int i = 0; i < images.size(); i++)
            images[i] = registration->maskImage(parser.image.at(i).copy(parser.crop.at(i)));
    }
    else {
        registration->enableDensity(true);
        for(int i = 0; i < images.size(); i++)
            images[i] = parser.image.at(i).copy(parser.crop.at(i));
    }

    if(parser.method == "BW-PD-msk")
    {
        QVector<QImage> masks(parser.mask.size());
        for(int i = 0; i < masks.size(); i++)
        {
            masks[i] = parser.mask.at(i).copy(parser.crop.at(i));
        }
        registration->setMasks(masks);
    }
    registration->setImages(images);
    registration->setPerspectives(parser.perspective);

    int pn = registration->getShapeParams().size();
    registration->setShapeParams(QVector<float>(pn, 0));

    // Novy odhad s upravenou delkou kosti
    QVector<float> params = registration->getStandardizedShapeParams();
    for(int i = 0; i < params.size(); i++)
        params[i] = 0;

    registration->setStandardizedShapeParams(params);
    registration->getImages();
    double D_avg = registration->getBoundingBoxSize().z();

    params[0] = 1;
    registration->setStandardizedShapeParams(params);
    registration->getImages();
    double D_sd1 = registration->getBoundingBoxSize().z();

    double D_x = pose_prox.myLength + pose_dist.myLength;
    double p_x = (D_x - D_avg) / (D_sd1 - D_avg);

    params[0] = p_x;
    registration->setStandardizedShapeParams(params);
    registration->getImages();
    float p_x_nonstd = registration->getShapeParams()[0];

    double refLength = 0;
    double refParam = 0;

    if(parser.hausdorff && parser.refLength)
    {
        struct model * mesh = read_model_file(parser.refModelFileName.toStdString().data());
        refLength = abs(mesh->bBox[0].z - mesh->bBox[1].z);
        double p_x = (refLength - D_avg) / (D_sd1 - D_avg);

        params[0] = p_x;
        registration->setStandardizedShapeParams(params);
        registration->getImages();
        refParam = registration->getShapeParams()[0];
    }

    if(parser.manualInit && !parser.length && !parser.refLength)
    {
        params[0] = 0;
        registration->setStandardizedShapeParams(params);
        registration->getImages();
    }


    PoseEstimator pose_proxD(image_prox, perspective_prox, pts_prox, pts2_prox,  registration->getBoundingBox().second.z(), parser.rotation[0].z(), true);
    PoseEstimator pose_distD(image_dist, perspective_dist, pts_dist, pts2_dist, -registration->getBoundingBox().first.z(), parser.rotation[1].z(), false);

    QVector<QVector3D> rotations, translations;
    rotations << pose_prox.rotation() << pose_dist.rotation();
    translations << pose_proxD.translation() << pose_distD.translation();

    if(!parser.manualInit)
    {
        registration->setRotations(rotations);
        registration->setTranslations(translations);
    }
    else
    {
        registration->setRotations(parser.rotation);
        registration->setTranslations(parser.translation);
    }

    TDefaultObserver * observer = new TDefaultObserver;
    observer->setVerbose(parser.verbose);
    observer->enableImages(parser.saveImages);
    observer->setImagesPath(parser.imagesPath + "/");
    observer->setRefImages(images);
    observer->setWholeBones(true);
    registration->setObserver(observer);

    QVector<int> imageCount(3, 0);
    QVector<int> iterationCount(3, 0);

    registration->optimizePose();

    imageCount[0]     = observer->getRenderedCount();
    iterationCount[0] = observer->getIterations();

    if(parser.fragments > 1 && parser.vertexMetric)
    {
        registration->optimizePoseShapeVertex(5);
        imageCount[1]     = observer->getRenderedCount() - imageCount[0];
        iterationCount[1] = observer->getIterations() - iterationCount[0];

        registration->optimizePoseShapeVertex();
        imageCount[2]     = observer->getRenderedCount()  - imageCount[0] - imageCount[1];
        iterationCount[2] = observer->getIterations() - iterationCount[0] - iterationCount[1];
    }
    else
    {
        registration->optimizePoseShape(5);
        imageCount[1]     = observer->getRenderedCount() - imageCount[0];
        iterationCount[1] = observer->getIterations() - iterationCount[0];

        registration->optimizePoseShape();
        imageCount[2]     = observer->getRenderedCount()  - imageCount[0] - imageCount[1];
        iterationCount[2] = observer->getIterations() - iterationCount[0] - iterationCount[1];
    }


    if(parser.saveMeasurement)
    {
        // Export xml with measurement results
        QFile measurementFile(parser.measurementFile);
        measurementFile.open(QIODevice::WriteOnly);
        QTextStream measureStream(&measurementFile);

        unsigned int NOA = 0, OA = 0, joint = 0, ref = 0;
        QVector<float> values       = registration->getValues();
        QVector<float> targetValues = registration->getTargetValues();
        for(int i = 0; i < values.size(); i++)
        {
            if(values.at(i) != targetValues.at(i))
                NOA++;
            if(values.at(i) != 0 && targetValues.at(i) != 0)
                OA++;
            if(values.at(i) != 0 || targetValues.at(i) != 0)
                joint++;
            if(targetValues.at(i) != 0)
                ref++;
        }

        QVector<float> vertexValues       = registration->getVertexValues();
        QVector<float> targetVertexValues = registration->getTargetVertexValues();
        unsigned int missedVertices = 0, wrongRenders = 0, wrongSum = 0;
        for(int i = 0; i < vertexValues.size(); i++)
        {
            if(vertexValues.at(i) != targetVertexValues.at(i))
            {
                missedVertices++;
                wrongRenders += abs(vertexValues.at(i) - targetVertexValues.at(i)) / 2;
                wrongSum += (vertexValues.at(i) - targetVertexValues.at(i)) / 2;
            }
        }

        double regie = observer->getRegistrationTime() - (observer->getMetricTime() + observer->getRenderingTime());

        float * v = NULL;
        int nv = 0;
        int * t = NULL;
        int nt = 0;

        double max = 0, mean = 0, rms = 0;
        double max_1 = 0, mean_1 = 0, rms_1 = 0;
        double max_2 = 0, mean_2 = 0, rms_2 = 0;


        double refLength = 0;
        if(parser.hausdorff)
        {
            registration->vertices(v, nv, QVector3D(), QVector3D());
            registration->triangles(t, nt);

            struct model m = meshModel(v, nv, t, nt);
            struct model m2 = hausdorffDistance(m, parser.refModelFileName.toStdString().data(), max, mean, rms, max_1, mean_1, rms_1, max_2, mean_2, rms_2);
            refLength = abs(m2.bBox[0].z - m2.bBox[1].z);
        }

        measureStream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
        measureStream << "<measurement>\r\n";
        measureStream << "  <noa pixels=\"" + QString::number(NOA) + "\" area=\"" + QString::number(NOA * 0.5 * 0.5) + "\" n=\"" + QString::number(values.count()) + "\" components=\"" + QString::number(registration->getStandardizedShapeParams().count()) + "\" />\r\n";
        measureStream << "  <oa pixels=\""  + QString::number(OA) + "\" area=\"" + QString::number(OA * 0.5 * 0.5) + "\" ratio=\"" + QString::number(NOA / (double)OA) + "\" />\r\n";
        measureStream << "  <joint pixels=\"" + QString::number(joint) + "\" area=\"" + QString::number(joint * 0.5 * 0.5) + "\"  ratio=\"" + QString::number(NOA / (double)joint) + "\" />\r\n";
        measureStream << "  <ref pixels=\"" + QString::number(ref) + "\" area=\"" + QString::number(ref * 0.5 * 0.5) + "\"  ratio=\"" + QString::number(NOA / (double)ref) + "\" />\r\n";
        measureStream << "  <vertices missed=\"" + QString::number(missedVertices) + "\" wrongRenders=\"" + QString::number(wrongRenders) + "\" sum=\"" + QString::number(wrongSum) + "\" vertices=\"" + QString::number(meshFile->getNumberOfVertices()) + "\" />\r\n";
        measureStream << "  <iterations first=\"" + QString::number(iterationCount[0]) + "\" second=\"" + QString::number(iterationCount[1]) + "\" third=\"" + QString::number(iterationCount[2]) + "\" sum=\"" + QString::number(observer->getIterations()) + "\" />\r\n";
        measureStream << "  <images first=\"" + QString::number(imageCount[0]) + "\" second=\"" + QString::number(imageCount[1]) + "\" third=\"" + QString::number(imageCount[2]) + "\" overall=\"" + QString::number(observer->getRenderedCount()) + "\" />\r\n";
        measureStream << "  <rendering time=\"" + QString::number(observer->getRenderingTime()) + "\" count=\"" + QString::number(observer->getRenderedCount()) + "\" timePerUnit=\"" + QString::number(observer->getTimePerRender()) + "\" />\r\n";
        measureStream << "  <metric time=\"" + QString::number(observer->getMetricTime()) + "\" count=\"" + QString::number(observer->getMetricCount()) + "\" timePerUnit=\"" + QString::number(observer->getTimePerMetric()) + "\" />\r\n";
        measureStream << "  <time regie=\"" + QString::number(regie) + "\" overall=\"" + QString::number(observer->getRegistrationTime()) + "\" />\r\n";
        measureStream << "  <length mean=\"" + QString::number(D_avg) + "\" estimated=\"" + QString::number(D_x) + "\" param=\"" + QString::number(p_x_nonstd) + "\" paramstd=\"" + QString::number(p_x) + "\" final=\"" + QString::number(registration->getBoundingBoxSize().z()) + "\" refLength=\"" + QString::number(refLength) + "\" refParam=\"" + QString::number(refParam) + "\" />\r\n";

        if(parser.hausdorff)
        {
            measureStream << "  <hausdorff max=\"" + QString::number(max) + "\" mean=\"" + QString::number(mean) + "\" rms=\"" + QString::number(rms) + "\" max1=\"" + QString::number(max_1) + "\" mean1=\"" + QString::number(mean_1) + "\" rms1=\"" + QString::number(rms_1) + "\" max2=\"" + QString::number(max_2) + "\" mean2=\"" + QString::number(mean_2) + "\" rms2=\"" + QString::number(rms_2) + "\">\r\n";
        }

        measureStream << "</measurement>\r\n";

        measureStream.flush();
        measurementFile.close();
    }

    // Export xml with poses
    QFile outfile(parser.posesFileName());
    outfile.open(QIODevice::WriteOnly);
    QTextStream out(&outfile);
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    out << "<poses>\n";

    QVector<QVector3D> r = registration->getRotations();
    QVector<QVector3D> t = registration->getTranslations();
    for(int i = 0; i < parser.fragments; i++)
    {
        out << "  <fragment>\n";
        out << "    <rotation x=\"" << r.at(i).x() << "\" y=\"" << r.at(i).y() << "\" z=\"" << r.at(i).z() << "\" />\n";
        out << "    <translation x=\"" << t.at(i).x() << "\" y=\"" << t.at(i).y() << "\" z=\"" << t.at(i).z() << "\" />\n";
        out << "  </fragment>\n";
    }

    out << "</poses>\n";

    outfile.close();
    registration->getImages();
    if(!parser.outCrop && !parser.transform)
        registration->exportSingleStl(parser.stlFileName());
    else
        registration->exportEachStl(parser.stlFileName(),
                                    parser.transform,
                                    parser.outCrop,
                                    parser.cutPlanes,
                                    pose_prox.myNormal + pose_dist.myNormal,
                                    pose_prox.myD + pose_dist.myD,
                                    pose_prox.mySign + pose_dist.mySign);

    return 0;
}
