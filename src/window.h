#pragma once

#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/upsample.h>
#include <iostream>
#include "scene.h"
#include "camera.h"

struct Window
{
public:
    Scene scene;
    Camera camera;
    igl::opengl::glfw::Viewer viewer;

    bool mousePressed = false;

    Window(Scene scene, Camera camera) : scene(scene), camera(camera)
    {   
        viewer.data().set_mesh(scene.mesh.V, scene.mesh.F);
        viewer.data().set_face_based(true);
        initializeKeyCallbacks();
    };

    void onMousePressed(bool isShiftPressed);
    void onMouseReleased();
    bool onMouseMoved();
    void onKeyPressed(unsigned char key, bool isShiftPressed);
    bool initializeKeyCallbacks();
    void launch();
};