/**
 * @file        libmultifragmentregister.hpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the LibMultiFragmentRegister template implementation.
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

#include "libmultifragmentregister.h"
#include "optimisationmodel.h"
#include "Observer/tobserver.h"
#include "VertexMetric/tsimplevertexmetric.h"
#include "VertexMetric/tsquareddifferencesvertexmetric.h"

#include <QDebug>
#include <vector>
#include <assert.h>

/**
 * @brief Creates new instance of the registration class
 * @param[in] FragmentCount Number of bone fragments
 * @param[in] ViewCount Number of radiographs per one bone fragment
 * @param[in] vertexMetric Pointer to the vertex metric object, can be null pointer
 * @return Pointer to created registration object
 */
template <class MetricType>
LibMultiFragmentRegister<MetricType> *
LibMultiFragmentRegister<MetricType>::
New(int FragmentCount, int ViewCount, TVertexMetric * vertexMetric)
{
    return new LibMultiFragmentRegister<MetricType>(FragmentCount, ViewCount, vertexMetric);
}

/**
 * @brief Registration class constructor
 * @param[in] FragmentCount Number of bone fragments
 * @param[in] ViewCount Number of radiographs per one bone fragment
 * @param[in] vertexMetric Pointer to the vertex metric object, can not be null pointer
 */
template <class MetricType>
LibMultiFragmentRegister<MetricType>::
LibMultiFragmentRegister(int FragmentCount, int ViewCount, TVertexMetric * vertexMetric):
    LibMultiFragmentRegisterAbstract(FragmentCount, ViewCount, vertexMetric),
    myShapeParamsCount(0),
    myValuesCount(0),
    myObserver(0),
    myVertexMetric(vertexMetric),
    myViewCount(ViewCount)
{
    assert(vertexMetric != NULL);

    myVertexMetric->setViewsNumber(ViewCount);
    myBoneFragments.clear();
    myBoneFragments.resize(FragmentCount);
    myRenderers.resize(0);
    for(int i = 0; i < FragmentCount; i++)
    {
        myBoneFragments[i] = new TBoneFragment<MetricType>(ViewCount);
        myRenderers += myBoneFragments[i]->getRenderers();
        myMetrics   += myBoneFragments[i]->getMetrics();
        myXRayViews += myBoneFragments[i]->getXRayViews();
    }
    //enableDensity(true);
}

/**
 * @brief Sets a number of shape model principal components
 * @param[in] count Number of principal components
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setShapeParamsCount(unsigned int count)
{
    if(count == 0)
        myShapeParamsCount = getShapeParams().size();
    else
        myShapeParamsCount = count;
}

/**
 * @brief Performs rigid registration including the vertex metric optimization
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
optimizePoseVertex()
{
    optimize(&LibMultiFragmentRegister<MetricType>::data_points_vertex,
             &LibMultiFragmentRegister<MetricType>::getResidualVertex,
             &LibMultiFragmentRegister<MetricType>::poseGradientVertex,
             &LibMultiFragmentRegister<MetricType>::posesToVector,
             &LibMultiFragmentRegister<MetricType>::vectorToPoses,
             &LibMultiFragmentRegister<MetricType>::posesChanged);
}

/**
 * @brief Performs non-rigid registration including the vertex metric optimization
 * @param[in] count Number of optimized shape model principal components
 *
 * If the count of principal components is not provided to the function,
 * the registration involves all shape parameters.
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
optimizePoseShapeVertex(unsigned int count = 0)
{
    setShapeParamsCount(count);
    optimize(&LibMultiFragmentRegister<MetricType>::data_points_vertex,
             &LibMultiFragmentRegister<MetricType>::getResidualVertex,
             &LibMultiFragmentRegister<MetricType>::poseShapeGradientVertex,
             &LibMultiFragmentRegister<MetricType>::posesShapeToVector,
             &LibMultiFragmentRegister<MetricType>::vectorToPosesShape,
             &LibMultiFragmentRegister<MetricType>::posesShapeChanged);
}

/**
 * @brief Performs rigid registration of the bone atlas
 *
 * Vertex metric is not involved.
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
optimizePose()
{
    optimize(&LibMultiFragmentRegister<MetricType>::data_points,
             &LibMultiFragmentRegister<MetricType>::getResidual,
             &LibMultiFragmentRegister<MetricType>::poseGradient,
             &LibMultiFragmentRegister<MetricType>::posesToVector,
             &LibMultiFragmentRegister<MetricType>::vectorToPoses,
             &LibMultiFragmentRegister<MetricType>::posesChanged);
}

/**
 * @brief Performs non-rigid registration
 * @param[in] count Number of optimized shape model principal components
 *
 * If the count of principal components is not provided to the function,
 * the registration involves all shape parameters. Vertex metric is not involved.
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
optimizePoseShape(unsigned int count = 0)
{
    setShapeParamsCount(count);
    optimize(&LibMultiFragmentRegister<MetricType>::data_points,
             &LibMultiFragmentRegister<MetricType>::getResidual,
             &LibMultiFragmentRegister<MetricType>::poseShapeGradient,
             &LibMultiFragmentRegister<MetricType>::posesShapeToVector,
             &LibMultiFragmentRegister<MetricType>::vectorToPosesShape,
             &LibMultiFragmentRegister<MetricType>::posesShapeChanged);
}

/**
 * @brief Optimizes image similarity and vertex metrics using Levenberg-Marquardt algorithm
 * @param[in] samples Function returning target values of involved metrics
 * @param[in] residuals Function returning differences between actual and target metrics values
 * @param[in] gradient Function approximating Jacobian matrix
 * @param[in] fromVector Function adjusting the shape model according to the optimized values
 * @param[in] toVector Function converting the shape model parameters to the vector of optimized values
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
optimize(
    sample_points (LibMultiFragmentRegister<MetricType>::*samples)(),
    double (LibMultiFragmentRegister<MetricType>::*residuals_)(
            const std::pair<input_vector, double>& data,
            const parameter_vector& params
            ),
    dlib::matrix<float> (LibMultiFragmentRegister<MetricType>::*gradient_)(),
    parameter_vector (LibMultiFragmentRegister<MetricType>::*toVector_)(),
    void (LibMultiFragmentRegister<MetricType>::*fromVector_)(const parameter_vector & params),
    void (LibMultiFragmentRegister<MetricType>::*paramsChanged_)()
)
{
    toVector        = toVector_;
    fromVector      = fromVector_;
    gradient        = gradient_;
    residuals       = residuals_;
    myParamsChanged = paramsChanged_;

    if(myObserver != NULL)
        myObserver->beforeRegistration();

    parameter_vector v = (this->*toVector)();
    typedef LibMultiFragmentRegister<MetricType> libmfr_type;
    libmfr_solve_least_squares_lm(libmfr_objective_delta_stop_strategy<libmfr_type>(1e-7, 150, this),
                           residuals, //&LibMultiFragmentRegister<MetricType>::getResidual,
                           &LibMultiFragmentRegister<MetricType>::getGradient,
                           (this->*samples)(), //data_points(),
                           v,
                           this);

    if(myObserver != NULL)
        myObserver->afterRegistration();
}

/**
 * @brief Gets current rotation angles for each bone fragment
 * @return Vector of angles
 */
