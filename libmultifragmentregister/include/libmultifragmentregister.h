/**
 * @file        libmultifragmentregister.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the LibMultiFragmentRegister class template declaration.
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

#ifndef LIBMULTIFRAGMENTREGISTER_H
#define LIBMULTIFRAGMENTREGISTER_H

#include "libmultifragmentregisterabstract.h"
#include "libmultifragmentregister_global.h"
#include "tbonefragment.h"
#include "VertexMetric/tvertexmetric.h"
#include "Observer/tobserver.h"

#include <QObject>
#include <QVector>

#include <dlib/optimization.h>

/**
 * @brief The LibMultiFragmentRegister template is the core of the library.
 */
template <class MetricType>
class LibMultiFragmentRegister : public LibMultiFragmentRegisterAbstract
{
public:    
    typedef LibMultiFragmentRegister<MetricType>* Pointer;
    void optimizePose();
    void optimizePoseShape(unsigned int count = 0);
    void optimizePoseVertex();
    void optimizePoseShapeVertex(unsigned int count = 0);

    QVector3D getBoundingBoxSize();
    QPair<QVector3D, QVector3D> getBoundingBox();

    LibMultiFragmentRegister(int FragmentCount, int ViewCount, TVertexMetric * vertexMetric);
    static Pointer New(int FragmentCount, int ViewCount, TVertexMetric * vertexMetric);

    void setMeshModel(SSIMRenderer::Mesh * mesh);
    void enableDensity(  bool enable);
    void enableMirroring(bool enable);

    void setObserver(TObserver * observer);

    void setShapeModel(  SSIMRenderer::MatStatisticalDataFile * shapeFile);
    void setDensityModel(SSIMRenderer::MatStatisticalDataFile * densityFile);    

    QVector<QVector3D> getRotations();
    QVector<QVector3D> getTranslations();
    QVector<QMatrix4x4> getTransformations();

    QVector3D getMeanTranslation();
    QVector3D getMeanRotation();

    QVector<QVector3D> transformPoints(const QVector<QVector3D> & points);
    QVector<QVector3D> transformPoints();
    double pointToPointDistance(const QVector<QVector3D> & points);
    double pointToPointDistance();
    void setPoints(const QVector<QVector3D> & points);

    QVector<float>  getValues();
    QVector<float>  getTargetValues();

    QVector<float> getVertexValues();
    QVector<float> getTargetVertexValues();
    QVector<float> getTargetValuesVertex();

    QVector<float> getStandardizedShapeParams();
    QVector<float> getShapeParams();

    int getWrongVerticesCount();

    void setStandardizedShapeParams(  const QVector<float> & shapeParams);
    void setStandardizedDensityParams(const QVector<float> & densityParams);
    void setShapeParams(  const QVector<float> & shapeParams);
    void setDensityParams(const QVector<float> & densityParams);

    void setPerspectives(const QVector<SSIMRenderer::Pyramid> & perspectives);
    void setRotations(const QVector<QVector3D> & rotations);
    void setTranslations(const QVector<QVector3D> & translations);
    void setRotations(const QVector3D & rotations);
    void setTranslations(const QVector3D & translations);
    void setImages(const QVector<QImage> & images);
    void setMasks(const QVector<QImage> & masks);
    void setCrops(const QVector<QRect> & crops);
    void setOpenGLCrops(const QVector<QRectF> & crops);
    void setAngles(const QVector<double> & angles);
    void setSizes(const QVector<QSize> & sizes);
    void setHistogramBinsCount(const QVector<int> & bins);
    void setPoseEps(double eps);

    void paramsChanged();

    QVector<QImage> getImages();
    TObserver * getObserver();

    void exportSingleStl(QString fileName);
    void exportEachStl(QString fileName, bool transform, bool crop, bool lengthFix, QVector<QVector3D> normals,
                       QVector<double> ds,
                       QVector<double> signs);
    void exportSTL(QString fileName);
    void exportSTL(QString fileName, const QVector3D & rotation, const QVector3D & translation);

    void vertices(float *& vertices, int &nv, const QVector3D & rotation, const QVector3D & translation);
    void triangles(int *& triangles, int &nt);

    void renderNow();

private:
    void optimize(
            sample_points (LibMultiFragmentRegister<MetricType>::*samples)(),
            double (LibMultiFragmentRegister<MetricType>::*residuals_)(
                const std::pair<input_vector, double>& data,
                const parameter_vector& params
                ),
            dlib::matrix<float> (LibMultiFragmentRegister<MetricType>::*gradient_)(),
            parameter_vector (LibMultiFragmentRegister<MetricType>::*toVector_)(),
            void (LibMultiFragmentRegister<MetricType>::*fromVector_)(const parameter_vector & params),
            void (LibMultiFragmentRegister<MetricType>::*paramsChanged_)()
    );

    parameter_vector posesToVector();
    parameter_vector posesShapeToVector();

    void vectorToPoses(const parameter_vector & params);
    void vectorToPosesShape(const parameter_vector & params);

    sample_points data_points();
    sample_points data_points_vertex();

    double getResidual(const std::pair<input_vector, double>& data,
                       const parameter_vector& params);
    double getResidualVertex(const std::pair<input_vector, double>& data,
                             const parameter_vector& params);

    parameter_vector getGradient(const std::pair<input_vector, double>& data,
                                 const parameter_vector& params);

    dlib::matrix<float> poseGradient();
    dlib::matrix<float> poseShapeGradient();
    dlib::matrix<float> poseGradientVertex();
    dlib::matrix<float> poseShapeGradientVertex();
    dlib::matrix<float> densityGradient();    

    void posesChanged();
    void posesShapeChanged();


    void updateMasks();


    void setShapeParamsCount(unsigned int count = 0);

    QVector<double> myAngles;
    int myValuesCount;

    dlib::matrix<float> (LibMultiFragmentRegister<MetricType>::*gradient)();
    double (LibMultiFragmentRegister<MetricType>::*residuals)(
            const std::pair<input_vector, double>& data,
            const parameter_vector& params
            );
    parameter_vector (LibMultiFragmentRegister<MetricType>::*toVector)();
    void (LibMultiFragmentRegister<MetricType>::*fromVector)(const parameter_vector & params);
    void (LibMultiFragmentRegister<MetricType>::*myParamsChanged)();

    QVector<TBoneFragment<MetricType> *> myBoneFragments;
    QVector<SSIMRenderer::OffscreenRenderer *> myRenderers;
    QVector<TXRayView<MetricType> *>  myXRayViews;
    QVector<MetricType *>  myMetrics;

    QVector<float> myValues;
    QVector<float> myTargetValues;
    QVector<float> myTargetValuesVertex;

    dlib::matrix<float> myGradients;
    unsigned int myShapeParamsCount;

    TObserver * myObserver;
    TVertexMetric * myVertexMetric;
    QVector<QRectF> myOpenGLCrops;
    int myViewCount;
};

#include "libmultifragmentregister.hpp"

#endif // LIBMULTIFRAGMENTREGISTER_H
