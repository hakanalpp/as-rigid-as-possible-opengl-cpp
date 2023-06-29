#pragma once

#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/upsample.h>

#include "mesh.h"

struct Scene
{
public:
    Mesh mesh;

    Scene(Mesh mesh) : mesh(mesh){};

    void displaySelectedPoints(igl::opengl::glfw::Viewer &viewer, const Mesh &mesh) const;

private:
};