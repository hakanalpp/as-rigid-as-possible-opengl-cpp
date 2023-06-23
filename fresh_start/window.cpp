#include <igl/unproject_onto_mesh.h>
#include <igl/unproject_on_line.h>
#include <igl/unproject_on_plane.h>
#include <igl/point_mesh_squared_distance.h>

#include "mesh.h"
#include "window.h"
#include "scene.h"

void Window::onMousePressed(igl::opengl::glfw::Viewer& viewer, Scene& scene, Mesh& mesh, bool isShiftPressed)
{
    mousePressed = true;

    // if (moveSelectionMode)
    // {
    //     projectOnMoveDirection(viewer, lastProjectedPoint);
    //     return;
    // }

    int fid;
    Eigen::Vector3d bc;
    // Cast a ray in the view direction starting from the mouse position
    double x = viewer.current_mouse_x;
    double y = viewer.core().viewport(3) - viewer.current_mouse_y;
    if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view, viewer.core().proj, viewer.core().viewport, mesh.V, mesh.F, fid, bc))
    {
        int closestVertex = 0;
        for (int i = 1; i < 3; i++)
            if (bc[i] > bc[closestVertex])
                closestVertex = i;

        // if (isShiftPressed)
        // {
        //     int selectedVertexIndex = mesh.F.row(fid)[closestVertex];
        //     int indexOnVectorIfExists = -1;
        //     for (int i = 0; i < selection.size(); i++)
        //         if (selection[i] == selectedVertexIndex)
        //         {
        //             indexOnVectorIfExists = i;
        //             break;
        //         }
        //     if (indexOnVectorIfExists < 0)
        //         selection.push_back(selectedVertexIndex);   // not in selection : add it
        //     else
        //         selection.erase(std::next(selection.begin(), indexOnVectorIfExists));   // already in the selection : remove it
        // }
        // else
        // {
        scene.selection.clear();
        scene.selection.push_back(mesh.F.row(fid)[closestVertex]);
        // }
    }
    // else if (isShiftPressed)
    //     selection.clear();

    scene.displaySelectedPoints(viewer, mesh);
}

void Window::onMouseReleased()
{
    mousePressed = false;
}

bool Window::onMouseMoved(igl::opengl::glfw::Viewer& viewer, Scene& scene, Mesh& mesh)
{
    if (mousePressed)
        return false;

    // if (selection.size() == 0)
    //     return true;

    scene.displaySelectedPoints(viewer, mesh);
    return true;
}

