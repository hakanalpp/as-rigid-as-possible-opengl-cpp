# ARAP (As-rigid-as-possible) and OpenGL Viewer

## Getting Started

CMake build system should automatically install dependencies. If there is problem, please make sure to install following:
- STL
- Eigen
- libigl

## Running the project

You need to compile the project like following.

```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

And run the project using

```sh
$ ./arap_cpp ../data/<mesh_obj>.obj
```

Or for the video generating sake (This is for our internal usage. No need to test)

```sh
$ ./arap_cpp video <mesh_path> <video_path>
```

## Tested environment   
- Operating System: Fedora 37
- No GPU needed

## Authors
* **Hakan Alp** - *GUI work and maintain* - [hakanalpp](https://github.com/hakanalpp)
* **Burak Berk Ozer** - *GUI work* - [burakberk](https://github.com/burakberk)
* **Nur Gizem Altintas** - *ARAP implementation* - [gizemaltintas](https://github.com/gizemaltintas)
* **Umut Kocasari** - *ARAP implementation* - [kocasariumut](https://github.com/kocasariumut)