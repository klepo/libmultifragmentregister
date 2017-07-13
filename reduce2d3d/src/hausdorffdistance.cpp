/**
 * @file        hausdorffdistance.cpp
 * @author      Ondrej Klima, BUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        11 July 2017
 *
 * @brief       The implementation file containing functions for Hausdorff distance evaluation.
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

#include "hausdorffdistance.h"

#include <qmath.h>

/**
 * @brief meshModel Creates a model structure from arrays of vertices and faces
 * @param vertices Array of float vertices
 * @param nv Number of vertices
 * @param triangles Array of faces
 * @param nt Number of faces
 * @return Initialized model structure
 */
struct model meshModel(float * vertices, int nv, int * triangles, int nt)
{
    struct model m;
    m.num_faces = nt / 3;
    m.faces = new face_t[m.num_faces];

    for(int i = 0; i < nt / 3; i++)
    {
        face_t face;
        face.f0 = *(3 * i + 0 + triangles);
        face.f1 = *(3 * i + 1 + triangles);
        face.f2 = *(3 * i + 2 + triangles);

        m.faces[i] = face;
    }

    nv = 10000;
    m.num_vert = nv;
    m.vertices = new vertex_t[m.num_vert];

    vertex_t bb1min, bb1max;
    bb1min.x = bb1min.y = bb1min.z =  FLT_MAX;
    bb1max.x = bb1max.y = bb1max.z = -FLT_MAX;

    for(int i = 0; i < nv; i++) {
        vertex_t vertex1;

        vertex1.x = *(3 * i + 0 + vertices);
        vertex1.y = *(3 * i + 1 + vertices);
        vertex1.z = *(3 * i + 2 + vertices);

        if (vertex1.x < bb1min.x) bb1min.x = vertex1.x;
        if (vertex1.x > bb1max.x) bb1max.x = vertex1.x;
        if (vertex1.y < bb1min.y) bb1min.y = vertex1.y;
        if (vertex1.y > bb1max.y) bb1max.y = vertex1.y;
        if (vertex1.z < bb1min.z) bb1min.z = vertex1.z;
        if (vertex1.z > bb1max.z) bb1max.z = vertex1.z;

        m.vertices[i] = vertex1;
    }

    m.bBox[0] = bb1min;
    m.bBox[1] = bb1max;

    m.builtin_normals = 0;
    m.normals = NULL;
    m.face_normals = NULL;
    m.area = NULL;
    m.total_area = 0;
    m.tree = NULL;

    return m;
}

/**
 * @brief hausdorffDistance Computes Hausdorff distance between two surfaces
 * @param m The first surface described by the model structure
 * @param fileName The second surface stored as a ply file
 * @param max Maximal distance
 * @param mean Mean distance
 * @param rms Distance RMS
 * @param max_1 Maximal distance from m to ply file
 * @param mean_1 Mean distance from m to ply file
 * @param rms_1 RMS of distance from m to ply file
 * @param max_2 Maximal distance from ply file to m
 * @param mean_2 Mean distance from ply file to m
 * @param rms_2 RMS of distance from ply file to m
 * @return
 */
