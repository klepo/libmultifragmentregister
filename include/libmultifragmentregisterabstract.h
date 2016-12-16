/**
 * @file        libmultifragmentregisterabstract.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the LibMultiFragmentRegisterAbstract class declaration.
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

#ifndef LIBMULTIFRAGMENTREGISTERABSTRACT_H
#define LIBMULTIFRAGMENTREGISTERABSTRACT_H

#include "libmultifragmentregisterabstract.h"
#include "libmultifragmentregister_global.h"
#include "tbonefragment.h"
#include "VertexMetric/tvertexmetric.h"
#include "Observer/tobserver.h"

#include <QObject>
#include <QVector>

#include <dlib/optimization.h>

typedef dlib::matrix<double,0,1> parameter_vector;
typedef dlib::matrix<double,0,1> input_vector;

typedef std::vector<std::pair<input_vector, double> > sample_points;
typedef std::pair<input_vector, double> sample_point;

/**
 * @brief Abstract base class of the registration template core
 */
class LibMultiFragmentRegisterAbstract
{
public:
    LibMultiFragmentRegisterAbstract() {}
    ~LibMultiFragmentRegisterAbstract() {}

    virtual void optimizePose() = 0;
    virtual void optimizePoseShape(unsigned int count = 0) = 0;
    virtual void optimizePoseVertex() = 0;
    virtual void optimizePoseShapeVertex(unsigned int count = 0) = 0;

    virtual void renderNow() = 0;    

    LibMultiFragmentRegisterAbstract(int FragmentCount, int ViewCount, TVertexMetric * vertexMetric) {}

    virtual void setMeshModel(SSIMRenderer::Mesh * mesh) = 0;
    virtual void enableDensity(  bool enable) = 0;
    virtual void enableMirroring(bool enable) = 0;

    virtual void setObserver(TObserver * observer) = 0;

    virtual void setShapeModel(  SSIMRenderer::MatStatisticalDataFile * shapeFile) = 0;
    virtual void setDensityModel(SSIMRenderer::MatStatisticalDataFile * densityFile) = 0;

    virtual QVector<QVector3D> getRotations() = 0;
    virtual QVector<QVector3D> getTranslations() = 0;

    virtual QVector3D getMeanTranslation() = 0;
    virtual QVector3D getMeanRotation() = 0;
    virtual QVector3D getBoundingBoxSize() = 0;
    virtual QPair<QVector3D, QVector3D> getBoundingBox() = 0;

    virtual QVector<float>  getValues() = 0;
    virtual QVector<float>  getTargetValues() = 0;

    virtual QVector<float> getStandardizedShapeParams() = 0;
    virtual QVector<float> getShapeParams() = 0;

    virtual int getWrongVerticesCount() = 0;

    virtual void setStandardizedShapeParams(  const QVector<float> & shapeParams) = 0;
    virtual void setStandardizedDensityParams(const QVector<float> & densityParams) = 0;
    virtual void setShapeParams(  const QVector<float> & shapeParams) = 0;
    virtual void setDensityParams(const QVector<float> & densityParams) = 0;

    virtual void setPerspectives(const QVector<SSIMRenderer::Pyramid> & perspectives) = 0;
    virtual void setRotations(const QVector<QVector3D> & rotations) = 0;
    virtual void setTranslations(const QVector<QVector3D> & translations) = 0;
    virtual void setRotations(const QVector3D & rotations) = 0;
    virtual void setTranslations(const QVector3D & translations) = 0;
    virtual void setImages(const QVector<QImage> & images) = 0;
    virtual void setCrops(const QVector<QRect> & crops) = 0;
    virtual void setOpenGLCrops(const QVector<QRectF> & crops) = 0;
    virtual void setAngles(const QVector<double> & angles) = 0;
    virtual void setSizes(const QVector<QSize> & sizes) = 0;
    virtual void setHistogramBinsCount(const QVector<int> & bins) = 0;

    virtual void paramsChanged() = 0;
    static QVector<float> loadDataFromCSVFile(QString fileName);
    static void printMatrix(const dlib::matrix<float> & g);
    static QImage maskImage(const QImage & image);
    static QRect cropImage(const QImage & image);

    virtual QVector<QImage> getImages() = 0;
    virtual TObserver * getObserver() = 0;

    virtual void exportSingleStl(QString fileName) = 0;
    virtual void exportEachStl(QString fileName, bool transform, bool crop, bool lengthFix,
                               QVector<QVector3D> normals,
                               QVector<double> ds,
                               QVector<double> signs) = 0;
    virtual void exportSTL(QString fileName) = 0;
    virtual void exportSTL(QString fileName, const QVector3D & rotation, const QVector3D & translation) = 0;

    virtual void vertices(float *& vertices, int &nv, const QVector3D & rotation, const QVector3D & translation) = 0;
    virtual void triangles(int *& triangles, int &nt) = 0;
};

#endif // LIBMULTIFRAGMENTREGISTERABSTRACT_H
