#pragma once

#include "mesh.h"

struct Scene
{
public:
    Mesh mesh;

    Scene(Mesh mesh) : mesh(mesh){};

    void displaySelectedPoints(igl::opengl::glfw::Viewer &viewer, const Mesh &mesh) const;

private:
};