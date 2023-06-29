#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/upsample.h>


#include "scene.h"

struct Window {
    public:
        bool mousePressed = false;

        Window() {};
        void onMousePressed(igl::opengl::glfw::Viewer& viewer, Scene& scene, Mesh& mesh, bool isShiftPressed);
        void onMouseReleased();
        bool onMouseMoved(igl::opengl::glfw::Viewer& viewer, Scene& scene, Mesh& mesh);
};