template <class MetricType>
QVector<QVector3D> LibMultiFragmentRegister<MetricType>::
getRotations()
{
    QVector<QVector3D> result(myBoneFragments.size());
    unsigned int i = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        result[i++] = boneFragment->getRotation();
    return result;
}

/**
 * @brief Gets current translation for each bone fragment
 * @return Vector of translations
 */
template <class MetricType>
QVector<QVector3D> LibMultiFragmentRegister<MetricType>::
getTranslations()
{
    QVector<QVector3D> result(myBoneFragments.size());
    unsigned int i = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        result[i++] = boneFragment->getTranslation();
    return result;
}

/**
 * @brief Calls the observer when the pose of shape model is changed
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
posesChanged()
{
    if(myObserver != NULL)
    {
        myObserver->rotationsChanged(getRotations());
        myObserver->translationsChanged(getTranslations());
    }
}

/**
 * @brief Calls the observer when the pose or shape of the shape model is changed
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
posesShapeChanged()
{
    if(myObserver != NULL)
    {
        posesChanged();
        myObserver->shapeChanged(getShapeParams());
    }
}

/**
 * @brief Calls the observer when the shape parameters are changed
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
paramsChanged()
{
    if(myObserver != NULL)
        (this->*myParamsChanged)();
}


/**
 * @brief Gets the target values of image similarity metrics
 * @return Vector of target values
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getTargetValues()
{
    unsigned int i = 0;
    float * data = new float[myValuesCount];
    foreach (MetricType * metric, myMetrics)
    {
        const int n = metric->valuesCount();
        float * values = metric->getTargetValues();
        memcpy((void*)(data + i), (void*)values, n*sizeof(float));
        i += n;
    }

    std::vector<float> vector;
    vector.assign(data, data + myValuesCount);
    QVector<float> result = QVector<float>::fromStdVector(vector);
    delete[] data;
    return result;
}

/**
 * @brief Gets the target values of vertex metric
 * @return Vector of target values
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getTargetValuesVertex()
{
    unsigned int i = 0;
    float * data = new float[myValuesCount + myVertexMetric->valuesCount()];
    foreach (MetricType * metric, myMetrics)
    {
        const int n = metric->valuesCount();
        float * values = metric->getTargetValues();
        memcpy((void*)(data + i), (void*)values, n*sizeof(float));
        i += n;
    }
    memcpy((void*)(data + i),
           (void*)myVertexMetric->getTargetValues(),
           sizeof(float) * myVertexMetric->valuesCount());

    std::vector<float> vector;
    vector.assign(data, data + myValuesCount + myVertexMetric->valuesCount());
    QVector<float> result = QVector<float>::fromStdVector(vector);
    delete[] data;
    return result;
}

/**
 * @brief Gets pointer to the current registration observer
 * @return Pointer to observer
 */
template <class MetricType>::
TObserver * LibMultiFragmentRegister<MetricType>::
getObserver()
{
    return myObserver;
}

/**
 * @brief Gets vector of bone fragments poses
 * @return Vector of poses
 */
template <class MetricType>
parameter_vector LibMultiFragmentRegister<MetricType>::
posesToVector()
{
    parameter_vector result(6 * myBoneFragments.size());
    unsigned int i = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        QVector3D r = boneFragment->getRotation();
        QVector3D t = boneFragment->getTranslation();

        result(i + 0) = r.x(); result(i + 1) = r.y(); result(i + 2) = r.z();
        result(i + 3) = t.x(); result(i + 4) = t.y(); result(i + 5) = t.z();

        i += 6;
    }
    return result;
}

