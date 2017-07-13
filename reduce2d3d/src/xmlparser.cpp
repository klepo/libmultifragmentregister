/**
 * @file        xmlparser.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The implementation file containing the xmlparser class.
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

#include "xmlparser.h"

#include <QXmlStreamReader>
#include <QRect>
#include <QFile>
#include <QDebug>

/**
 * @brief xmlparser::xmlparser Constructor of the class
 * @param mpath Location of the input XML file
 * @param filename XML file name
 */
xmlparser::xmlparser(QString mpath, QString filename):
    path(mpath)
{
    QFile file(path + "/" + filename);
    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(std::string("Error opening XML file ") + path.toStdString() + "/" + filename.toStdString());

    rotate = false;
    lengthFix = false;
    cutPlanes = false;
    manualInit = false;
    saveMeasurement = false;
    saveImages = false;
    hausdorff = false;
    length = false;
    refLength = false;
    read(&file);
    file.close();
}

/**
 * @brief xmlparser::~xmlparser Destructor of the class
 */
xmlparser::~xmlparser()
{

}

/**
 * @brief xmlparser::shapeFileName Returns the shape model file name
 * @return file name
 */
QString xmlparser::shapeFileName() const
{
    return shapeModel;
}

/**
 * @brief xmlparser::densityFileName Returns the density model file name
 * @return file name
 */
QString xmlparser::densityFileName() const
{
    return densityModel;
}

/**
 * @brief xmlparser::meshFileName Returns the mesh model file name
 * @return file name
 */
QString xmlparser::meshFileName() const
{
    return tetraModel;
}

/**
 * @brief xmlparser::stlFileName Returns the names of the files where
 * the output model should be stored
 * @return file name
 */
QString xmlparser::stlFileName() const
{
    return stl;
}

/**
 * @brief xmlparser::posesFileName Returns the name of the XML file
 * where the final pose for each fragment should be stored
 * @return
 */
QString xmlparser::posesFileName() const
{
    return poseFile;
}

/**
 * @brief xmlparser::read Parses the input XML file
 * @param device Opened handle to the XML file
 * @return True upon success
 */
