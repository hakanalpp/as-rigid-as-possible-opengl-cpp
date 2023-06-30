#pragma once

#include "scene.h"
#include "camera.h"


struct Window
{
public:
    Scene scene;
    Camera camera;
    igl::opengl::glfw::Viewer viewer;
    igl::opengl::glfw::imgui::ImGuiMenu menu;

    SelectionMode cp_selection_mode = NONE;

    bool mousePressed = false;
    bool moveMode = false;

    Window(Scene scene, Camera camera) : scene(scene), camera(camera)
    {
        viewer.data().set_mesh(scene.mesh.V, scene.mesh.F);
        viewer.data().set_face_based(true);
        viewer.plugins.push_back(&menu);
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
    void projectOnMoveDirection(Eigen::Vector3d& projectionReceiver);
};