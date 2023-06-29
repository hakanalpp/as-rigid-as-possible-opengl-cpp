#pragma once
#include <igl/opengl/glfw/Viewer.h>
#include "ControlPoint.h"
#include <igl/readOFF.h>
#include <igl/upsample.h>

struct Mesh
{
private:
	std::vector<ControlPoint> C;	// Control points, private because we absolutely want to force the user to user our setter
	Eigen::MatrixXd L;				// Laplacian Matrix, private because the arap wont access this matrix itself, but a modified version

	void computeN();
	void computeW();
	void computeL();

public:
	Mesh();
	Mesh(const std::string& filename);

	Eigen::MatrixXd V;				// vertices positions
	Eigen::MatrixXi F;				// faces, defined by their vertices' indexes

	std::vector<std::list<int>> N;	// neighboors
	Eigen::MatrixXd W;				// weight
	std::vector<int> selectedPoints;



	Eigen::MatrixXd getVerticesFromIndex(const std::vector<int>& indexes) const;
	std::vector<int> getControlPointsIndex() const;
	Eigen::MatrixXd getControlPointsWantedPositionBySelection(const std::vector<int>& selection, bool invert = false) const;
	bool isAControlPoint(int vertexIndex) const;
	std::vector<int> getNonSelectedControlPointsIndex() const;
	std::vector<int> getSelectedControlPointsIndex(bool invert = false) const;

	// const std::vector<ControlPoint>& getControlPoints() const { return C; }
	// std::vector<ControlPoint*> getControlPointsW();
	// Eigen::MatrixXd getControlPointsWantedPosition() const;
	// ControlPoint* getControlPoint(int vertexIndex);		// be careful : changing C vector may change its memory location => would invalidate the pointer
	// int getControlPointCount() const;
	
	// void addControlPoint(int vertexIndex);
	// void addControlPoint(int vertexIndex, Eigen::RowVector3d position);
	// void removeControlPoint(int vertexIndex);

	// void printControlPoints() const;

	// void computeL_W_N();
	// Eigen::MatrixXd getL_withCP() const;
};