Multi-fragment Registration Library 
===============================================================================
Multi-fragment Registration Library is a C++ Qt-based library for 2D-3D 
atlas-based reconstruction and fracture reduction of traumatized long bones.   

Description
-------------------------------------------------------------------------------
The main aim of the library is a 2D-3D reconstruction of long bones from plain
radiographs. The reconstruction is based on deformable registration of 3D 
statistical shape and intensity model to 2D X-ray images. The relative pose 
of the radiographs must be known and an initial estimation of the shape model 
pose must be provided by the user. The result of the reconstruction is 
a polygonal model of complete bone. The library provides both density-based
and feature-based reconstruction approaches. The reconstruction is available
even for dislocated fractures of long bones. The library provides multi-fragment 
2D-3D registration with simultaneous fracture reduction - the complete bone 
can be reconstructed from individual bone fragments. The accurate length of
a bone reconstructed from multiple fragments is ensured by proposed vertex 
metric. The reconstruction pipeline is accelerated using OpenGL rendering and 
OpenGL shaders implementation of image similarity metrics.

Version from 12 July 2017.

 
Features
-------------------------------------------------------------------------------
 * Density-based non-rigid 2D-3D registration of shape model to radiographs   
 * Feature-based deformable 2D-3D registration     
 * Simultaneous reconstruction and fracture reduction of injured long bones
 * Reconstruction from multiple bone fragments
 * Accurate bone length estimation
 * Fast registration ensured by OpenGL acceleration
 * etc.      

Prerequisites
-------------------------------------------------------------------------------
Qt Toolkit in version 5.x or later is required. Other required librarires are 
redistributed within the registration library package. The libraries are 
redistributed in both source and binary form. Following 3rd party libraries are 
included:

 * SSIM Renderer (http://www.fit.vutbr.cz/research/prod/?id=458)
 * dlib
    
The library has been developped for usage with Windows 7 or later.

Examples
-------------------------------------------------------------------------------
To get started with the SSIM Renderer library, please have a look 
at the included examples:

 * DensityRegistration.cpp
   - The first example performs single fragment density-based 2D-3D 
     reconstruction of femoral bone
       
 * FeatureRegistration.cpp                                               
   - The second example shows feature-based registration based on binary masks 
     registration
      
 * MultiFragmentFeatureRegistration.cpp
   - The last example shows fractured bone reconstruction with accurate length 
     estimation using the feature-based registration and vertex metric
   
   
There is also a full reference manual available.

Downloading
-------------------------------------------------------------------------------
The SSIM Renderer library can be further redistributed under the terms 
of the LGPL version 3 open source license. 
The library can be obtained from the following location: 
<http://www.fit.vutbr.cz/research/prod/?id=505>

Acknowledgment
-------------------------------------------------------------------------------
This work has been supported by the Technology Agency of the Czech Republic 
(TA CR, Project Id: TA04011606).

Authors
-------------------------------------------------------------------------------
 * Ondrej Klima     (<iklima@fit.vutbr.cz>)
 * Petr Kleparnik   (<ikleparnik@fit.vutbr.cz>)
 * Michal Spanel    (<spanel@fit.vutbr.cz>)
 * Pavel Zemcik     (<zemcik@stud.fit.vutbr.cz>)

References
-------------------------------------------------------------------------------
[1] KLIMA Ondrej, KLEPARNIK Petr, SPANEL Michal a ZEMCIK Pavel. Intensity-based 
    femoral atlas 2D/3D registration using Levenberg-Marquardt optimisation. 
    In: Biomedical Applications in Molecular, Structural, and Functional 
    Imaging. San Diego: SPIE - the international society for optics 
    and photonics, 2016, s. 1-6. ISBN 978-1-5106-0023-2.
    
[2] KLIMA Ondrej, KLEPARNIK Petr, SPANEL Michal a ZEMCIK Pavel. Towards 
    an accurate 3D reconstruction of fractured long bones from plain 2D 
    radiographs. Proceedings of the 30th International Congress and Exhibition 
    on Computer Assisted Radiology and Surgery. Heidelberg, 2016.
    
[3] KLIMA Ondrej, CHROMY Adam, ZEMCIK Pavel, KLEPARNIK Petr a SPANEL Michal. 
    A Study on Performace of Levenberg-Marquardt and CMA-ES Optimization Methods 
    for Atlas-based 2D/3D Reconstruction. In: Proceedings of PDeS. Brno: 
    Institute of Electrical and Electronics Engineers, 2016, s. 1-6. 
    ISBN 978-3-902823-53-3. 
