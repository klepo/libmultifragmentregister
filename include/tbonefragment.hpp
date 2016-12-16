/**
 * @file        libmultifragmentregister.hpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TBoneFragment template implementation.
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

#include <assert.h>
#include "tbonefragment.h"

/**
 * @brief Constructor of the bone fragment class
 * @param[in] ViewCount Number of radiographs capturing the fragment
 */
template <class MetricType>
TBoneFragment<MetricType>::TBoneFragment(int ViewCount):
    myValuesCount(0),
    myObserver(0)
{
    assert(ViewCount > 0);
    myXRayViews.resize(ViewCount);    

    ///qDebug() << "Creating context" << 0;
    myXRayViews[0] = new TXRayView<MetricType>();    
    for(int i = 1; i < ViewCount; i++)        
    {
        ///qDebug() << "Creating context" << i;
        myXRayViews[i] = new TXRayView<MetricType>(myXRayViews[0]->getRenderer());
    }

    myRenderers.resize(ViewCount);
    myMetrics.resize(ViewCount);

    for(int i = 0; i < ViewCount; i++)
    {
        myRenderers[i] = myXRayViews[i]->getRenderer();
        myMetrics[i]   = myXRayViews[i]->getMetric();
    }
}

/**
 * @brief Gets the bone fragment translation
 * @return Translation of the fragment
 */
template <class MetricType>
QVector3D TBoneFragment<MetricType>::
getTranslation() const
{
    return myRenderers[0]->getTranslation();
}

/**
 * @brief Gets the bone fragment rotation
 * @return Rotation of the fragment
 */
template <class MetricType>
QVector3D TBoneFragment<MetricType>::
getRotation() const
{
    return myRenderers[0]->getRotation();
}

/**
 * @brief Initializes the overall dimensionality of the image similarity metrics
 */
template <class MetricType>
void TBoneFragment<MetricType>::
initValuesCount()
{
    myValuesCount = 0;
    foreach (MetricType * metric, myMetrics)
        myValuesCount += metric->valuesCount();
}

/**
 * @brief Gets shape parameters standardized by the standard deviation
 * @return Vector of standardized shape parameters
 */
template <class MetricType>
QVector<float> TBoneFragment<MetricType>::
getStandardizedShapeParams()
{
    SSIMRenderer::StatisticalData * data = myRenderers[0]->getStatisticalData();
    const int n = data->getNumberOfParameters();
    QVector<float> result(n);
    for(int i = 0; i < n; i++)
        result[i] = data->getPcsMatrix()[i] / data->getStdMatrix()[i];

    return result;
}

/**
 * @brief Gets non-standardized shape parameters
 * @return Vector of shape parameters
 */
template <class MetricType>
QVector<float> TBoneFragment<MetricType>::
getShapeParams()
{
    SSIMRenderer::StatisticalData * data = myRenderers[0]->getStatisticalData();
    const int n = data->getNumberOfParameters();
    std::vector<float> v;
    v.assign(data->getPcsMatrix(), data->getPcsMatrix() + n);
    return QVector<float>::fromStdVector(v);
}

/**
 * @brief Computes Jacobian matrix of the pose parameters
 * @param[in] setPose Function to adjust the scene using given pose parameters
 * @param[in] getPose Function to obtain current pose parameters from the scene
 * @return Jacobian matrix approximated by central differences method
 */