bool xmlparser::read(QIODevice *device)
{
    QXmlStreamReader xml(device);



    xml.readNextStartElement();
    if (xml.name() != "reduce2d3d")
        throw std::runtime_error(std::string("Error reading XML file: the first element is not \"reduce2d3d\"."));



    int fragmentId = -1;
    int cropId = -1;
    int vertexCropId = -1;
    int lineId = -1;
    int overflowId = -1;
    int maskId = -1;

    while (!xml.atEnd()) {
        //qDebug() << "ctu xml" << xml.name();
        if (xml.readNext() != QXmlStreamReader::StartElement)
            continue;

        if (xml.name() == "setup") {
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "fragments") {
                     fragments = attr.value().toInt();
                } else if (attr.name().toString() == "views") {
                     views = attr.value().toInt();
                } else if (attr.name().toString() == "verbose") {
                     verbose = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "rotate") {
                     rotate = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "method") {
                     method = attr.value().toString();
                } else if (attr.name().toString() == "algorithm") {
                     algorithm = attr.value().toString();
                } else if (attr.name().toString() == "vertexMetric") {
                     vertexMetric = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "manualInit") {
                     manualInit = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "length") {
                     length = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "refLength") {
                     refLength = attr.value().toInt() == 1;
                }
            }
            rotation.resize(fragments);
            translation.resize(fragments);
            const int n = fragments * views;
            crop.resize(n);
            vertexCrop.resize(n);
            perspective.resize(n);
            image.resize(n);
            point1.resize(n);
            point2.resize(n);
            overflow.resize(n);
            mask.resize(4);
            for(int i = 0; i < n; i++)
                overflow[i] = QVector<bool>(4, false);

        } else if (xml.name() == "model") {
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "shape") {
                     shapeModel = attr.value().toString();
                } else if (attr.name().toString() == "density") {
                     densityModel = attr.value().toString();
                } else if (attr.name().toString() == "tetra") {
                     tetraModel = attr.value().toString();
                } else if (attr.name().toString() == "mirror") {
                     mirror = attr.value().toInt() == 1;
                }
            }
        } else if (xml.name() == "output") {
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "surface") {
                    stl = attr.value().toString();
                } else if (attr.name().toString() == "file") {
                    poseFile = attr.value().toString();
                } else if (attr.name().toString() == "transform") {
                    transform = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "crop") {
                    outCrop = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "cutPlanes") {
                    cutPlanes = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "lengthFix") {
                    lengthFix = attr.value().toInt() == 1;
                }
            }
        } else if (xml.name() == "fragment") {
            fragmentId += 1;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if(attr.name().toString() == "calibration") {
                    //SSIMRenderer::XMLCalibsFile file(path + "/" + attr.value().toString());
                    SSIMRenderer::XMLCalibsFile file(attr.value().toString());
                    for(int i = 0; i < views; i++) {
                        const int offset = fragmentId * views + i;
                        //image[offset] = QImage(path + "/" + file.getImages().at(i));
                        image[offset] = QImage(file.getImages().at(i));
                        perspective[offset] = file.getPyramids().at(i);
                    }
                }
            }
        } else if (xml.name() == "rotation") {
            QVector3D v;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "x") {
                     v.setX(attr.value().toDouble());
                } else if (attr.name().toString() == "y") {
                     v.setY(attr.value().toDouble());
                } else if (attr.name().toString() == "z") {
                     v.setZ(attr.value().toDouble());
                }
            }
            rotation[fragmentId] = v;
        } else if (xml.name() == "overflow") {
            overflowId += 1;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "top") {
                     overflow[overflowId][0] = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "left") {
                     overflow[overflowId][1] = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "bottom") {
                     overflow[overflowId][2] = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "right") {
                     overflow[overflowId][3] = attr.value().toInt() == 1;
                }
            }
        } else if (xml.name() == "mask") {
            maskId += 1;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "path") {
                     mask[maskId] = QImage(attr.value().toString());
                }
            }
        } else if (xml.name() == "translation") {
            QVector3D v;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "x") {
                     v.setX(attr.value().toDouble());
                } else if (attr.name().toString() == "y") {
                     v.setY(attr.value().toDouble());
                } else if (attr.name().toString() == "z") {
                     v.setZ(attr.value().toDouble());
                }
            }
            translation[fragmentId] = v;
        } else if (xml.name() == "crop") {
            cropId += 1;
            QPoint p;
            int width, height;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "x") {
                     p.setX(attr.value().toDouble());
                } else if (attr.name().toString() == "y") {
                     p.setY(attr.value().toDouble());
                } else if (attr.name().toString() == "width") {
                     width = attr.value().toDouble();
                } else if (attr.name().toString() == "height") {
                     height = attr.value().toDouble();
                }
            }
            crop[cropId] = QRect(p, QSize(width, height));
        } else if (xml.name() == "vertexCrop") {
            vertexCropId += 1;
            QPoint p;
            int width, height;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "x") {
                     p.setX(attr.value().toDouble());
                } else if (attr.name().toString() == "y") {
                     p.setY(attr.value().toDouble());
                } else if (attr.name().toString() == "width") {
                     width = attr.value().toDouble();
                } else if (attr.name().toString() == "height") {
                     height = attr.value().toDouble();
                }
            }
            vertexCrop[vertexCropId] = QRect(p, QSize(width, height));
        } else if (xml.name() == "line") {
            lineId += 1;
            QPoint p1, p2;
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "x1") {
                     p1.setX(attr.value().toDouble());
                } else if (attr.name().toString() == "y1") {
                     p1.setY(attr.value().toDouble());
                } else if (attr.name().toString() == "x2") {
                     p2.setX(attr.value().toDouble());
                } else if (attr.name().toString() == "y2") {
                     p2.setY(attr.value().toDouble());
                }
            }
            point1[lineId] = p1;
            point2[lineId] = p2;
        } else if (xml.name() == "images") {
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "save") {
                    saveImages = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "path") {
                    imagesPath = attr.value().toString();
                }
            }
        } else if (xml.name() == "measurement") {
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                if (attr.name().toString() == "save") {
                    saveMeasurement = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "file") {
                    measurementFile = attr.value().toString();
                } else if (attr.name().toString() == "hausdorff") {
                    hausdorff = attr.value().toInt() == 1;
                } else if (attr.name().toString() == "mesh") {
                    refModelFileName = attr.value().toString();
                }
            }
        }
        else {
            xml.skipCurrentElement();
        }
    }



    return true;
}
