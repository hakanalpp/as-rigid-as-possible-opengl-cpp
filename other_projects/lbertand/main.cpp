#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/upsample.h>
#include <igl/arap.h>
#include "ARAPSolver.h"
#include "Mesh.h"
#include "InterfaceManager.h"
#include "TestARAP.h"

//#define TEST


void performARAP(Mesh& mesh, const EInitialisationType& initialisationType, igl::opengl::glfw::Viewer& viewer, const InterfaceManager& interfaceManager)
{
    mesh.V = arap(mesh, 100, initialisationType);
    interfaceManager.displaySelectedPoints(viewer, mesh);
    viewer.data().set_mesh(mesh.V, mesh.F);
}


#ifdef  TEST

int main(int argc, char* argv[])
{
    // Test on the subdivide mesh
    // Small displacement to compare between inits
    TestParam test0 = TestParam(0,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(0.1,0.1,0.1)}
    );
    TestParam test1 = TestParam(1,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(0.1,0.1,0.1) }
    );
    TestParam test2 = TestParam(2,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(0.1,0.1,0.1) }
    );
    TestParam test3 = TestParam(3,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(0.1,0.1,0.1) }
    );
    TestParam test4 = TestParam(4,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(0.1,0.1,0.1) }
    );
    TestParam test5 = TestParam(5,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(0.1,0.1,0.1) }
    );

    // Large displacement to study speed convergence and the number of iterations
    TestParam test6 = TestParam(0,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(1.0,0.,0.) }
    );
    TestParam test7 = TestParam(1,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(1.0,0.,0.) }
    );
    TestParam test8 = TestParam(2,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(1.0,0.,0.) }
    );
    TestParam test9 = TestParam(3,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(1.0,0.,0.) }
    );
    TestParam test10 = TestParam(4,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(1.0,0.,0.) }
    );
    TestParam test11 = TestParam(5,
        { 0 },
        { 7 },
        { Eigen::RowVector3d(1.0,0.,0.) }
    );

    // Test on complex mesh
    /*TestParam test2 = TestParam("../data/cactus_small.off",
        { 0 },
        { 1 },
        { Eigen::RowVector3d(0.1,0,0) }
    );*/
}

#else


int main(int argc, char *argv[])
{
    Mesh mesh = Mesh();

    if (argc > 1)
    {
        std::cout << "Reading input file: " << argv[1] << std::endl;
        igl::readOFF(argv[1], mesh.V, mesh.F);
        std::cout << "Reading complete !" << std::endl;
    }
    else
    {
        // No external mesh input : use default subdivided cube

        // Inline mesh of a cube
        mesh.V = (Eigen::MatrixXd(8, 3) <<
            0.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 1.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 1.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, 1.0).finished();
        mesh.F = (Eigen::MatrixXi(12, 3) <<
            1, 7, 5,
            1, 3, 7,
            1, 4, 3,
            1, 2, 4,
            3, 8, 7,
            3, 4, 8,
            5, 7, 8,
            5, 8, 6,
            1, 5, 6,
            1, 6, 2,
            2, 6, 8,
            2, 8, 4).finished().array() - 1;

        int subNb = 3;
        for (int i = 0; i < subNb; i++) {
            MatrixXd V_sub(mesh.V.rows(), mesh.V.cols());
            MatrixXi F_sub(mesh.F.rows(), mesh.F.cols());
            igl::upsample(mesh.V, mesh.F, V_sub, F_sub);

            mesh.V = V_sub;
            mesh.F = F_sub;
        }

        std::cout << "Using default mesh: Cube subdivided " << subNb << " times." << std::endl;
    }

    // Center the mesh
    mesh.V = mesh.V.rowwise() - mesh.V.colwise().mean();

    std::cout << mesh.V.rows() << " vertices loaded." << std::endl;

    bool needToPerformArap = false;
    EInitialisationType initialisationType = EInitialisationType::e_LastFrame;
   

    // Initialize neighbors, weights and Laplace-Beltrami matrix
    mesh.computeL_W_N();
    const Eigen::MatrixXd V_save = mesh.V;


    // Setup the interface
    std::cout << "" << std::endl;
    igl::opengl::glfw::Viewer viewer;
    InterfaceManager interfaceManager = InterfaceManager();
    viewer.callback_pre_draw = [&interfaceManager, &mesh, &needToPerformArap, &initialisationType](igl::opengl::glfw::Viewer& viewer)->bool
    {
        if (needToPerformArap)
        {
            performARAP(mesh, initialisationType, viewer, interfaceManager);
            needToPerformArap = false;
        }

        return false;
    };
    viewer.callback_mouse_down = [&interfaceManager, &mesh](igl::opengl::glfw::Viewer& viewer, int, int modifier)->bool
    {
        interfaceManager.onMousePressed(viewer, mesh, modifier & 0x00000001);
        return false;
    };
    viewer.callback_mouse_up = [&interfaceManager](igl::opengl::glfw::Viewer& viewer, int, int)->bool
    {
        interfaceManager.onMouseReleased();
        return false;
    };
    viewer.callback_mouse_move = [&interfaceManager, &mesh, &needToPerformArap, &initialisationType](igl::opengl::glfw::Viewer& viewer, int, int modifier)->bool
    {
        return interfaceManager.onMouseMoved(viewer, mesh, needToPerformArap, initialisationType);
    };
    viewer.callback_key_down = [&interfaceManager, &mesh, &needToPerformArap, &initialisationType, &V_save](igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier)->bool
    {
        bool removedCP = false;
        interfaceManager.onKeyPressed(viewer, mesh, key, modifier & 0x00000001, needToPerformArap, initialisationType, removedCP);
        if (removedCP)
        {
            if (mesh.getControlPointCount() == 0)
            {
                mesh.V = V_save;
                interfaceManager.displaySelectedPoints(viewer, mesh);
                viewer.data().set_mesh(mesh.V, mesh.F);
            }
            else
                performARAP(mesh, EInitialisationType::e_Laplace, viewer, interfaceManager);
        }

        return false;
    };
    
    // Display Key binding for our interface
    InterfaceManager::displayKeyBindOnConsole();

    // Plot the mesh
    viewer.data().set_mesh(mesh.V, mesh.F);
    viewer.data().set_face_based(true);
    viewer.launch();
}

#endif //  TEST