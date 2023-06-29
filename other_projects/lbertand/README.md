# As-Rigid-As-Possible project

https://user-images.githubusercontent.com/81630925/204822066-1e2a16cc-415e-4bf0-9e3c-15358ed2c952.mp4

## Abstract

The As-Rigid-As-Possible (or in short ARAP) is a way to deform meshes using control points that preserves as much as it can the intial shape of the mesh.
This is achieved using an algorithm explained on [this paper](https://igl.ethz.ch/projects/ARAP/).


## Interface

Since we wanted to build an interaction version of the ARAP method, we had to implement an interface to go with it.
We decided to go with a design where there are 2 functionnement  modes.<br>
The first one is the selection mode, it allows to select points on the mesh and to add them or remove them from the mesh's control point list.<br>
The second mode is the grab mode, where the user can move the selected control points and see the ARAP algorithm in action.<br>
<br>
Here are the different commands:

```
			---		Common 		---
A,a				Starts an ARAP deformation (Key Q,q on azerty keyboards)
1,&				Set the ARAP intialisation method on 'Last Frame'
2,Ú				Set the ARAP intialisation method on 'Laplacian'
G,g				Toggle interface mode between Selection and Grabing

			---	Selection Mode	---
click			Set selection to clicked point
click + shift	Add/Remove clicked point to selection, or set selection to none if no clicked point
C,c				Add selected points to mesh's control points
R,r				Remove selected points to mesh's control points (Triggers ARAP with 'Laplacian' intialisation method)

			---	  Grab Mode		---
drag			Move selected control points among set axis (Triggers ARAP if intialisation method is 'Last Frame')
X,x				Set move axis to X
Y,y				Set move axis to Y
Z,z				Set move axis to Z (Key W,w on azerty keyboards)
X,x + shift		Set move plane to (Y,Z)
Y,y + shift		Set move plane to (X,Z)
Z,z + shift		Set move plane to (X,Y) (Key W,w on azerty keyboards)
```


In the selection mode, a selected point will appear as red, a selected control point as light green, and a not selected control point as drak green.<br>
In the grab mode, a selected point will appear as orange, a selected control point as a blueish light green, and a not selected control point as the same drak green.<br>
If a control point isn't at its position defined by the user, an edge will link the user's wanted position for this control point and the actual position on the mesh of the control point.<br>
<br>
When grabing a control point, the move axis appears. The red one is the X axis, the green one is the Y one, and the blue one is the Z one.


## Mesh

By default, our programm will launch using a subdivided cube. However, one can starts the program with an optional argument.
That argument will be the relative path to a external mesh file, saved in the .off format. The data folder contains several .off exmaple files that can be used.


## Use the project

### Dependencies

The only dependencies are STL, Eigen, [libigl](http://libigl.github.io/libigl/) and the dependencies
of the `igl::opengl::glfw::Viewer` (OpenGL, glad and GLFW).
The CMake build system will automatically download libigl and its dependencies using
[CMake FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html),
thus requiring no setup on your part.

To use a local copy of libigl rather than downloading the repository via FetchContent, you can use
the CMake cache variable `FETCHCONTENT_SOURCE_DIR_LIBIGL` when configuring your CMake project for
the first time:
```
cmake -DFETCHCONTENT_SOURCE_DIR_LIBIGL=<path-to-libigl> ..
```
When changing this value, do not forget to clear your `CMakeCache.txt`, or to update the cache variable
via `cmake-gui` or `ccmake`.

### Compile

Compile this project using the standard cmake routine:

```
    mkdir build
    cd build
    cmake ..
    make
```

This should find and build the dependencies and create the needed files, including the project .sln file.
