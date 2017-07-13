/**
 * @file        DensityRegistration.cpp
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        16 December 2016
 *
 * @brief       This example performs single fragment density-based 2D-3D registration
 *
 * @example     DensityRegistration.cpp
 *
 * This example shows an atlas-based reconstruction of 3D polygonal model
 * from 2 radiographs. The radiographs, X-ray calibrations and initial
 * pose estimation are required for the reconstruction. During the optimization
 * the iteration number and current value of similarity metric are displayed
 * to standard output. Rendered images are overlayed over the original radiographs
 * and stored to the "example1_images" folder at the end of each iteration;
 * the original bone is colored red, the shape model is rendered in green color.
 * Resulting polygonal model is saved as "example1.stl". The model can be compared
 * with the model "226.ply", which is obtained from CT image.
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

#include <ImageMetric/topenglnormalizedmutualinformationmetric.h>
#include <VertexMetric/tsimplevertexmetric.h>
#include <Observer/TDefaultObserver.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    a.addLibraryPath("plugins");


    typedef LibMultiFragmentRegister<TOpenGLNormalizedMutualInformationMetric> RegistrationType;
    RegistrationType * registration = RegistrationType::New(1, 2, new TSimpleVertexMetric);


    QString         meshFileName = DATA_PATH "/mean.230.a10.mesh";
    QString      densityFileName = DATA_PATH "/density.b2.mat";
    QString       saveImagesPath = DATA_PATH "/example1_images";

    QString       image1FileName = DATA_PATH "/226.7.png";
    QString       image2FileName = DATA_PATH "/226.10.png";

    QString perspective1FileName = DATA_PATH "/226.7.png.csv";
    QString perspective2FileName = DATA_PATH "/226.10.png.csv";

    QString         poseFileName = DATA_PATH "/transform.226.csv7.csv";
    QString        shapeFileName = DATA_PATH "/shape.mat";

    QString   polygonalModelName = DATA_PATH "/example1";

    SSIMRenderer::Lm6MeshFile * meshFile;
    SSIMRenderer::MatStatisticalDataFile * shapeFile;
    SSIMRenderer::MatStatisticalDataFile * densityFile;
    QVector<float> poseFile;
    QVector<float> perspective1File;
    QVector<float> perspective2File;

    try {
        meshFile         = new SSIMRenderer::Lm6MeshFile(meshFileName);
        shapeFile        = new SSIMRenderer::MatStatisticalDataFile(shapeFileName);
        densityFile      = new SSIMRenderer::MatStatisticalDataFile(densityFileName);
        poseFile         = RegistrationType::loadDataFromCSVFile(poseFileName);
        perspective1File = RegistrationType::loadDataFromCSVFile(perspective1FileName);
        perspective2File = RegistrationType::loadDataFromCSVFile(perspective2FileName);
    }
    catch (std::exception &e) {
        // Wrong file
        qFatal(e.what());
        exit(EXIT_FAILURE);
    }

    registration->enableDensity(true);
    registration->setMeshModel(meshFile);
    registration->setDensityModel(densityFile);
    registration->setShapeModel(shapeFile);

    QVector<QSize> sizes(2);
    sizes[0] = QImage(image1FileName).scaledToHeight(512).size();
    sizes[1] = QImage(image2FileName).scaledToHeight(512).size();

    registration->setSizes(sizes);
    QVector<QRect> crops(2);
    crops[0] = QRect(QPoint(0, 0), QPoint(511, 511));
    crops[1] = QRect(QPoint(0, 0), QPoint(511, 511));
    registration->setCrops(crops);

    registration->enableMirroring(false);

    QVector<QVector3D> rotations(1);
    rotations[0] = QVector3D(poseFile[0], poseFile[1], poseFile[2]);
    QVector<QVector3D> translations(1);
    translations[0] = QVector3D(poseFile[3], poseFile[4], poseFile[5]);

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

    QVector<SSIMRenderer::Pyramid> perspectives(2);
    perspectives[0] = perspective1;
    perspectives[1] = perspective2;

    registration->setPerspectives(perspectives);

    QVector<QImage> images(2);
    images[0] = QImage(image1FileName).scaledToHeight(512).copy(crops.at(0));
    images[1] = QImage(image2FileName).scaledToHeight(512).copy(crops.at(1));

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
    registration->optimizePoseShape(5);
    registration->optimizePoseShape();

    registration->exportSTL(polygonalModelName);

    return EXIT_SUCCESS;
}