/**
 * @brief Gets vector of bone fragments poses and shape parameters
 * @return Vector of poses and shape
 */
template <class MetricType>
parameter_vector LibMultiFragmentRegister<MetricType>::
posesShapeToVector()
{
    parameter_vector poses = posesToVector();
    parameter_vector result(poses.size() + myShapeParamsCount);
    QVector<float> shapeParams = getStandardizedShapeParams();

    dlib::set_subm(
            result,
            dlib::range(0, poses.size() - 1),
            dlib::range(0, 0)
         ) = poses;

    for(unsigned int i = 0; i < myShapeParamsCount; i++)
        result(poses.size() + i) = shapeParams[i];

    return result;
}

/**
 * @brief Sets the poses of the bone fragments
 * @param params Vector of poses
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
vectorToPoses(const parameter_vector & params)
{
    assert(params.size() == 6 * myBoneFragments.size());
    unsigned int i = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        QVector3D r(params(i + 0), params(i + 1), params(i + 2));
        QVector3D t(params(i + 3), params(i + 4), params(i + 5));

        boneFragment->setRotation(r);
        boneFragment->setTranslation(t);

        i += 6;
    }
}

/**
 * @brief Sets the poses and the shape of the bone fragments
 * @param params Vector of poses and shape parameters
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
vectorToPosesShape(const parameter_vector & params)
{
    parameter_vector v(6 * myBoneFragments.size());
    for(int i = 0; i < v.size(); i++)
        v(i) = params(i);
    vectorToPoses(v);

    QVector<float> shapeParams = getStandardizedShapeParams();
    const int n = params.size() - v.size();

    for(int i = 0; i < n; i++)
        shapeParams[i] = params(v.size() + i);

    setStandardizedShapeParams(shapeParams);
}

/**
 * Gets target values including the vertex metric
 * @return Metrics target values
 */
template <class MetricType>
sample_points LibMultiFragmentRegister<MetricType>::
data_points_vertex()
{
    assert(myValuesCount > 0);
    sample_points result;
    input_vector input(1);
    QVector<float> targetValues = getTargetValuesVertex();

    for(int i = 0; i < targetValues.count(); i++)
    {
        input(0) = i;
        result.push_back(std::make_pair(input, targetValues.at(i)));
    }

    return result;
}

/**
 * Gets target values of image similarity metrics
 * @return Similarity metrics target values
 */
template <class MetricType>
sample_points LibMultiFragmentRegister<MetricType>::
data_points()
{
    assert(myValuesCount > 0);
    sample_points result;
    input_vector input(1);
    QVector<float> targetValues = getTargetValues();

    for(int i = 0; i < targetValues.count(); i++)
    {
        input(0) = i;
        result.push_back(std::make_pair(input, targetValues.at(i)));
    }

    return result;
}

/**
 * Gets differences between current and target image similarirty metrics values
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose and/or shape parameters
 * @return Redidual scalar
 */
template <class MetricType>
double LibMultiFragmentRegister<MetricType>::
getResidual(const std::pair<input_vector, double>& data,
            const parameter_vector& params)
{
    int i = static_cast<int>(data.first(0));
    if(i == 0)
    {
        (this->*fromVector)(params);
        myValues = getValues();
    }
    //qDebug() << i << myValues.at(i) << data.second << myValues.at(i) - data.second;
    return myValues.at(i) - data.second;
}

/**
 * Gets differences between current and target vertex metrics values
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose and/or shape parameters
 * @return Redidual scalar
 */
template <class MetricType>
double LibMultiFragmentRegister<MetricType>::
getResidualVertex(const std::pair<input_vector, double>& data,
            const parameter_vector& params)
{
    int i = static_cast<int>(data.first(0));
    if(i == 0)
    {
        (this->*fromVector)(params);
        myValues = getValues() + getVertexValues();
    }
    return myValues.at(i) - data.second;
}

/**
 * Computes gradient of the pose and (optionally) shape parameters
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose and (optionally) shape parameters
 * @return Vector of partial derivatives
 */
template <class MetricType>
parameter_vector LibMultiFragmentRegister<MetricType>::
getGradient(const std::pair<input_vector, double>& data,
            const parameter_vector& params)
{
    int i = static_cast<int>(data.first(0));
    if(i == 0)
    {
        (this->*fromVector)(params);
        myGradients = (this->*gradient)();
    }

    parameter_vector result(myGradients.nc());
    for(int j = 0; j < myGradients.nc(); j++)
        result(j) = myGradients(i, j);

    return result;

}

/**
 * @brief Sets target radiographs for the registration
 * @param[in] images Vector of input images
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setImages(const QVector<QImage> & images)
{
    unsigned int i = 0;
    foreach (MetricType * metric, myMetrics)
        metric->setImage(images.at(i++));

    myValuesCount = 0;
    foreach (MetricType * metric, myMetrics)
        myValuesCount += metric->valuesCount();

    foreach (TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->initValuesCount();

    myTargetValues = getTargetValues();
    //qDebug() << myTargetValues.size();
    myTargetValuesVertex = getTargetValuesVertex();
}

/**
 * Computes gradient of the pose parameters
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose parameters
 * @return Vector of partial derivatives
 */
