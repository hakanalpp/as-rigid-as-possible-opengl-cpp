#pragma once
#include <unordered_map>
#include <string>

enum SelectionMode
{
	NONE,
	STATIC,
	DYNAMIC
};

extern std::unordered_map<SelectionMode, std::string> SelectionModeString;

struct ControlPoint
{
	int vertexIndexInMesh;
	Eigen::RowVector3d wantedVertexPosition;
	SelectionMode type = NONE;

	ControlPoint(int index, Eigen::RowVector3d position, SelectionMode type = NONE) : vertexIndexInMesh(index), wantedVertexPosition(position), type(type) {}
};