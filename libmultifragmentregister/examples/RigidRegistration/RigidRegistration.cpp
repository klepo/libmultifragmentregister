/**
 * @file        RigidRegistration.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        7 February 2017
 *
 * @brief       Rigid registration of a bone fragment
 *
 * @example     FeatureRegistration.cpp
 *
 * This example shows rigid registration of a polygonal model
 * of a bone fragment using the feature-based registration.
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
    RegistrationType * registration = RegistrationType::New(1, 2, new TSimpleVertexMetric);


    QString         meshFileName = DATA_PATH "/prox.ply";
    QString       saveImagesPath = DATA_PATH "/example4_images";

    QString       image1FileName = DATA_PATH "/sproxIM000008.png";
    QString       image2FileName = DATA_PATH "/sproxIM000009.png";

    QString perspective1FileName = DATA_PATH "/pIM000008.csv";
    QString perspective2FileName = DATA_PATH "/pIM000009.csv";

    QString   polygonalModelName = DATA_PATH "/example4";

    SSIMRenderer::PLYMeshFile * meshFile;
    SSIMRenderer::CSVPyramidFile * perspective1File;
    SSIMRenderer::CSVPyramidFile * perspective2File;

    try {
        meshFile         = new SSIMRenderer::PLYMeshFile(meshFileName);
        perspective1File = new SSIMRenderer::CSVPyramidFile(perspective1FileName);
        perspective2File = new SSIMRenderer::CSVPyramidFile(perspective2FileName);
    }
    catch (std::exception &e) {
        // Wrong file
        qFatal(e.what());
        exit(EXIT_FAILURE);
    }


    registration->enableDensity(false);
    registration->setMeshModel(meshFile);

    QVector<QSize> sizes(2);
    sizes[0] = QImage(image1FileName).scaledToHeight(512).size();
    sizes[1] = QImage(image2FileName).scaledToHeight(512).size();

    registration->setSizes(sizes);
    QVector<QRect> crops(2);
    crops[0] = QRect(QPoint(0, 0), sizes.at(0));
    crops[1] = QRect(QPoint(0, 0), QSize(sizes.at(1).width(), 512)); //474
    registration->setCrops(crops);

    registration->enableMirroring(false);

    QVector<QVector3D> rotations(1);
    rotations[0] = QVector3D(0,0,0);
    QVector<QVector3D> translations(1);
    translations[0] = QVector3D(0,0,0);

    registration->setRotations(rotations);
    registration->setTranslations(translations);

    QVector<SSIMRenderer::Pyramid> perspectives(2);
    perspectives[0] = *perspective1File;
    perspectives[1] = *perspective2File;

    registration->setPerspectives(perspectives);

    QVector<QImage> images(2);
    images[0] = RegistrationType::maskImage(QImage(image1FileName).scaledToHeight(512).copy(crops.at(0)));
    images[1] = RegistrationType::maskImage(QImage(image2FileName).scaledToHeight(512).copy(crops.at(1)));

    registration->setImages(images);
    registration->setCrops(crops);

    TDefaultObserver * observer = new TDefaultObserver;
    observer->setVerbose(true);
    observer->enableImages(true);
    observer->setImagesPath(saveImagesPath);
    observer->setRefImages(images);
    observer->setWholeBones(true);
    registration->setObserver(observer);

    registration->optimizePose();

    registration->exportSTL(polygonalModelName);
    qDebug() << registration->getRotations().at(0);
    qDebug() << registration->getTranslations().at(0);

    return EXIT_SUCCESS;
}
