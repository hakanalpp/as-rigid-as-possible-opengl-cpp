#include "window.h"

void Window::launch()
{
    viewer.launch();
}

void Window::onMousePressed(bool isShiftPressed)
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
    if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view, viewer.core().proj, viewer.core().viewport, scene.mesh.V, scene.mesh.F, fid, bc))
    {
        int closestVertex = 0;
        for (int i = 1; i < 3; i++)
            if (bc[i] > bc[closestVertex])
                closestVertex = i;

        if (isShiftPressed)
        {
            int selectedVertexIndex = scene.mesh.F.row(fid)[closestVertex];
            int indexOnVectorIfExists = -1;
            for (int i = 0; i < scene.mesh.selectedPoints.size(); i++)
                if (scene.mesh.selectedPoints[i] == selectedVertexIndex)
                {
                    indexOnVectorIfExists = i;
                    break;
                }
            if (indexOnVectorIfExists < 0)
                scene.mesh.selectedPoints.push_back(selectedVertexIndex); // not in selection : add it
            else
                scene.mesh.selectedPoints.erase(std::next(scene.mesh.selectedPoints.begin(), indexOnVectorIfExists)); // already in the selection : remove it
        }
        else
        {
            scene.mesh.selectedPoints.clear();
            scene.mesh.selectedPoints.push_back(scene.mesh.F.row(fid)[closestVertex]);
        }
    }
    else if (isShiftPressed)
        scene.mesh.selectedPoints.clear();

    scene.displaySelectedPoints(viewer, scene.mesh);
}

void Window::onMouseReleased()
{
    mousePressed = false;
}

bool Window::onMouseMoved()
{
    if (mousePressed)
        return false;

    // if (selection.size() == 0)
    //     return true;

    scene.displaySelectedPoints(viewer, scene.mesh);
    return true;
}

void Window::onKeyPressed(unsigned char key, bool isShiftPressed)
{
    // //std::cout << "pressed Key: " << key << " " << (unsigned int)key << std::endl;
    // if (key == 'A')
    // {
    //     needArap = true;
    // }
    // else if (key == '1')
    // {
    //     std::cout << "ARAP Initialization set to : \t\tLast Frame" << std::endl;
    //     initType = EInitialisationType::e_LastFrame;
    // }
    // else if (key == '2')
    // {
    //     std::cout << "ARAP Initialization set to : \t\tLaplace intialisation" << std::endl;
    //     initType = EInitialisationType::e_Laplace;
    // }
    // else if (key == 'G')
    // {
    //     // Swap mode : move or select
    //     moveSelectionMode = !moveSelectionMode;
    //     displaySelectedPoints(viewer, mesh);
    // }
    // else if (key == 'C')
    // {
    //     for (const auto& i : selection)
    //         mesh.addControlPoint(i);
    //     displaySelectedPoints(viewer, mesh);
    // }
    // else if (key == 'R')
    // {
    //     int nbCP = mesh.getControlPointCount();
    //     for (const auto& i : selection)
    //         mesh.removeControlPoint(i);
    //     displaySelectedPoints(viewer, mesh);
    //     removedCP = nbCP != mesh.getControlPointCount();    // the number of cp is different -> one has been removed
    // }
    // else if (key == 'X')
    //     setMoveDirection(Eigen::Vector3d(1, 0, 0), isShiftPressed, viewer, mesh);

    // else if (key == 'Y')
    //     setMoveDirection(Eigen::Vector3d(0, 1, 0), isShiftPressed, viewer, mesh);

    // else if (key == 'Z')
    //     setMoveDirection(Eigen::Vector3d(0, 0, 1), isShiftPressed, viewer, mesh);
}

bool Window::initializeKeyCallbacks()
{
    viewer.callback_mouse_down = [this](igl::opengl::glfw::Viewer &viewer, int, int modifier) -> bool
    {
        onMousePressed(modifier & 0x00000001);
        return false;
    };
    viewer.callback_mouse_up = [this](igl::opengl::glfw::Viewer &viewer, int, int) -> bool
    {
        onMouseReleased();
        return false;
    };
    viewer.callback_mouse_move = [this](igl::opengl::glfw::Viewer &viewer, int, int modifier) -> bool
    {
        return onMouseMoved();
    };
    viewer.callback_key_down = [this](igl::opengl::glfw::Viewer &viewer, unsigned char key, int modifier) -> bool
    {
        bool removedCP = false;
        onKeyPressed(key, modifier & 0x00000001);
        // if (removedCP)
        // {
        //     if (mesh.getControlPointCount() == 0)
        //     {
        //         mesh.V = V_save;
        //         interfaceManager.displaySelectedPoints(viewer, mesh);
        //         viewer.data().set_mesh(mesh.V, mesh.F);
        //     }
        //     else
        //         performARAP(mesh, EInitialisationType::e_Laplace, viewer, interfaceManager);
        // }

        return false;
    };

    return true;
}