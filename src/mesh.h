#pragma once

#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/upsample.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/unproject_on_plane.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>
#include <iostream>

#include "ControlPoint.h"
#include "meshArap.h"

class MeshArap;
struct Mesh
{
public:
	Mesh();
	Mesh(const std::string &filename);

	std::vector<ControlPoint> C;
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;

	std::vector<int> selectedPoints;

	void addSelectedPoint(int fid, Eigen::Vector3d bc, bool kk, bool isShiftPressed);
	void addControlPoints(SelectionMode selection_mode);

	Eigen::MatrixXd getVerticesFromIndex(const std::vector<int> &indexes) const;
	std::vector<int> getControlPoints(SelectionMode selection_mode);
	ControlPoint* getControlPoint(int vertexIndex);
	void filterControlPoints(SelectionMode selection_mode);
	void moveControlPoints(Eigen::Vector3d moveVector);

    void algorithmToGui(MeshArap meshArap);

};