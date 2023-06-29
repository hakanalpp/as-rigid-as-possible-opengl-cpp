#pragma once
#include "Mesh.h"

struct TestParam
{
	Mesh mesh;

	std::vector<int> fixedCPindex;
	std::vector<int> movedCPindex;
	std::vector<Eigen::RowVector3d> movedCPoffset;

	int res_iteration[2] = {};
	float res_time[2] = {};
	float res_initEnergy[2] = {};
	float res_finalEnergy[2] = {};

	TestParam(int cubeSubdivision, std::vector<int> _fixedCPindex, std::vector<int> _movedCPindex, std::vector<Eigen::RowVector3d> _movedCPoffset);
	TestParam(char filePath[], std::vector<int> _fixedCPindex, std::vector<int> _movedCPindex, std::vector<Eigen::RowVector3d> _movedCPoffset);

	
private:
	TestParam(std::vector<int> _fixedCPindex, std::vector<int> _movedCPindex, std::vector<Eigen::RowVector3d> _movedCPoffset);
	void ExecuteTest();
};