template <class MetricType>
dlib::matrix<float> LibMultiFragmentRegister<MetricType>::
poseGradient()
{
    dlib::matrix<float> result = dlib::zeros_matrix<float>(
                                            myValuesCount,
                                            6 * myBoneFragments.size()
                                       );
    unsigned int col = 0;
    unsigned int row = 0;

    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        int valuesCount = boneFragment->getValuesCount();

        dlib::set_subm(result,
              dlib::range(row, row + valuesCount - 1),
              dlib::range(col, col + 2)
        ) = boneFragment->rotationGradient();

        col += 3;

        dlib::set_subm(result,
              dlib::range(row, row + valuesCount - 1),
              dlib::range(col, col + 2)
        ) = boneFragment->translationGradient();

        col += 3;
        row += valuesCount;
    }
    return result;
}

/**
 * Computes gradient of the pose parameters
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose parameters
 * @return Vector of partial derivatives
 */
template <class MetricType>
dlib::matrix<float> LibMultiFragmentRegister<MetricType>::
poseShapeGradient()
{
    dlib::matrix<float> pose = poseGradient();
    assert(myValuesCount == pose.nr());

    dlib::matrix<float> result = dlib::zeros_matrix<float>(
                                            pose.nr(),
                                            pose.nc() + myShapeParamsCount
                                       );

    dlib::set_subm(result,
          dlib::range(0, pose.nr() - 1),
          dlib::range(0, pose.nc() - 1)
    ) = pose;

    unsigned int row = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        int valuesCount = boneFragment->getValuesCount();

        dlib::set_subm(result,
              dlib::range(row, row + valuesCount - 1),
              dlib::range(pose.nc(), pose.nc() + myShapeParamsCount - 1)
        ) = boneFragment->shapeGradient(myShapeParamsCount);

        row += valuesCount;
    }

    return result;

}

/**
 * Computes gradient of the pose parameters including vertex metric
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose parameters
 * @return Vector of partial derivatives
 */
template <class MetricType>
dlib::matrix<float> LibMultiFragmentRegister<MetricType>::
poseGradientVertex()
{
    dlib::matrix<float> result = dlib::zeros_matrix<float>(
                                            myValuesCount + myVertexMetric->valuesCount(),
                                            6 * myBoneFragments.size()
                                       );
    unsigned int col = 0;
    unsigned int row = 0;

    // tady si vytvorit a ulozit masky...
    // ziskat pole vertexu
    float * vertices = NULL;
    myRenderers[0]->getRecomputedVertices(vertices);
    const int nv = myRenderers[0]->getMesh()->getNumberOfVertices();
    assert(vertices != NULL);
    // plus maska bude jina pro kazdej pohled, ale stejna pro kazdej param, navic stejna pro plus i minus
    QVector<QVector<bool> > mask;
    mask.resize(myRenderers.size());
    unsigned int i = 0;
    foreach (SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        QRectF crop = myXRayViews.at(i)->getOpenGLCrop();
        double angle = myXRayViews.at(i)->getAngle();
        mask[i] = renderer->getVerticesMask(vertices, nv, crop, angle);
        i += 1;
    }


    delete[] vertices;

    // tzn. do plus masks i minus masks ulozit to samy

    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        int valuesCount = boneFragment->getValuesCount();

        // Vypocet gradientu rotace

        // ... a tady je zase "popnout" do XRayViews
        // Tady by se mely masky pohledu fragmentu, ktere nezavisi na danych parametrech,
        // naplnit nejakou aktualni maskou
        unsigned int k = 0;
        foreach(TXRayView<MetricType> * XRayView, myXRayViews)
        {
            XRayView->myPlusMasks.resize(3);
            XRayView->myMinusMasks.resize(3);
            for(int i = 0; i < 3; i++)
            {
                XRayView->myPlusMasks[i]  = mask[k];
                XRayView->myMinusMasks[i] = mask[k];
            }
            k += 1;
        }

        dlib::set_subm(result,
              dlib::range(row, row + valuesCount - 1),
              dlib::range(col, col + 2)
        ) = boneFragment->rotationGradient();

        const int n = myXRayViews.size();
        const int ParamCount = 3;

        // Vypocet gradientu pro vertexovou metriku
        for(int p = 0; p < ParamCount; p++)
        {
            QVector<QVector<bool> >  plusMasks(n);
            QVector<QVector<bool> > minusMasks(n);

            unsigned int i = 0;
            foreach (TXRayView<MetricType> * XRayView, myXRayViews)
            {
                 plusMasks[i] = XRayView->myPlusMasks.at(p);
                minusMasks[i] = XRayView->myMinusMasks.at(p);
                i += 1;
            }
            float * values = myVertexMetric->getValues(plusMasks);
            for(int j = 0; j < myVertexMetric->valuesCount(); j++)
                result(myValuesCount + j, col + p) = values[j] / 2;

            myVertexMetric->getValues(minusMasks);
            for(int j = 0; j < myVertexMetric->valuesCount(); j++)
                result(myValuesCount + j, col + p) -= values[j] / 2;

        }

        col += 3;

        // Tady by masky mely byt uz inicializovane z predchoziho kroku

        // Vypocet gradientu translace
        dlib::set_subm(result,
              dlib::range(row, row + valuesCount - 1),
              dlib::range(col, col + 2)
        ) = boneFragment->translationGradient();

        // Vypocet gradientu pro vertexovou metriku
        for(int p = 0; p < ParamCount; p++)
        {
            QVector<QVector<bool> >  plusMasks(n);
            QVector<QVector<bool> > minusMasks(n);

            unsigned int i = 0;
            foreach (TXRayView<MetricType> * XRayView, myXRayViews)
            {
                 plusMasks[i] = XRayView->myPlusMasks.at(p);
                minusMasks[i] = XRayView->myMinusMasks.at(p);
                i += 1;
            }
            float * values = myVertexMetric->getValues(plusMasks);
            for(int j = 0; j < myVertexMetric->valuesCount(); j++)
                result(myValuesCount + j, col + p) = values[j] / 2;

            myVertexMetric->getValues(minusMasks);
            for(int j = 0; j < myVertexMetric->valuesCount(); j++)
                result(myValuesCount + j, col + p) -= values[j] / 2;

        }

        col += 3;
        row += valuesCount;
    }
    return result;
}