template <class MetricType>
dlib::matrix<float> TBoneFragment<MetricType>::
poseGradient(setPoseType setPose,
             getPoseType getPose)
{
    //qDebug() << "pose gradient";
    assert(myValuesCount > 0);
    const QVector3D pose = (this->*getPose)();

    const int ParamCount = 3;
    const float eps = 1;
    for(int i = 0; i < myXRayViews.size(); i++)
        myXRayViews[i]->resizeVertexMasks(ParamCount);

    // Toto alokovat uz v momente, kdy je znama velikost myValuesCount
    dlib::matrix<float> result(myValuesCount, ParamCount);

    float * vertices = NULL;
    const int vn = myRenderers.at(0)->getMesh()->getNumberOfVertices();

    for(int col = 0; col < ParamCount; col++)
    {
        //qDebug() << "pose param" << col;
        QVector3D  plus(pose);
        QVector3D minus(pose);

         plus[col] += eps;
        minus[col] -= eps;

        (this->*setPose)(plus);
        int row = 0;
        for(int i = 0; i < myRenderers.size(); i++)
        {
            if(myObserver != NULL)
                myObserver->beforeRendering();

            myRenderers[i]->renderNow();
            if(vertices == NULL)
            {
                myXRayViews[0]->getRenderer()->getRecomputedVertices(vertices);
            }

            if(myObserver != NULL)
                myObserver->afterRendering();

            myXRayViews[i]->myPlusMasks[col] = myRenderers[i]->getVerticesMask(vertices, vn, myXRayViews[i]->getOpenGLCrop(), myXRayViews[i]->getAngle());

            // toto zkusit predavat jako floatove pole, ne jako vektor
            float * v = myXRayViews[i]->getMetric()->getValues();
            for(int j = 0; j < myXRayViews[i]->getMetric()->valuesCount(); j++)
                result(row++, col) = v[j];
        }

        (this->*setPose)(minus);
        row = 0;
        for(int i = 0; i < myRenderers.size(); i++)
        {
            if(myObserver != NULL)
                myObserver->beforeRendering();

            myRenderers[i]->renderNow();            

            if(myObserver != NULL)
                myObserver->afterRendering();

            myXRayViews[i]->myMinusMasks[col] = myRenderers[i]->getVerticesMask(vertices, vn, myXRayViews[i]->getOpenGLCrop(), myXRayViews[i]->getAngle());

            // toto zkusit predavat jako floatove pole, ne jako vektor
            float * v = myXRayViews[i]->getMetric()->getValues();
            for(int j = 0; j < myXRayViews[i]->getMetric()->valuesCount(); j++)
            {
                result(row, col) = (result(row, col) - v[j]) / (2 * eps);
                row += 1;
            }
        }
    }

    delete vertices;

    (this->*setPose)(pose);
    return result;
}


/**
 * @brief Computes Jacobian matrix of the shape parameters
 * @param[in] ParamCount Number of currently optimized shape parameters
 * @return Jacobian matrix approximated by central differences method
 */
template <class MetricType>
dlib::matrix<float> TBoneFragment<MetricType>::
shapeGradient(unsigned int ParamCount)
{
    //qDebug() << "shape gradient";

    assert(myValuesCount > 0);
    QVector<float> shape = getStandardizedShapeParams();
    if(ParamCount == 0)
        ParamCount = shape.size();

    ////////////// Tady menit //////////////
    const float eps = 1.0;

    for(int i = 0; i < myXRayViews.size(); i++)
        myXRayViews[i]->resizeVertexMasks(ParamCount);

    // Toto alokovat uz v momente, kdy je znama velikost myValuesCount i ParamCount
    dlib::matrix<float> result(myValuesCount, ParamCount);
    const int vn = myRenderers.at(0)->getMesh()->getNumberOfVertices();

    for(unsigned int col = 0; col < ParamCount; col++)
    {
        //qDebug() << "shape param" << col;

        QVector<float>  plus(shape);
        QVector<float> minus(shape);

         plus[col] += eps;
        minus[col] -= eps;

        setStandardizedShapeParams(plus);
        int row = 0;
        for(int i = 0; i < myRenderers.size(); i++)
        {
            if(myObserver != NULL)
                myObserver->beforeRendering();

            myRenderers[i]->renderNow();

            if(myObserver != NULL)
                myObserver->afterRendering();

            /*** vertex metrika ***/
            float * vertices = NULL;
            myRenderers[0]->getRecomputedVertices(vertices);
            myXRayViews[i]->myPlusMasks[col] = myRenderers[i]->getVerticesMask(vertices, vn, myXRayViews[i]->getOpenGLCrop(), myXRayViews[i]->getAngle());
            delete vertices;


            float * v = myXRayViews[i]->getMetric()->getValues();
            for(int j = 0; j < myXRayViews[i]->getMetric()->valuesCount(); j++)
                result(row++, col) = v[j];
        }

        setStandardizedShapeParams(minus);
        row = 0;
        for(int i = 0; i < myRenderers.size(); i++)
        {
            if(myObserver != NULL)
                myObserver->beforeRendering();

            myRenderers[i]->renderNow();

            if(myObserver != NULL)
                myObserver->afterRendering();

            /*** vertex metrika ***/
            float * vertices = NULL;
            myRenderers[0]->getRecomputedVertices(vertices);
            myXRayViews[i]->myMinusMasks[col] = myRenderers[i]->getVerticesMask(vertices, vn, myXRayViews[i]->getOpenGLCrop(), myXRayViews[i]->getAngle());
            delete vertices;


            // toto zkusit predavat jako floatove pole, ne jako vektor
            float * v = myXRayViews[i]->getMetric()->getValues();
            for(int j = 0; j < myXRayViews[i]->getMetric()->valuesCount(); j++)
            {
                result(row, col) = (result(row, col) - v[j]) / (2 * eps);
                row += 1;
            }
        }
    }

    setStandardizedShapeParams(shape);
    return result;
}

