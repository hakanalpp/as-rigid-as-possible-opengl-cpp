#include "window.h"

int main(int argc, char *argv[])
{
    Mesh mesh = argc > 1 ? Mesh(argv[1]) : Mesh();

    Scene scene = Scene(mesh);

    Camera camera = Camera();

    Window window = Window(scene, camera);

    window.launch();
}