/**
 * Computes gradient of the pose and shape parameters including vertex metric
 * @param[in] data Vector containing target values
 * @param[in] params Vector containing pose parameters
 * @return Vector of partial derivatives
 */
template <class MetricType>
dlib::matrix<float> LibMultiFragmentRegister<MetricType>::
poseShapeGradientVertex()
{
    dlib::matrix<float> pose = poseGradientVertex();
    assert(myValuesCount + myVertexMetric->valuesCount() == pose.nr());

    // Zase pridat radky pro vertex metric gradient
    dlib::matrix<float> result = dlib::zeros_matrix<float>(
                                            pose.nr(),
                                            pose.nc() + myShapeParamsCount
                                       );

    // Tady to zkopirovat vcetne radku pro vertex metric
    dlib::set_subm(result,
          dlib::range(0, pose.nr() - 1),
          dlib::range(0, pose.nc() - 1)
    ) = pose;

    unsigned int row = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        int valuesCount = boneFragment->getValuesCount();

        dlib::set_subm(result,
              dlib::range(row, row + valuesCount - 1),
              dlib::range(pose.nc(), pose.nc() + myShapeParamsCount - 1)
        ) = boneFragment->shapeGradient(myShapeParamsCount);

        row += valuesCount;
    }

    // Tady spocitat gradient pro shape parametry a ulozit na spodek do spravnych sloupcu
    const int n = myXRayViews.size(); // !!!! ASI
    for(unsigned int p = 0; p < myShapeParamsCount; p++)
    {
        QVector<QVector<bool> >  plusMasks(n);
        QVector<QVector<bool> > minusMasks(n);

        unsigned int i = 0;
        foreach (TXRayView<MetricType> * XRayView, myXRayViews)
        {
             plusMasks[i] = XRayView->myPlusMasks.at(p);
            minusMasks[i] = XRayView->myMinusMasks.at(p);
            i += 1;
        }

        float * values = myVertexMetric->getValues(plusMasks);
        for(int j = 0; j < myVertexMetric->valuesCount(); j++)
            result(myValuesCount + j, pose.nc() + p) = values[j] / 2;

        myVertexMetric->getValues(minusMasks);
        for(int j = 0; j < myVertexMetric->valuesCount(); j++)
            result(myValuesCount + j, pose.nc() + p) -= values[j] / 2;

    }


    return result;

}

/**
 * @brief Set sizes of rendered images
 * @param[in] Vector of sizes
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setSizes(const QVector<QSize> & size)
{
    int i = 0;
    foreach (SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        renderer->setRenderSize(size.at(i).width(), size.at(i).height());
        i++;
    }
}

/**
 * @brief Gets shape parameters standardized by the standard deviation
 * @return Standardized shape parameters
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getStandardizedShapeParams()
{
    assert(myBoneFragments.size() > 0);
    return myBoneFragments[0]->getStandardizedShapeParams();
}

/**
 * @brief Gets non-standardized shape parameters
 * @return Vector of shape parameters
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getShapeParams()
{
    assert(myBoneFragments.size() > 0);
    return myBoneFragments[0]->getShapeParams();
}

/**
 * @brief Prototype of function for computing density gradient
 */
template <class MetricType>
dlib::matrix<float> LibMultiFragmentRegister<MetricType>::
densityGradient()
{
}

/**
 * @brief Gets current values of image similarity metrics
 * @return Vector of metrics values
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getValues()
{
    renderNow();
    unsigned int i = 0;
    float * data = new float[myValuesCount];
    foreach (MetricType * metric, myMetrics)
    {
        const int n = metric->valuesCount();
        float * values = metric->getValues();
        memcpy((void*)(data + i), (void*)values, n * sizeof(float));
        i += n;
    }

    std::vector<float> vector;
    vector.assign(data, data + myValuesCount);
    QVector<float> result = QVector<float>::fromStdVector(vector);
    delete[] data;
    return result;
}

/**
 * @brief Gets current values of vertex metric
 * @return Vector of metric values
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getVertexValues()
{
    updateMasks();
    QVector<QVector<bool> > masks(myXRayViews.size());
    unsigned int i = 0;
    foreach(TXRayView<MetricType> * XRayView, myXRayViews)
        masks[i++] = XRayView->getMask();

    float * values = myVertexMetric->getValues(masks);
    std::vector<float> vector;
    vector.assign(values, values + myVertexMetric->valuesCount());
    return QVector<float>::fromStdVector(vector);
}

/**
 * @brief Gets target values for vertex metric
 * @return Vector of target values
 */
