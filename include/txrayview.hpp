/**
 * @file        libmultifragmentregister.hpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The header file containing the TXRayView template implementation.
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

#include "txrayview.h"
#include <assert.h>

/**
 * @brief Constructor of the radiograph class
 * @param[in] renderer Pointer to the parent shared renderer
 */
template <class MetricType>
TXRayView<MetricType>::TXRayView(SSIMRenderer::OffscreenRenderer *renderer):
    myRenderer(new SSIMRenderer::OffscreenRenderer(512, 512, renderer)),
    myImageMetric(new MetricType(myRenderer)),
    myObserver(0)
{
}

/**
 * @brief Gets the virtual radiograph renderer
 * @return Pointer to the renderer
 */
template <class MetricType>
SSIMRenderer::OffscreenRenderer * TXRayView<MetricType>::getRenderer()
{
    return myRenderer;
}

/**
 * @brief Gets the image similarity metric
 * @return Pointer to the image metric
 */
template <class MetricType>
MetricType * TXRayView<MetricType>::getMetric()
{
    return myImageMetric;
}

/**
 * @brief Sets the radiograph observer
 * @param[in] observer Pointer to the observer object
 */
template <class MetricType>
void TXRayView<MetricType>::
setObserver(TObserver * observer)
{
    myObserver = observer;
    myImageMetric->setObserver(observer);
}

/**
 * @brief Sets the radiograph angle
 * @param[in] angle Radiograph angle
 */
template <class MetricType>
void TXRayView<MetricType>::
setAngle(double angle)
{
    myAngle = angle;
}

/**
 * @brief Gets the radiograph angle
 * @return Radiograph angle
 */
template <class MetricType>
double TXRayView<MetricType>::
getAngle() const
{
    return myAngle;
}

/**
 * @brief Updates the mask of the visible vertices
 * @param[in] v Pointer to the buffer containg recomputed vertices
 * @param[in] vn Number of vertices
 */
template <class MetricType>
void TXRayView<MetricType>::
updateMask(float * v, int vn)
{
    myMask = myRenderer->getVerticesMask(v, vn, myOpenGLCrop, myAngle);
}

/**
 * @brief Gets the vertices mask
 * @return Vertices mask
 */
template <class MetricType>
QVector<bool> TXRayView<MetricType>::
getMask()
{
    return myMask;
}

/**
 * @brief Radiograph class destructor
 */
template <class MetricType>
TXRayView<MetricType>::~TXRayView()
{
}

/**
 * @brief Changes the size of the visible vertices mask
 * @param[in] n Size of the mask
 */
template <class MetricType>
void TXRayView<MetricType>::
resizeVertexMasks(int n)
{
    myPlusMasks.resize(n);
    myMinusMasks.resize(n);
}

/**
 * @brief Sets the crop in OpenGL coordinates
 * @param[in] OpenGLCrop Float rectangle crop
 */
template <class MetricType>
void TXRayView<MetricType>::
setOpenGLCrop(const QRectF & OpenGLCrop)
{
    myOpenGLCrop = OpenGLCrop;
}

/**
 * @brief Gets the crop in OpenGL coordinates
 * @return Float rectangle crop
 */
template <class MetricType>
QRectF TXRayView<MetricType>::
getOpenGLCrop() const
{
    return myOpenGLCrop;
}
