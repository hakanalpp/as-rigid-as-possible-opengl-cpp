#include "window.h"
#include <iostream>

std::unordered_map<SelectionMode, std::string> SelectionModeString = {{NONE, "NONE"},
                                                                      {STATIC, "STATIC"},
                                                                      {DYNAMIC, "DYNAMIC"}};

void Window::initTextPanel()
{
    double doubleVariable = 0.1f; // Shared between two menus
    menu.callback_draw_custom_window = [&]()
    {
        ImGui::SetNextWindowPos(ImVec2(981.f * menu.menu_scaling(), 0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 160), ImGuiCond_FirstUseEver);
        ImGui::Begin(
            "Helper Window", nullptr,
            ImGuiWindowFlags_NoSavedSettings);
        ImGui::InputText("Control Pointer Selection Mode", SelectionModeString[cp_selection_mode]);
        ImGui::End();
    };
}

void Window::projectOnMoveDirection(Eigen::Vector3d &projectionReceiver)
{
    double x = viewer.current_mouse_x;
    double y = viewer.current_mouse_y;

    Eigen::Vector4f planeEquation = Eigen::Vector4f(1, 1, 1, 1);
    igl::unproject_on_plane(Eigen::Vector2f(x, y), viewer.core().proj, viewer.core().viewport, planeEquation, projectionReceiver);
}

void Window::displaySelectedPoints()
{
    Eigen::MatrixXd cp_none = scene.mesh.getVerticesFromIndex(scene.mesh.selectedPoints);
    Eigen::MatrixXd cp_dynamic = scene.mesh.getVerticesFromIndex(scene.mesh.getControlPoints(DYNAMIC));
    Eigen::MatrixXd cp_static = scene.mesh.getVerticesFromIndex(scene.mesh.getControlPoints(STATIC));

    viewer.data().set_points(cp_none, Eigen::RowVector3d(1, 0, 0));
    viewer.data().add_points(cp_dynamic, Eigen::RowVector3d(0, 1, 0));
    viewer.data().add_points(cp_static, Eigen::RowVector3d(0, 0, 1));
}

void Window::launch()
{
    viewer.launch();
}

void Window::onMousePressed(bool isShiftPressed)
{
    mousePressed = true;

    int fid;
    Eigen::Vector3d bc;
    // Cast a ray in the view direction starting from the mouse position
    double x = viewer.current_mouse_x;
    double y = viewer.core().viewport(3) - viewer.current_mouse_y;

    mousePressedCoords = Eigen::Vector2d(x, y);

    if (cp_selection_mode != NONE)
    {
        bool kk = igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view, viewer.core().proj, viewer.core().viewport, scene.mesh.V, scene.mesh.F, fid, bc);
        scene.mesh.addSelectedPoint(fid, bc, kk, isShiftPressed);
        displaySelectedPoints();
    }
}

void Window::onMouseReleased()
{
    // 1280x800
    if (moveMode)
    {
        double x = viewer.current_mouse_x;
        double y = viewer.core().viewport(3) - viewer.current_mouse_y;
        Eigen::Vector2d grabbed = Eigen::Vector2d(x, y) - mousePressedCoords;

        x = grabbed.x() / 1280.0;
        y = grabbed.y() / 800.0;

        Eigen::Vector3d up = Eigen::Vector3d(0, 1, 0);
        Eigen::Vector3d right = Eigen::Vector3d(5, 0, 0);

        Eigen::Vector3d projPoint = right * x + up * y;

        projPoint.normalize();
        scene.mesh.moveControlPoints(projPoint / 5);
        scene.meshArap.guiToAlgorithm(scene.mesh.C);
        scene.meshArap.ARAP(10);

        std::cout << "ARAP" << std::endl;
        scene.meshArap.mesh_frame_update();
        scene.mesh.algorithmToGui(scene.meshArap);
        viewer.data().set_mesh(scene.mesh.V, scene.mesh.F);
        displaySelectedPoints();
        std::cout << "move mode" << std::endl;
    }

    mousePressed = false;
    moveMode = false;
}

bool Window::onMouseMoved()
{
    if (mousePressed && !moveMode)
    {
        return false;
    }
    return true;
}

void Window::changeCPSelectionMode()
{
    switch (cp_selection_mode)
    {
    case NONE:
        cp_selection_mode = STATIC;
        break;
    case STATIC:
        cp_selection_mode = DYNAMIC;
        break;
    case DYNAMIC:
        cp_selection_mode = NONE;
        break;
    }
    menu.draw_custom_window();
}

void Window::onKeyPressed(unsigned char key, bool isShiftPressed)
{
    if (key == 'C')
    {
        changeCPSelectionMode();
        scene.mesh.selectedPoints.clear();
        displaySelectedPoints();
    }
    else if (key == 'V')
    {
        if (cp_selection_mode == NONE)
        {
            return;
        }

        scene.mesh.addControlPoints(cp_selection_mode);
        scene.mesh.selectedPoints.clear();
        displaySelectedPoints();
    }
    else if (key == 'G')
    {
        moveMode = !moveMode;
    }
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
        onKeyPressed(key, modifier & 0x00000001);
        return false;
    };

    return true;
}