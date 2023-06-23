//#pragma once
#ifndef MESH_H_
#define MESH_H_
#include <string>
#include <vector>

struct Mesh
{
private:
	// std::vector<ControlPoint> C;	// Control points, private because we absolutely want to force the user to user our setter
	// Eigen::MatrixXd L;				// Laplacian Matrix, private because the arap wont access this matrix itself, but a modified version

	// void computeN();
	// void computeW();
	// void computeL();

public:
	// Eigen::MatrixXd V;				// vertices positions
	// Eigen::MatrixXi F;				// faces, defined by their vertices' indexes

	// std::vector<std::list<int>> N;	// neighboors
	// Eigen::MatrixXd W;				// weight
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	std::vector<int> faces; // Won't be used at the moment.
	std::vector<float> vertex_types; // Won't be used at the moment.

	Mesh(const char * filename);

	// Eigen::MatrixXd getVerticesFromIndex(const std::vector<int>& indexes) const;

	// const std::vector<ControlPoint>& getControlPoints() const { return C; }
	// std::vector<ControlPoint*> getControlPointsW();
	// std::vector<int> getControlPointsIndex() const;
	// Eigen::MatrixXd getControlPointsWantedPosition() const;
	// Eigen::MatrixXd getControlPointsWantedPositionBySelection(const std::vector<int>& selection, bool invert = false) const;
	// bool isAControlPoint(int vertexIndex) const;
	// ControlPoint* getControlPoint(int vertexIndex);		// be careful : changing C vector may change its memory location => would invalidate the pointer
	// int getControlPointCount() const;
	
	// void addControlPoint(int vertexIndex);
	// void addControlPoint(int vertexIndex, Eigen::RowVector3d position);
	// void removeControlPoint(int vertexIndex);

	// void printControlPoints() const;

	// void computeL_W_N();
	// Eigen::MatrixXd getL_withCP() const;
};

#endif