template <class MetricType>
QVector<float> LibMultiFragmentRegister<MetricType>::
getTargetVertexValues()
{
    float * values = myVertexMetric->getTargetValues();
    std::vector<float> vector;
    vector.assign(values, values + myVertexMetric->valuesCount());
    return QVector<float>::fromStdVector(vector);
}

/**
 * @brief Sets rotation angles for each bone fragment
 * @param[in] rotations Vector of rotation angles
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setRotations(const QVector<QVector3D> & rotations)
{
    assert(rotations.size() == myBoneFragments.size());
    unsigned int i = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setRotation(rotations.at(i++));
}

/**
 * @brief Sets translations for each bone fragment
 * @param[in] translations Vector of translations
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setTranslations(const QVector<QVector3D> & translations)
{
    assert(translations.size() == myBoneFragments.size());
    unsigned int i = 0;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setTranslation(translations.at(i++));
}

/**
 * @brief Sets the same rotation angles for each bone fragment
 * @param[in] rotation Rotation angles
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setRotations(const QVector3D & rotation)
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setRotation(rotation);
}

/**
 * @brief Sets the same translation for each bone fragment
 * @param[in] translation Translation values
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setTranslations(const QVector3D & translation)
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setTranslation(translation);
}

/**
 * @brief Renders images according to current shape and pose parameters
 * @return Vector of rendered images
 */
template <class MetricType>
QVector<QImage> LibMultiFragmentRegister<MetricType>::
getImages()
{
    QVector<QImage> result;
    foreach(SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        renderer->renderNow();
        result.append(renderer->getRenderedImage());
    }
    return result;
}

/**
 * @brief Renders images according to current shape and pose parameters
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
renderNow()
{
    foreach(SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        if(myObserver != NULL)
            myObserver->beforeRendering();

        renderer->renderNow();

        if(myObserver != NULL)
            myObserver->afterRendering();
    }
}

/**
 * @brief Sets the perspective pyramid for each radiograph
 * @param[in] Vector of perspective pyramids
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setPerspectives(const QVector<SSIMRenderer::Pyramid> & perspectives)
{
    assert(perspectives.size() == myRenderers.size());
    unsigned int i = 0;
    foreach(SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
        renderer->setPerspective(perspectives.at(i++));
}

/**
 * @brief Sets the polygonal or tetrahedral mesh
 * @param[in] mesh Pointer to the mesh
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setMeshModel(SSIMRenderer::Mesh * mesh)
{
    // Create array of white colors for polygonal model rendering
    int size = mesh->getNumberOfVertices() * 3;
    float * colors = new float[size]();
    for(int i = 0; i < size; i++)
        colors[i] = 1.0;

    myVertexMetric->setMesh(mesh);
    foreach(SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        renderer->setMesh(mesh, colors);
    }
}

/**
 * @brief Sets the shape model
 * @param[in] shapeFile Shape model file
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setShapeModel(SSIMRenderer::MatStatisticalDataFile * shapeFile)
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setShapeModel(shapeFile);
}

/**
 * @brief Sets the density model
 * @param[in] densityFile Density model file
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setDensityModel(SSIMRenderer::MatStatisticalDataFile * densityFile)
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setDensityModel(densityFile);
}

/**
 * @brief Sets shape parameters standardized by the standard deviation
 * @param[in] shapeParams Vector of standardized shape parameters
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setStandardizedShapeParams(const QVector<float> & shapeParams)
{
    //qDebug() << "Set Standardized Shape params";
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setStandardizedShapeParams(shapeParams);
    //qDebug() << getShapeParams();
}

/**
 * @brief Sets density parameters standardized by the standard deviation
 * @param[in] densityParams Vector of standardized density parameters
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setStandardizedDensityParams(const QVector<float> & densityParams)
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setStandardizedDensityParams(densityParams);
}

/**
 * @brief Sets non-standardized shape parameters
 * @param[in] shapeParams Vector of non-standardized shape parameters
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setShapeParams(const QVector<float> & shapeParams)
{
    //qDebug() << "Set Shape params";
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setShapeParams(shapeParams);
}

/**
 * @brief Sets non-standardized density parameters
 * @param[in] densityParams Vector of non-standardized density parameters
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setDensityParams(const QVector<float> & densityParams)
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setDensityParams(densityParams);
}

/**
 * @brief Enables mirroring of the shape model
 * @param[in] enable Boolean value
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
enableMirroring(bool enable)
{
    foreach(SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
        renderer->enableXMirroring(enable);
}

/**
 * @brief Enables density rendering of the shape model
 * @param[in] enable Boolean value
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
enableDensity(bool enable)
{
    foreach(SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        renderer->enableDensity(enable);
        renderer->enableSilhouettes(false);
        renderer->enablePolygonal(!enable);

        renderer->enablePyramid(false);
        renderer->enablePolygonalLighting(false);
        renderer->setIntensity(0.61);
        renderer->enableLogMessages(false);
    }
}

/**
 * @brief Sets the observer object for the registration
 * @param[in] observer Pointer to the observer object
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setObserver(TObserver * observer)
{
    myObserver = observer;
    foreach (TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->setObserver(myObserver);
}

/**
 * @brief Gets average translation computed from every bone fragment
 * @return Mean translation
 */
