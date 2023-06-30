#pragma once

#include "mesh.h"

struct Scene
{
public:
    Mesh mesh;
    MeshArap meshArap;

    Scene(Mesh mesh, MeshArap meshArap) : mesh(mesh), meshArap(meshArap) {
    };


private:
};