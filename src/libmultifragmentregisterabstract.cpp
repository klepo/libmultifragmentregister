/**
 * @file        libmultifragmentregisterabstract.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        15 December 2016
 *
 * @brief       The implementation file containing the LibMultiFragmentRegisterAbstract class.
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

#include "libmultifragmentregisterabstract.h"

/**
 * @brief Loads float values from CSV file
 * @param filename The CSV file name
 * @return Vector of float values
 *
 * The values in CSV file have to be separated by semilocon character
 *
 */
QVector<float> LibMultiFragmentRegisterAbstract::
loadDataFromCSVFile(QString filename)
{
    // Try open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open " + filename;
        throw std::runtime_error(std::string("Error opening CSV file ") + filename.toStdString());
    }

    QVector<float> array;
    QList<QByteArray> list;
    int rows = 0;
    int cols = 0;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        list = line.split(';');
        cols = list.count();
        foreach (QByteArray item, list) {
            array.append(item.trimmed().toFloat());
        }
        rows++;
    }
    file.close();
    return array;
}

/**
 * @brief Displays the matrix values
 * @param[in] g Matrix to be displayed
 */
void LibMultiFragmentRegisterAbstract::
printMatrix(const dlib::matrix<float> & g)
{
    for(int row = 0; row < g.nr(); row++)
    {
        QStringList stringList;
        for(int col = 0; col < g.nc(); col++)
            stringList << QString::number(g(row, col));
        qDebug() << stringList.join(" ");
    }
}

/**
 * @brief Creates a binary mask from given image
 * @param image Image to be masked
 * @return Binary image mask
 */
QImage LibMultiFragmentRegisterAbstract::maskImage(const QImage & image)
{
    QImage result(image.size(), QImage::Format_Indexed8); //image.format());
    result.setColor(255, qRgb(255,255,255));
    result.setColor(0, qRgb(0,0,0));
    result.fill(0);

    for(int x = 0; x < result.width(); x++)
        for(int y = 0; y < result.height(); y++)
            if(qRed(image.pixel(x, y)) > 10 || qGreen(image.pixel(x, y)) > 10 || qBlue(image.pixel(x, y)) > 10)
                result.setPixel(x, y, 255);

    return result;
}

/**
 * @brief Trims given image
 * @param image Image to be trimmed
 * @return Trimmed image
 */
QRect LibMultiFragmentRegisterAbstract::
cropImage(const QImage & image)
{
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;

    QImage nimage = image.convertToFormat(QImage::Format_RGB888);

    for(int x = 0; x < nimage.width(); x++)
        for(int y = 0; y < nimage.height(); y++)
            if(qRed(nimage.pixel(x, y)) > 10)
            {
                left = x;
                goto next1;
            }
    next1:
    for(int x = nimage.width() - 1; x >=0; x--)
        for(int y = 0; y < nimage.height(); y++)
            if(qRed(nimage.pixel(x, y)) > 10)
            {
                right = x;
                goto next2;
            }
    next2:
    for(int y = 0; y < nimage.height(); y++)
        for(int x = 0; x < nimage.width(); x++)
            if(qRed(nimage.pixel(x, y)) > 10)
            {
                top = y;
                goto next3;
            }
    next3:
    for(int y = nimage.height() - 1; y >=0; y--)
        for(int x = 0; x < nimage.width(); x++)
            if(qRed(nimage.pixel(x, y)) > 10)
            {
                bottom = y;
                goto next4;
            }
    next4:

    left -= 8;
    top -= 8;
    bottom += 8;
    right += 8;

    if(left < 0) left = 0;
    if(top  < 0) top = 0;

    if(right  > nimage.rect().right())  right  = nimage.rect().right();
    if(bottom > nimage.rect().bottom()) bottom = nimage.rect().bottom();

    return QRect(QPoint(left, top), QPoint(right, bottom));
}