template <class MetricType>
QVector3D LibMultiFragmentRegister<MetricType>::
getMeanTranslation()
{
    unsigned int i = 0;
    QVector3D sum;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        sum += boneFragment->getTranslation();
        i += 1;
    }
    return sum / i;
}

/**
 * @brief Gets average rotation computed from every bone fragment
 * @return Mean rotation
 */
template <class MetricType>
QVector3D LibMultiFragmentRegister<MetricType>::
getMeanRotation()
{
    unsigned int i = 0;
    QVector3D sum;
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        sum += boneFragment->getRotation();
        i += 1;
    }
    return sum / i;
}

/**
 * @brief Exports stl model from the first bone fragment
 * @param[in] fileName File name without extension
 *
 * The exported model is translated on centroid point.
 * Model is not cropped.
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
exportSingleStl(QString fileName)
{
    myRenderers[0]->exportSTL(fileName + ".0.stl", false);
}

/**
 * @brief Gets a bounding box size of the shape model from for the first bone fragment
 * @return Bounding box size
 */
template <class MetricType>
QVector3D LibMultiFragmentRegister<MetricType>::
getBoundingBoxSize()
{
    QVector3D min(10000, 10000, 10000);
    QVector3D max(-10000, -10000, -10000);
    QVector<QVector3D> v = myRenderers[0]->getRecomputedVertices(false);
    for(int i = 0; i < v.size(); i++)
    {
        if(v.at(i).x() < min.x()) min.setX(v.at(i).x());
        if(v.at(i).y() < min.y()) min.setY(v.at(i).y());
        if(v.at(i).z() < min.z()) min.setZ(v.at(i).z());

        if(v.at(i).x() > max.x()) max.setX(v.at(i).x());
        if(v.at(i).y() > max.y()) max.setY(v.at(i).y());
        if(v.at(i).z() > max.z()) max.setZ(v.at(i).z());
    }
    return max-min;
}

/**
 * @brief Gets a bounding box of the shape model from for the first bone fragment
 * @return Bounding box
 */
template <class MetricType>
QPair<QVector3D, QVector3D> LibMultiFragmentRegister<MetricType>::
getBoundingBox()
{
    QVector3D min(10000, 10000, 10000);
    QVector3D max(-10000, -10000, -10000);
    QVector<QVector3D> v = myRenderers[0]->getRecomputedVertices(false);
    for(int i = 0; i < v.size(); i++)
    {
        if(v.at(i).x() < min.x()) min.setX(v.at(i).x());
        if(v.at(i).y() < min.y()) min.setY(v.at(i).y());
        if(v.at(i).z() < min.z()) min.setZ(v.at(i).z());

        if(v.at(i).x() > max.x()) max.setX(v.at(i).x());
        if(v.at(i).y() > max.y()) max.setY(v.at(i).y());
        if(v.at(i).z() > max.z()) max.setZ(v.at(i).z());
    }
    return QPair<QVector3D, QVector3D>(min, max);
}

/**
 * @brief Exports stl model for each bone fragment
 * @param[in] fileName File name without extension
 * @param[in] transform Exports models in scene coordinates if set
 * @param[in] crop Exports only visible part of models if set
 * @param[in] lengthFix Fixes length is set (experimental)
 * @param[in] normals Vector of vertex normals
 * @param[in] ds (experimental)
 * @param[in] signs (experimental)
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
exportEachStl(QString fileName, bool transform, bool crop, bool lengthFix,
              QVector<QVector3D> normals,
              QVector<double> ds,
              QVector<double> signs)
{
    QVector<QVector<bool> > msks(myBoneFragments.size());
    int i = 0;
    const int n = myRenderers[0]->getMesh()->getNumberOfVertices();
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
    {
        if(crop)
        {
            float * v = NULL;            
            boneFragment->getRenderers()[0]->getRecomputedVertices(v);
            QVector<bool> msk = boneFragment->getRenderers()[0]->getVerticesMask(v, n, myOpenGLCrops.at(i*myViewCount), myAngles.at(i*myViewCount));


            /* vypocet masky podle orezove roviny */
            int i2 = i * this->myViewCount;
            QVector<bool> msk2(n, false);
            QVector<QVector3D> vertices = boneFragment->getRenderers()[0]->getRecomputedVertices(true);
            for(int j = 0; j < msk2.size(); j++)
            {
                msk2[j] = (QVector3D::dotProduct(vertices[j], normals[i2]) + ds[i2])*signs[i2] <= 0;
            }
            if(!lengthFix)
            {
                msk2 = msk;
            }

            msks[i] = msk2;



            boneFragment->getRenderers()[0]->exportSTL(fileName + "." + QString::number(i) + ".stl", transform, msk2, 0);
            delete v;
        }
        else
        {
            boneFragment->getRenderers()[0]->exportSTL(fileName + "." + QString::number(i) + ".stl", transform);
        }
        i += 1;
    }
    if(!transform && crop)
    {
        QVector<bool> msk(n);
        for(int j = 0; j < msk.size(); j++)
            msk[j] = !msks.at(0).at(j) && !msks.at(1).at(j);
        myBoneFragments[0]->getRenderers()[0]->exportSTL(fileName + "." + QString::number(2) + ".stl", transform, msk, 0);
    }
}

