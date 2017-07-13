/**
 * @file        hausdorffdistance.h
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The header file containing declarations of functions for Hausdorff distance evaluation.
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

#ifndef HAUSDORFFDISTANCE_H
#define HAUSDORFFDISTANCE_H

#include <cmath>
#include <iostream>
#include <vector>

#include <mesh.h>
#include <mesh_run.h>
#include <lib3d/3dmodel.h>
#include <lib3d/model_in.h>
#include <compute_error.h>
#include <lib3d/geomutils.h>
#include <xalloc.h>

// reads a ply model and fill a model structure
struct model *read_model_file(const char *fname);

// creates a model structure from arrays of vertices and faces
struct model meshModel(float * vertices, int nv, int * triangles, int nt);

// computes Hausdorff distance between two models
model hausdorffDistance(struct model m, const char *fileName, double &max, double &mean, double &rms, double &max_1, double &mean_1, double &rms_1, double &max_2, double &mean_2, double &rms_2);

#endif // HAUSDORFFDISTANCE_H

