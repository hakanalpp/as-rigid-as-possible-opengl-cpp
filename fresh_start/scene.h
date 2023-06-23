#pragma once

#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/upsample.h>

#include "mesh.h"

struct Scene {
    private:

    public:
        Scene() {};
        std::vector<int>	selection;
        void displaySelectedPoints(igl::opengl::glfw::Viewer& viewer, const Mesh& mesh) const;
        std::vector<int> getNonSelectedControlPointsIndex(const Mesh& mesh) const;
        std::vector<int> getSelectedControlPointsIndex(const Mesh& mesh, bool invert = false) const;
};