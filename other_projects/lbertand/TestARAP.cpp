#include "TestARAP.h"

#include <igl/readOFF.h>
#include <igl/upsample.h>
#include <chrono>
#include "ARAPSolver.h"


TestParam::TestParam(std::vector<int> _fixedCPindex, std::vector<int> _movedCPindex, std::vector<Eigen::RowVector3d> _movedCPoffset) :
    mesh(),
    fixedCPindex(_fixedCPindex),
    movedCPindex(_movedCPindex),
    movedCPoffset(_movedCPoffset)
{
    static int testIndex = 0;
    testIndex++;
    std::cout << "===== Test n " << testIndex << ": =====" << std::endl;
};

TestParam::TestParam(int cubeSubdivision, std::vector<int> _fixedCPindex, std::vector<int> _movedCPindex, std::vector<Eigen::RowVector3d> _movedCPoffset) :
	TestParam(_fixedCPindex, _movedCPindex, _movedCPoffset)
{
	// Contructor with a subdivided cube mesh
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

    for (int i = 0; i < cubeSubdivision; i++) {
        Eigen::MatrixXd V_sub(mesh.V.rows(), mesh.V.cols());
        Eigen::MatrixXi F_sub(mesh.F.rows(), mesh.F.cols());
        igl::upsample(mesh.V, mesh.F, V_sub, F_sub);

        mesh.V = V_sub;
        mesh.F = F_sub;
    }

    // Complete the meshes initialization & Start the Test
    ExecuteTest();
}
TestParam::TestParam(char filePath[], std::vector<int> _fixedCPindex, std::vector<int> _movedCPindex, std::vector<Eigen::RowVector3d> _movedCPoffset) :
	TestParam(_fixedCPindex, _movedCPindex, _movedCPoffset)
{
    igl::readOFF(filePath, mesh.V, mesh.F);
    ExecuteTest();
}

void TestParam::ExecuteTest()
{
    // Complete the mesh initialization : add all needed control points
    mesh.computeL_W_N();
    for (const auto& i : fixedCPindex)
        mesh.addControlPoint(i);

    if (movedCPindex.size() != movedCPoffset.size())
    {
        std::cout << "\n\nERROR : not the same amount of values in movedCPindex & movedCPoffset\n\n" << std::endl;
        return;
    }

    for (int j = 0; j < movedCPindex.size(); j++)
    {
        const auto& i = movedCPindex[j];
        mesh.addControlPoint(i, mesh.V.row(i) + movedCPoffset[j]);
    }

    auto start = std::chrono::steady_clock::now();
    arap(mesh, 100, EInitialisationType::e_LastFrame, res_iteration, res_initEnergy, res_finalEnergy);
    auto pause = std::chrono::steady_clock::now();
    arap(mesh, 100, EInitialisationType::e_Laplace, res_iteration+1, res_initEnergy+1, res_finalEnergy+1);
    auto end = std::chrono::steady_clock::now();

    res_time[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(pause - start).count();
    res_time[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - pause).count();

    std::cout << "Iteration (kf): \t" << res_iteration[0] << "\t | " << res_iteration[1] << std::endl;
    std::cout << "Time: \t\t" << res_time[0] << "ns\t | " << res_time[1] << "ns" << std::endl;
    std::cout << "Energy(1): \t" << res_initEnergy[0] << "\t | " << res_initEnergy[1] << std::endl;
    std::cout << "Energy(kf): \t" << res_finalEnergy[0] << "\t | " << res_finalEnergy[1] << std::endl;
}