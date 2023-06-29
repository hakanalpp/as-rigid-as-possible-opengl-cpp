#pragma once
#include <igl/opengl/glfw/Viewer.h>

struct ControlPoint
{
	int					vertexIndexInMesh;
	Eigen::RowVector3d	wantedVertexPosition;

	ControlPoint(int index, Eigen::RowVector3d position) : vertexIndexInMesh(index), wantedVertexPosition(position) {}
};