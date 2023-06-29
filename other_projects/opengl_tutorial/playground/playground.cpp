#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glut.h>

// Structure to store vertex data
struct Vertex {
    float x, y, z;
};

// Structure to store model data
struct Model {
    std::vector<Vertex> vertices;
};

// Global variables
float rotationX = 0.0f;
float rotationY = 0.0f;
float zoom = 1.0f;
int lastMouseX = 0;
int lastMouseY = 0;
Model model;

// Function to load a model from a file
bool loadModel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Vertex vertex;
        if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
            std::cout << "Error reading file: " << filename << std::endl;
            return false;
        }
        model.vertices.push_back(vertex);
    }

    return true;
}

// Function to handle mouse motion
void handleMouseMotion(int x, int y) {
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    rotationX += deltaY;
    rotationY += deltaX;
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

// Function to handle mouse click
void handleMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        lastMouseX = x;
        lastMouseY = y;
    }
}

// Function to handle keyboard input
void handleKeyboard(unsigned char key, int x, int y) {
    if (key == '+') {
        zoom += 0.1f;
    } else if (key == '-') {
        zoom -= 0.1f;
    }
    glutPostRedisplay();
}

// Function to render the scene
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 5.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glTranslatef(0.0f, 0.0f, -zoom);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

    // Render the model
    glBegin(GL_TRIANGLES);
    for (const Vertex& vertex : model.vertices) {
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Mouse Control");

    // Register callback functions
    glutDisplayFunc(renderScene);
    glutMotionFunc(handleMouseMotion);
    glutMouseFunc(handleMouseClick);
    glutMouseWheelFunc();
    glutKeyboardFunc(handleKeyboard);
    
    // Load model
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

    if (!loadModel("cube.obj")) {
        return 1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Start the main loop
    glutMainLoop();

    return 0;
}
