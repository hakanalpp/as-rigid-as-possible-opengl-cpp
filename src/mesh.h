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

	std::vector<ControlPoint> C; // Control points, private because we absolutely want to force the user to user our setter
	Eigen::MatrixXd V; // vertices positions
	Eigen::MatrixXi F; // faces, defined by their vertices' indexes

	std::vector<int> selectedPoints;

	void addSelectedPoint(int fid, Eigen::Vector3d bc, bool kk, bool isShiftPressed);
	void addControlPoints(SelectionMode selection_mode);

	Eigen::MatrixXd getVerticesFromIndex(const std::vector<int> &indexes) const;
	std::vector<int> getControlPoints(SelectionMode selection_mode);
	ControlPoint* getControlPoint(int vertexIndex);
	void filterControlPoints(SelectionMode selection_mode);
	void moveControlPoints(Eigen::Vector3d moveVector);

    void algorithmToGui(MeshArap meshArap);


	// void ARAP(int interation_num);
	// Eigen::MatrixXd compute_weight();
	// Eigen::MatrixXd compute_L(Eigen::MatrixXd weight);
	// Eigen::Matrix3d compute_covariance_matrix(Eigen::MatrixXd weight, int index);
	// Eigen::Matrix3d compute_rotation_matrix(Eigen::Matrix3d covariance_matrix);
	// Eigen::MatrixXd compute_b(std::vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight);
	// double compute_energy(std::vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight);
	// void mesh_vertices_update(Eigen::MatrixXd new_deformed_verts);
};