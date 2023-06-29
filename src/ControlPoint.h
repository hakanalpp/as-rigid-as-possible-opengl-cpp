#pragma once

struct ControlPoint
{
	int vertexIndexInMesh;
	Eigen::RowVector3d wantedVertexPosition;

	ControlPoint(int index, Eigen::RowVector3d position) : vertexIndexInMesh(index), wantedVertexPosition(position) {}
};