/**
 * @brief Gets the Jacobian matrix for rotation parameters
 * @return Central differences approximation of Jacobian matrix
 */
template <class MetricType>
dlib::matrix<float> TBoneFragment<MetricType>::
rotationGradient()
{
    return poseGradient(&TBoneFragment<MetricType>::setRotation,
                        &TBoneFragment<MetricType>::getRotation);
}

/**
 * @brief Gets the Jacobian matrix for translation parameters
 * @return Central differences approximation of Jacobian matrix
 */
template <class MetricType>
dlib::matrix<float> TBoneFragment<MetricType>::
translationGradient()
{
    return poseGradient(&TBoneFragment<MetricType>::setTranslation,
                        &TBoneFragment<MetricType>::getTranslation);
}

/**
 * @brief Updates masks of rendered vertices for each radiograph
 */
template <class MetricType>
void TBoneFragment<MetricType>::
updateMasks()
{
    long int vn = myRenderers[0]->getMesh()->getNumberOfVertices();
    float * v = NULL;
    myRenderers[0]->getRecomputedVertices(v);
    assert(v != NULL);

    foreach (TXRayView<MetricType> * XRayView, myXRayViews)
        XRayView->updateMask(v, vn);

    delete v;
}

/**
 * @brief Sets the translation of the bone fragment
 * @param[in] translation The translation vector
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setTranslation(const QVector3D & translation)
{
    myRenderers[0]->setTranslation(translation);
}

/**
 * @brief Sets the rotation of the bone fragment
 * @param[in] rotation The rotation vector
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setRotation(const QVector3D & rotation)
{
    myRenderers[0]->setRotation(rotation);
}

/**
 * @brief Sets the shape model
 * @param[in] shapeFile Shape model
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setShapeModel(SSIMRenderer::MatStatisticalDataFile * shapeFile)
{
    myRenderers[0]->setVertices(shapeFile);
}

/**
 * @brief Sets the density model
 * @param[in] densityFile Density model
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setDensityModel(SSIMRenderer::MatStatisticalDataFile * densityFile)
{
    myRenderers[0]->setCoefficients(densityFile);
}

/**
 * @brief Sets the values of shape parameters of the model
 * @param[in] shapeParams Vector of shape parameters
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setShapeParams(const QVector<float> & shapeParams)
{
    SSIMRenderer::StatisticalData * data = myRenderers[0]->getStatisticalData();
    for(int i = 0; i < shapeParams.size(); i++)
        data->updatePcsMatrix(i, shapeParams.at(i));

    myRenderers[0]->updateVertices(data);
}

/**
 * @brief Sets the standardized values of shape parameters
 * @param[in] shapeParams Vector of shape parameters standardized by the standard deviation
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setStandardizedShapeParams(const QVector<float> & shapeParams)
{
    SSIMRenderer::StatisticalData * data = myRenderers[0]->getStatisticalData();
    for(int i = 0; i < shapeParams.size(); i++)
    {
        const float score = shapeParams.at(i) * data->getStdMatrix()[i];
        data->updatePcsMatrix(i, score);
    }
    myRenderers[0]->updateVertices(data);
}

/**
 * @brief Sets the values of density parameters of the model
 * @param[in] densityParams Vector of density parameters
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setDensityParams(const QVector<float> & densityParams)
{
    Q_UNUSED(densityParams);
    qDebug() << "ERROR: TBoneFragment<MetricType>::setDensityParams NOT implemented.";
    exit(EXIT_FAILURE);
}

/**
 * @brief Sets the standardized values of density parameters
 * @param[in] densityParams Vector of density parameters standardized by the standard deviation
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setStandardizedDensityParams(const QVector<float> & densityParams)
{
    Q_UNUSED(densityParams);
    qDebug() << "ERROR: TBoneFragment<MetricType>::setDensityParams NOT implemented.";
    exit(EXIT_FAILURE);
}

/**
 * @brief Sets the observer object
 * @param[in] observer Pointer to the observer object
 */
template <class MetricType>
void TBoneFragment<MetricType>::
setObserver(TObserver * observer)
{
    myObserver = observer;
    foreach (TXRayView<MetricType> * XRayView, myXRayViews)
        XRayView->setObserver(myObserver);
}

/**
 * @brief Bone fragment destructor
 */
template <class MetricType>
TBoneFragment<MetricType>::~TBoneFragment()
{
}