struct model hausdorffDistance(struct model m, const char *fileName,
                       double &max, double &mean, double &rms,
                       double &max_1, double &mean_1, double &rms_1,
                       double &max_2, double &mean_2, double &rms_2)
{
    struct args pargs;
    memset(&pargs,0,sizeof(pargs));

    pargs.do_symmetric = 1;
    pargs.no_gui = 1;
    pargs.quiet = 1;
    pargs.sampling_step = 0.1;
    pargs.min_sample_freq = 5;

    struct model_error model1, model2;

    struct dist_surf_surf_stats stats;
    struct dist_surf_surf_stats stats_rev;
    double bbox1_diag,bbox2_diag;
    double abs_sampling_step,abs_sampling_dens;

    /* Initialize application */
    memset(&model1,0,sizeof(model1));
    memset(&model2,0,sizeof(model2));

    model1.mesh = &m;
    pargs.m2_fname = (char*)fileName;
    model2.mesh = read_model_file(pargs.m2_fname);

    bbox1_diag = dist_v(&model1.mesh->bBox[0], &model1.mesh->bBox[1]);
    bbox2_diag = dist_v(&model2.mesh->bBox[0], &model2.mesh->bBox[1]);

    struct model_info *m1info,*m2info;
    m1info = (struct model_info*) xa_malloc(sizeof(*m1info));
    m2info = (struct model_info*) xa_malloc(sizeof(*m2info));

    analyze_model(model2.mesh,m2info,1,0,NULL,"model 2");
    model2.info = m2info;

    analyze_model(model1.mesh,m1info,0,0,NULL,"model 1");
    model1.info = m1info;

    abs_sampling_step = pargs.sampling_step*bbox2_diag;
    abs_sampling_dens = 1/(abs_sampling_step*abs_sampling_step);

    /* Print available model information */
    printf("\n                      Model information\n"
                  "     (degenerate faces ignored for manifold/closed info)\n\n");
    printf("Number of vertices:      \t%11d\t%11d\n",
                  model1.mesh->num_vert,model2.mesh->num_vert);
    printf("Number of triangles:     \t%11d\t%11d\n",
                  model1.mesh->num_faces,model2.mesh->num_faces);
    printf("Degenerate triangles:    \t%11d\t%11d\n",
                  m1info->n_degenerate,m2info->n_degenerate);
    printf("BoundingBox diagonal:    \t%11g\t%11g\n",
                  bbox1_diag,bbox2_diag);
    printf("Number of disjoint parts:\t%11d\t%11d\n",
                  m1info->n_disjoint_parts,m2info->n_disjoint_parts);
    printf("Manifold:                \t%11s\t%11s\n",
                  (m1info->manifold ? "yes" : "no"),
                  (m2info->manifold ? "yes" : "no"));
    printf("Originally oriented:     \t%11s\t%11s\n",
                  (m1info->orig_oriented ? "yes" : "no"),
                  (m2info->orig_oriented ? "yes" : "no"));
    printf("Orientable:              \t%11s\t%11s\n",
                  (m1info->orientable ? "yes" : "no"),
                  (m2info->orientable ? "yes" : "no"));
    printf("Closed:                  \t%11s\t%11s\n",
                  (m1info->closed ? "yes" : "no"),
                  (m2info->closed ? "yes" : "no"));

    dist_surf_surf(&model1, model2.mesh, abs_sampling_dens, pargs.min_sample_freq,
                   &stats, pargs.no_gui, NULL);

    dist_surf_surf(&model2, model1.mesh, abs_sampling_dens, pargs.min_sample_freq,
                   &stats_rev, pargs.no_gui, NULL);

    printf("\n       Distance from model 1 to model 2\n\n");
    printf("        \t   Absolute\t%% BBox diag\n");
    printf("        \t           \t  (Model 2)\n");
    printf("Min:    \t%11g\t%11g\n",
                  stats.min_dist,stats.min_dist/bbox2_diag*100);
    printf("Max:    \t%11g\t%11g\n",
                  stats.max_dist,stats.max_dist/bbox2_diag*100);
    printf("Mean:   \t%11g\t%11g\n",
                  stats.mean_dist,stats.mean_dist/bbox2_diag*100);
    printf("RMS:    \t%11g\t%11g\n",
                  stats.rms_dist,stats.rms_dist/bbox2_diag*100);
    printf("\n");

    max_1  = stats.max_dist;
    mean_1 = stats.mean_dist;
    rms_1  = stats.rms_dist;

    printf("       Distance from model 2 to model 1\n\n");

    printf("        \t   Absolute\t%% BBox diag\n");
    printf("        \t           \t  (Model 2)\n");
    printf("Min:    \t%11g\t%11g\n",
                  stats_rev.min_dist,stats_rev.min_dist/bbox2_diag*100);
    printf("Max:    \t%11g\t%11g\n",
                  stats_rev.max_dist,stats_rev.max_dist/bbox2_diag*100);
    printf("Mean:   \t%11g\t%11g\n",
                  stats_rev.mean_dist,stats_rev.mean_dist/bbox2_diag*100);
    printf("RMS:    \t%11g\t%11g\n",
                  stats_rev.rms_dist,stats_rev.rms_dist/bbox2_diag*100);
    printf("\n");

    max_2  = stats_rev.max_dist;
    mean_2 = stats_rev.mean_dist;
    rms_2  = stats_rev.rms_dist;


    /* Print symmetric distance measures */
    printf("       Symmetric distance between model 1 and model 2\n\n");
    printf("        \t   Absolute\t%% BBox diag\n");
    printf("        \t           \t  (Model 2)\n");
    printf("Min:    \t%11g\t%11g\n",
                  qMax(stats.min_dist, stats_rev.min_dist),
                  qMax(stats.min_dist, stats_rev.min_dist) /bbox2_diag*100);
    printf("Max:    \t%11g\t%11g\n",
                  qMax(stats.max_dist, stats_rev.max_dist),
                  qMax(stats.max_dist, stats_rev.max_dist) /bbox2_diag*100);
    printf("Mean:   \t%11g\t%11g\n",
                  qMax(stats.mean_dist, stats_rev.mean_dist),
                  qMax(stats.mean_dist, stats_rev.mean_dist) /bbox2_diag*100);
    printf("RMS:    \t%11g\t%11g\n",
                  qMax(stats.rms_dist, stats_rev.rms_dist),
                  qMax(stats.rms_dist, stats_rev.rms_dist) /bbox2_diag*100);
    printf("\n");

    // qMax(stats.min_dist, stats_rev.min_dist);
    max  = qMax(stats.max_dist,  stats_rev.max_dist);
    mean = qMax(stats.mean_dist, stats_rev.mean_dist);
    rms  = qMax(stats.rms_dist,  stats_rev.rms_dist);

    return *model2.mesh;
}

/**
 * @brief Reads a model from file 'fname' and returns the model read. If an error
 * occurs a message is printed and the program exists
 * @param fname File name of the model in the ply format
 * @return Initialized model structure
 */
struct model *read_model_file(const char *fname)
{
  int rcode;
  struct model *m;
  const char *errstr;

  rcode = read_fmodel(&m,fname,MESH_FF_AUTO,1);
  if (rcode <= 0) {
    switch (rcode) {
    case 0:
      errstr = "no models in file";
      break;
    case MESH_NO_MEM:
      errstr = "no memory";
      break;
    case MESH_CORRUPTED:
      errstr = "corrupted file or I/O error";
      break;
    case MESH_MODEL_ERR:
      errstr = "model error";
      break;
    case MESH_NOT_TRIAG:
      errstr = "not a triangular mesh model";
      break;
    case MESH_BAD_FF:
      errstr = "unrecognized file format";
      break;
    case MESH_BAD_FNAME:
      errstr = strerror(errno);
      break;
    default:
      errstr = "unknown error";
    }
    fprintf(stderr,"ERROR: %s: %s\n",fname,errstr);
    exit(1);
  } else if (m->num_faces == 0) {
    fprintf(stderr,"ERROR: %s: empty model (no faces)\n",fname);
    exit(1);
  }
  return m;
}
