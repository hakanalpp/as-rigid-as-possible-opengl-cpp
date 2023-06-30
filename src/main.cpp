#include "window.h"
#include "scene.h"
#include "meshArap.h"

int main(int argc, char *argv[])
{
    Mesh mesh = argc > 1 ? Mesh(argv[1]) : Mesh();
    MeshArap meshArap = MeshArap();

    char *filename = argv[1];
    meshArap.loadOff(filename);

    Scene scene = Scene(mesh, meshArap);

    Camera camera = Camera();

    Window window = Window(scene, camera);

    window.launch();
}
