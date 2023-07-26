#pragma once

#include "scene.h"

struct Window
{
public:
    Scene scene;
    igl::opengl::glfw::Viewer viewer;
    igl::opengl::glfw::imgui::ImGuiMenu menu;

    SelectionMode cp_selection_mode = NONE;

    Eigen::Vector2d mousePressedCoords;
    bool mousePressed = false;
    bool moveMode = false;

    Window(Scene scene) : scene(scene)
    {
        viewer.data().set_mesh(scene.mesh.V, scene.mesh.F);
        viewer.data().set_face_based(true);
        viewer.plugins.push_back(&menu);
        viewer.core().set_rotation_type(igl::opengl::ViewerCore::ROTATION_TYPE_TRACKBALL);
        initTextPanel();
        initializeKeyCallbacks();
    };

    void initTextPanel();
    void onMousePressed(bool isShiftPressed);
    void onMouseReleased();
    bool onMouseMoved();
    void onKeyPressed(unsigned char key, bool isShiftPressed);
    bool initializeKeyCallbacks();
    void launch();
    void changeCPSelectionMode();
    void displaySelectedPoints();
    void projectOnMoveDirection(Eigen::Vector3d &projectionReceiver);
};