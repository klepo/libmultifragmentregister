/**
 * @file        MultiFragmentFeatureRegistration.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        16 December 2016
 *
 * @brief       The last example shows fractured bone reconstruction with accurate length estimation
 *
 * @example     MultiFragmentFeatureRegistration.cpp
 *
 * The bone is reconstructed from one proximal and one distal fragment. The vertex
 * metric ensures the length of the reconstructed bone is estimated accurately.
 * The images from the registration are saved to the "examples3_images" folder,
 * the reconstructed and reduced polygonal model is stored as "example3.stl".
 * For more details, please see previous examples.
 *
 */

#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <ssimrenderer.h>
#include <libmultifragmentregister.h>

#include <ImageMetric/tsimplemetric.h>
#include <VertexMetric/tsimplevertexmetric.h>
#include <Observer/TDefaultObserver.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath("plugins");


    typedef LibMultiFragmentRegister<TSimpleMetric> RegistrationType;
    RegistrationType * registration = RegistrationType::New(2, 2, new TSimpleVertexMetric);


    QString         meshFileName = DATA_PATH "/mean.230.a10.mesh";
    QString      densityFileName = DATA_PATH "/density.b2.mat";
    QString       saveImagesPath = DATA_PATH "/example3_images";

    QString       image1FileName = DATA_PATH "/226.7.proximal.png";
    QString       image2FileName = DATA_PATH "/226.10.proximal.png";
    QString       image3FileName = DATA_PATH "/226.7.distal.png";
    QString       image4FileName = DATA_PATH "/226.10.distal.png";

    QString perspective1FileName = DATA_PATH "/226.7.png.csv";
    QString perspective2FileName = DATA_PATH "/226.10.png.csv";

    QString        pose1FileName = DATA_PATH "/transform.226.csv6.csv";
    QString        pose2FileName = DATA_PATH "/transform.226.csv7.csv";
    QString        shapeFileName = DATA_PATH "/shape.mat";

    QString   polygonalModelName = DATA_PATH "/example3";

    SSIMRenderer::Lm6MeshFile * meshFile;
    SSIMRenderer::MatStatisticalDataFile * shapeFile;
    SSIMRenderer::MatStatisticalDataFile * densityFile;
    QVector<float> pose1File;
    QVector<float> pose2File;
    QVector<float> perspective1File;
    QVector<float> perspective2File;

    try {
        meshFile         = new SSIMRenderer::Lm6MeshFile(meshFileName);
        shapeFile        = new SSIMRenderer::MatStatisticalDataFile(shapeFileName);
        densityFile      = new SSIMRenderer::MatStatisticalDataFile(densityFileName);
        pose1File        = RegistrationType::loadDataFromCSVFile(pose1FileName);
        pose2File        = RegistrationType::loadDataFromCSVFile(pose2FileName);
        perspective1File = RegistrationType::loadDataFromCSVFile(perspective1FileName);
        perspective2File = RegistrationType::loadDataFromCSVFile(perspective2FileName);
    }
    catch (std::exception &e) {
        // Wrong file
        qFatal(e.what());
        exit(EXIT_FAILURE);
    }

    registration->enableDensity(false);
    registration->setMeshModel(meshFile);
    registration->setDensityModel(densityFile);
    registration->setShapeModel(shapeFile);

    QVector<QSize> sizes(4);
    sizes[0] = QImage(image1FileName).scaledToHeight(512).size();
    sizes[1] = QImage(image2FileName).scaledToHeight(512).size();
    sizes[2] = QImage(image3FileName).scaledToHeight(512).size();
    sizes[3] = QImage(image4FileName).scaledToHeight(512).size();

    registration->setSizes(sizes);
    QVector<QRect> crops(4);
    crops[0] = QRect(QPoint(0, 256), QPoint(511, 511));
    crops[1] = QRect(QPoint(0, 256), QPoint(511, 511));
    crops[2] = QRect(QPoint(0, 0), QPoint(511, 255));
    crops[3] = QRect(QPoint(0, 0), QPoint(511, 255));
    registration->setCrops(crops);

    QVector<QRectF> openGlCrops(4);
    openGlCrops[0] = QRectF(-1, 0, 2, 1);
    openGlCrops[1] = QRectF(-1, 0, 2, 1);
    openGlCrops[2] = QRectF(-1, -1, 2, 1);
    openGlCrops[3] = QRectF(-1, -1, 2, 1);
    registration->setOpenGLCrops(openGlCrops);

    registration->enableMirroring(false);

    QVector<QVector3D> rotations(2);
    rotations[0] = QVector3D(pose1File[0], pose1File[1], pose1File[2]);
    rotations[1] = QVector3D(pose2File[0], pose2File[1], pose2File[2]);
    QVector<QVector3D> translations(2);
    translations[0] = QVector3D(pose1File[3], pose1File[4], pose1File[5]);
    translations[1] = QVector3D(pose2File[3], pose2File[4], pose2File[5]);

    registration->setRotations(rotations);
    registration->setTranslations(translations);

    SSIMRenderer::Pyramid perspective1(
            QVector3D(perspective1File[0],  perspective1File[4],  perspective1File[2]),
            QVector3D(perspective1File[3],  perspective1File[1],  perspective1File[5]),
            QVector3D(perspective1File[6],  perspective1File[10], perspective1File[8]),
            QVector3D(perspective1File[9],  perspective1File[7],  perspective1File[11]),
            QVector3D(perspective1File[15], perspective1File[16], perspective1File[17])
        );

    SSIMRenderer::Pyramid perspective2(
            QVector3D(perspective2File[0],  perspective2File[4],  perspective2File[2]),
            QVector3D(perspective2File[3],  perspective2File[1],  perspective2File[5]),
            QVector3D(perspective2File[6],  perspective2File[10], perspective2File[8]),
            QVector3D(perspective2File[9],  perspective2File[7],  perspective2File[11]),
            QVector3D(perspective2File[15], perspective2File[16], perspective2File[17])
        );

    QVector<SSIMRenderer::Pyramid> perspectives(4);
    perspectives[0] = perspective1;
    perspectives[1] = perspective2;
    perspectives[2] = perspective1;
    perspectives[3] = perspective2;

    registration->setPerspectives(perspectives);

    QVector<QImage> images(4);
    images[0] = RegistrationType::maskImage(QImage(image1FileName).scaledToHeight(512).copy(crops.at(0)));
    images[1] = RegistrationType::maskImage(QImage(image2FileName).scaledToHeight(512).copy(crops.at(1)));
    images[2] = RegistrationType::maskImage(QImage(image3FileName).scaledToHeight(512).copy(crops.at(2)));
    images[3] = RegistrationType::maskImage(QImage(image4FileName).scaledToHeight(512).copy(crops.at(3)));

    registration->setImages(images);
    registration->setCrops(crops);

    int pn = registration->getShapeParams().size();
    registration->setShapeParams(QVector<float>(pn, 0));

    TDefaultObserver * observer = new TDefaultObserver;
    observer->setVerbose(true);
    observer->enableImages(true);
    observer->setImagesPath(saveImagesPath);
    observer->setRefImages(images);
    observer->setWholeBones(true);
    registration->setObserver(observer);

    registration->optimizePose();
    registration->optimizePoseShapeVertex(5);
    registration->optimizePoseShapeVertex();

    registration->exportSTL(polygonalModelName);

    return EXIT_SUCCESS;
}
