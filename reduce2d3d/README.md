
Reduce 2D-3D application 
===============================================================================

Description
-------------------------------------------------------------------------------
Reduce 2D-3D application is a command line interface for the Multi-fragment 
Registration Library (https://github.com/klepo/libmultifragmentregister), 
which is a C++ Qt-based library for 2D-3D atlas-based reconstruction 
and fracture reduction of traumatized long bones.   

Version from 12 July 2017.

Binaries
-------------------------------------------------------------------------------
The 32-bit binaries with all dependent libraries are available 
in the "bin/x86/release" folder.

 
Examples
-------------------------------------------------------------------------------
There are two examples focused on reconstruction of fractured tibia and femur.
Please copy the folder "Reduce2D3D_examples" to the "C:/Reduce2D3D_examples"
location. Then you can execute the examples by typing:

reduce2d3d.exe C:/Reduce2D3D_examples/femur/ reduce.manual.xml



    
Prerequisites
-------------------------------------------------------------------------------
Qt Toolkit in version 5.x or later is required. Other required librarires are 
redistributed within the registration library package. The libraries are 
redistributed in both source and binary form. Following 3rd party libraries are 
included:

 * SSIM Renderer (http://www.fit.vutbr.cz/research/prod/?id=458)
 * Multi-fragment Registration Library (https://github.com/klepo/libmultifragmentregister)
 * dlib
 * OpenCV
 * libmesh
    
The library has been developped for usage with Windows 7 or later.
   
   
Downloading
-------------------------------------------------------------------------------

The Reduce2D3D application can be further redistributed under the terms 
of the LGPL version 3 open source license. 
The library can be obtained from the following location: 
<http://www.fit.vutbr.cz/research/prod/?id=505>


Acknowledgement
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