/**
 * @brief Exports stl model from the first bone fragment
 * @param[in] fileName File name without extension
 *
 * The exported model is in the scene coordinates,
 * only the part of the model visible in the radiographs is exported.
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
exportSTL(QString fileName)
{
    float * v = NULL;
    const int n = myRenderers[0]->getMesh()->getNumberOfVertices();
    myRenderers[0]->getRecomputedVertices(v);
    QVector<bool> msk = myRenderers[0]->getVerticesMask(v, n);

    myRenderers[0]->exportSTL(fileName + ".stl", true, msk);
}

/**
 * @brief Exports stl model from the first bone fragment with given pose
 * @param[in] fileName File name with extension
 * @param[in] rotation Rotation of the model
 * @param[in] translation Translation of the model
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
exportSTL(QString fileName,
          const QVector3D & rotation,
          const QVector3D & translation)
{
    QVector<QVector3D> rotations    = getRotations();
    QVector<QVector3D> translations = getTranslations();

    //setRotations(rotation);
    //setTranslations(translation);

    renderNow();
    myRenderers[0]->exportSTL(fileName);

    setRotations(rotations);
    setTranslations(translations);
}

/**
 * @brief Gets recomputed vertices of the shape model
 * @param[out] vertices Pointer to buffer of vertices
 * @param[out] nv Pointer to the buffer size
 * @param[in] rotation Rotation of the shape model
 * @param[in] translation Translation of the shape model
 *
 * The buffer of vertices is allocated in this function
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
vertices(float *& vertices, int &nv, const QVector3D & rotation, const QVector3D & translation)
{
    QVector3D r = myRenderers[0]->getRotation();
    QVector3D t = myRenderers[0]->getTranslation();

    myRenderers[0]->setRotation(rotation);
    myRenderers[0]->setTranslation(translation);

    myRenderers[0]->renderNow();
    nv = myRenderers[0]->getRecomputedVertices(vertices, true);
    myRenderers[0]->setRotation(r);
    myRenderers[0]->setTranslation(t);
}

/**
 * @brief Gets polygonal net of the shape model
 * @param[out] triangles Pointer to buffer of triangles
 * @param[out] nt Pointer to the buffer size
 *
 * The buffer of vertices is allocated in this function
 *
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
triangles(int *& triangles, int &nt)
{
    QVector<QVector<unsigned int>> result = myRenderers[0]->getMesh()->getTriangles2DVector();
    nt = result.size() * 3;
    triangles = new int[nt];
    int k = 0;
    for(int i = 0; i < result.size(); i++)
    {
        //for(int j = 0; j < result[i].size(); j++)
        triangles[k++] = result.at(i).at(1);
        triangles[k++] = result.at(i).at(0);
        triangles[k++] = result.at(i).at(2);
    }
}

/**
 * @brief Updates vertex mask for each bone fragment
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
updateMasks()
{
    foreach(TBoneFragment<MetricType> * boneFragment, myBoneFragments)
        boneFragment->updateMasks();
}

/**
 * @brief Sets crop for each radiograph
 * @param[in] crops Vector of rectangle crops
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setCrops(const QVector<QRect> & crops)
{
    unsigned int i = 0;
    foreach (SSIMRenderer::OffscreenRenderer * renderer, myRenderers)
    {
        QRect c = crops.at(i++);
        renderer->setCropWindow(c);
    }
        //metric->setCrop(crops.at(i++));

    //foreach (MetricType * metric, myMetrics)
    //    metric->setCrop(crops.at(i++));
}

/**
 * @brief Sets the count of bins for each NMI metric
 * @param[in] crops Vector of bin counts
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setHistogramBinsCount(const QVector<int> & bins)
{
    int i = 0;
    foreach (MetricType * metric, myMetrics)
    {
        metric->setHistogramBinsCount(bins.at(i));
        i++;
    }
}

/**
 * @brief Sets crop in OpenGL coordinate system for each radiograph
 * @param[in] crops Vector of rectangle crops
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setOpenGLCrops(const QVector<QRectF> & crops)
{
    myOpenGLCrops = crops;
    unsigned int i = 0;
    foreach (TXRayView<MetricType> * XRayView, myXRayViews)
        XRayView->setOpenGLCrop(crops.at(i++));
}

/**
 * @brief Sets angle for each radiograph
 * @param[in] angles Vector of angles
 */
template <class MetricType>
void LibMultiFragmentRegister<MetricType>::
setAngles(const QVector<double> & angles)
{
    myAngles = angles;
    unsigned int i = 0;
    foreach (TXRayView<MetricType> * XRayView, myXRayViews)
        XRayView->setAngle(angles.at(i++));
}

/**
 * @brief Gets number of wrongly rendered vertices
 * @return Number of wrong vertices
 */
template <class MetricType>
int LibMultiFragmentRegister<MetricType>::
getWrongVerticesCount()
{
    return myVertexMetric->getWrongVerticesCount();
}
