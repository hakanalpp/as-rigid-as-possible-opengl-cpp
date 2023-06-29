#pragma once
#include <igl/opengl/glfw/Viewer.h>
#include "Mesh.h"
#include "ARAPInitEnum.h"

class InterfaceManager
{
private:
	bool				mouseIsPressed = false;
	bool				moveSelectionMode = false;
	std::vector<int>	selection;
	
	Eigen::Vector3d		firstMoveDirection = Eigen::Vector3d(1, 0, 0);
	bool				moveOnLine = true;
	Eigen::Vector3d		lastProjectedPoint = Eigen::Vector3d(0, 0, 0);

private:
	std::vector<ControlPoint*> getSelectedControlPoints(Mesh& mesh) const;
	std::vector<int> getSelectedControlPointsIndex(const Mesh& mesh, bool invert = false) const;			// if invert = false, returns the control points that are in the selection ; if = true, returns the selected points that are not control points
	std::vector<int> getNonSelectedControlPointsIndex(const Mesh& mesh) const;							// returns not selected control points
	void displayMoveAxis(igl::opengl::glfw::Viewer& viewer, const Eigen::Vector3d& axisVector, const Eigen::MatrixXd& cppSelected) const;
	void projectOnMoveDirection(igl::opengl::glfw::Viewer& viewer, Eigen::Vector3d& projectionReceiver) const;
	void setMoveDirection(const Eigen::Vector3d& direction, const bool& isShiftPressed, igl::opengl::glfw::Viewer& viewer, const Mesh& mesh);

public:
	void onMousePressed(igl::opengl::glfw::Viewer& viewer, Mesh& mesh, bool isShiftPressed);
	void onMouseReleased();
	bool onMouseMoved(igl::opengl::glfw::Viewer& viewer, Mesh& mesh, bool& needArap, const EInitialisationType& initialisationType);
	void onKeyPressed(igl::opengl::glfw::Viewer& viewer, Mesh& mesh, unsigned char key, bool isShiftPressed, bool& needArap, EInitialisationType& initType, bool& removedCP);
	void displaySelectedPoints(igl::opengl::glfw::Viewer& viewer, const Mesh& mesh) const;

	static void displayKeyBindOnConsole();
};