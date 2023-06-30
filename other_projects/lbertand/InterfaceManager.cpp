#include "InterfaceManager.h"
#include <igl/unproject_onto_mesh.h>
#include <igl/unproject_on_line.h>
#include <igl/unproject_on_plane.h>
#include <igl/point_mesh_squared_distance.h>


std::vector<ControlPoint*> InterfaceManager::getSelectedControlPoints(Mesh& mesh) const
{
    std::vector<ControlPoint*> cp = mesh.getControlPointsW();
    std::vector<ControlPoint*> selection_cp = std::vector<ControlPoint*>();
    for (const auto& ptr : cp)
        for (const auto& i : selection)
            if (ptr->vertexIndexInMesh == i)
            {
                selection_cp.push_back(ptr);
                break;
            }

    return selection_cp;
}
std::vector<int> InterfaceManager::getSelectedControlPointsIndex(const Mesh& mesh, bool invert) const
{
    std::vector<int> selection_cp = std::vector<int>();
    for (const auto& i : selection)
        if (mesh.isAControlPoint(i) ^ invert)
            selection_cp.push_back(i);

    return selection_cp;
}
std::vector<int> InterfaceManager::getNonSelectedControlPointsIndex(const Mesh& mesh) const
{
    std::vector<int> selection_cp = std::vector<int>();
    std::vector<int> all_cp = mesh.getControlPointsIndex();
    for (const auto& i : all_cp)
    {
        bool notInSelection = true;
        for (const auto& j : selection)
            if (i == j)
            {
                notInSelection = false;
                break;
            }
        if(notInSelection)
            selection_cp.push_back(i);
    }
    
    return selection_cp;
}

void InterfaceManager::displaySelectedPoints(igl::opengl::glfw::Viewer& viewer, const Mesh& mesh) const
{
    // retrieve the control points not selected
    Eigen::MatrixXd cpNotSelected = mesh.getVerticesFromIndex(getNonSelectedControlPointsIndex(mesh));
    Eigen::MatrixXd cppNotSelected = mesh.getControlPointsWantedPositionBySelection(selection, true);
    // retrieve the control points selected
    Eigen::MatrixXd cpSelected = mesh.getVerticesFromIndex(getSelectedControlPointsIndex(mesh));
    Eigen::MatrixXd cppSelected = mesh.getControlPointsWantedPositionBySelection(selection);
    // retrieve the standard points selected
    Eigen::MatrixXd notCpSelected = mesh.getVerticesFromIndex(getSelectedControlPointsIndex(mesh, true));
    


    Eigen::MatrixXd cpNotSelected_edgesVert = Eigen::MatrixXd::Zero(cpNotSelected.rows() + cppNotSelected.rows(), 3);
    cpNotSelected_edgesVert << cpNotSelected,
        cppNotSelected;
    const int nbCpNotSelected = cpNotSelected.rows();
    Eigen::MatrixXi cpNotSelected_edges = Eigen::MatrixXi::Zero(nbCpNotSelected, 2);
    for (int i = 0; i < nbCpNotSelected; i++)
        cpNotSelected_edges.row(i) = Eigen::RowVector2i(i, i + nbCpNotSelected);

    
    if (moveSelectionMode)
    {
        viewer.data().set_edges(cpNotSelected_edgesVert, cpNotSelected_edges, Eigen::RowVector3d(0, 0.5, 0));
        viewer.data().add_edges(cpSelected, cppSelected, Eigen::RowVector3d(0, 1, 0.5));
        viewer.data().set_points(cpNotSelected, Eigen::RowVector3d(0, 0.5, 0));
        viewer.data().add_points(cppNotSelected, Eigen::RowVector3d(0, 0.5, 0));
        viewer.data().add_points(cpSelected, Eigen::RowVector3d(0, 1, 0.5));
        viewer.data().add_points(cppSelected, Eigen::RowVector3d(0, 1, 0.5));
        viewer.data().add_points(notCpSelected, Eigen::RowVector3d(1, 0.5, 0));
    }
    else
    {
        viewer.data().set_edges(cpNotSelected_edgesVert, cpNotSelected_edges, Eigen::RowVector3d(0, 0.5, 0));
        viewer.data().add_edges(cpSelected, cppSelected, Eigen::RowVector3d(0, 1, 0));
        viewer.data().set_points(cpNotSelected, Eigen::RowVector3d(0, 0.5, 0));
        viewer.data().add_points(cppNotSelected, Eigen::RowVector3d(0, 0.5, 0));
        viewer.data().add_points(cpSelected, Eigen::RowVector3d(0, 1, 0));
        viewer.data().add_points(cppSelected, Eigen::RowVector3d(0, 1, 0));
        viewer.data().add_points(notCpSelected, Eigen::RowVector3d(1, 0, 0));
    }

    if (!moveSelectionMode)
        return;

    if (moveOnLine)
    {
        displayMoveAxis(viewer, firstMoveDirection, cppSelected);
    }
    else
    {
        if (firstMoveDirection.x() != 0)
        {
            displayMoveAxis(viewer, Eigen::Vector3d(0, 1, 0), cppSelected);
            displayMoveAxis(viewer, Eigen::Vector3d(0, 0, 1), cppSelected);
        }
        else if (firstMoveDirection.y() != 0)
        {
            displayMoveAxis(viewer, Eigen::Vector3d(1, 0, 0), cppSelected);
            displayMoveAxis(viewer, Eigen::Vector3d(0, 0, 1), cppSelected);
        }
        else
        {
            displayMoveAxis(viewer, Eigen::Vector3d(1, 0, 0), cppSelected);
            displayMoveAxis(viewer, Eigen::Vector3d(0, 1, 0), cppSelected);
        }
    }
}

void InterfaceManager::displayMoveAxis(igl::opengl::glfw::Viewer& viewer, const Eigen::Vector3d& axisVector, const Eigen::MatrixXd& cppSelected) const
{
    const Eigen::RowVector3d axisTransposed = axisVector.transpose();
    Eigen::RowVector3d origin = Eigen::RowVector3d::Zero();
    for (int i = 0; i < cppSelected.rows(); i++)
        origin += cppSelected.row(i);

    Eigen::RowVector3d start = origin + axisTransposed * 5;
    Eigen::RowVector3d end = origin + axisTransposed * -5;
    viewer.data().add_edges(start, end, axisTransposed);
}

void InterfaceManager::onMousePressed(igl::opengl::glfw::Viewer& viewer, Mesh& mesh, bool isShiftPressed)
{
    mouseIsPressed = true;

    if (moveSelectionMode)
    {
        projectOnMoveDirection(viewer, lastProjectedPoint);
        return;
    }

    int fid;
    Eigen::Vector3d bc;
    // Cast a ray in the view direction starting from the mouse position
    double x = viewer.current_mouse_x;
    double y = viewer.core().viewport(3) - viewer.current_mouse_y;
    if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view,
        viewer.core().proj, viewer.core().viewport, mesh.V, mesh.F, fid, bc))
    {
        int closestVertex = 0;
        for (int i = 1; i < 3; i++)
            if (bc[i] > bc[closestVertex])
                closestVertex = i;

        if (isShiftPressed)
        {
            int selectedVertexIndex = mesh.F.row(fid)[closestVertex];
            int indexOnVectorIfExists = -1;
            for (int i = 0; i < selection.size(); i++)
                if (selection[i] == selectedVertexIndex)
                {
                    indexOnVectorIfExists = i;
                    break;
                }
            if (indexOnVectorIfExists < 0)
                selection.push_back(selectedVertexIndex);   // not in selection : add it
            else
                selection.erase(std::next(selection.begin(), indexOnVectorIfExists));   // already in the selection : remove it
        }
        else
        {
            selection.clear();
            selection.push_back(mesh.F.row(fid)[closestVertex]);
        }
    }
    else if (isShiftPressed)
        selection.clear();

    displaySelectedPoints(viewer, mesh);
}

void InterfaceManager::onMouseReleased()
{
    mouseIsPressed = false;
}

bool InterfaceManager::onMouseMoved(igl::opengl::glfw::Viewer& viewer, Mesh& mesh, bool& needArap, const EInitialisationType& initialisationType)
{
    if (!mouseIsPressed)
        return false;

    if (moveSelectionMode)
    {
        if (selection.size() == 0)
            return true;

        Eigen::Vector3d projPoint = Eigen::Vector3d();
        projectOnMoveDirection(viewer, projPoint);

        Eigen::RowVector3d mouseMovement = (lastProjectedPoint - projPoint).transpose();
        lastProjectedPoint = projPoint;
        bool canDoARAPOnMove = initialisationType == EInitialisationType::e_LastFrame;

        for (auto& cpp : getSelectedControlPoints(mesh))
        {
            cpp->wantedVertexPosition += mouseMovement;
            needArap = canDoARAPOnMove;
        }

        
        displaySelectedPoints(viewer, mesh);
        return true;
    }
    else
        return false;   // simply move the viewpoint    
}

void InterfaceManager::projectOnMoveDirection(igl::opengl::glfw::Viewer& viewer, Eigen::Vector3d& projectionReceiver) const
{
    double x = viewer.current_mouse_x;
    double y = viewer.current_mouse_y;

    // move control points that are in the selection
    if (moveOnLine)
        // Move along a line
        igl::unproject_on_line(Eigen::Vector2f(x, y), viewer.core().proj, viewer.core().viewport, Eigen::Vector3d(1, 1, 1), firstMoveDirection, projectionReceiver);
    
    else
    {
        // Mode along a plane
        Eigen::Vector4f planeEquation = Eigen::Vector4f(firstMoveDirection.x(), firstMoveDirection.y(), firstMoveDirection.z(), 3);
        igl::unproject_on_plane(Eigen::Vector2f(x, y), viewer.core().proj, viewer.core().viewport, planeEquation, projectionReceiver);
    }
}

void InterfaceManager::onKeyPressed(igl::opengl::glfw::Viewer& viewer, Mesh& mesh, unsigned char key, bool isShiftPressed, bool& needArap, EInitialisationType& initType, bool& removedCP)
{
    //std::cout << "pressed Key: " << key << " " << (unsigned int)key << std::endl;
    if (key == 'A')
    {
        needArap = true;
    }
    else if (key == '1')
    {
        std::cout << "ARAP Initialization set to : \t\tLast Frame" << std::endl;
        initType = EInitialisationType::e_LastFrame;
    }
    else if (key == '2')
    {
        std::cout << "ARAP Initialization set to : \t\tLaplace intialisation" << std::endl;
        initType = EInitialisationType::e_Laplace;
    }
    else if (key == 'G')
    {
        // Swap mode : move or select
        moveSelectionMode = !moveSelectionMode;
        displaySelectedPoints(viewer, mesh);
    }
    else if (key == 'C')
    {
        for (const auto& i : selection)
            mesh.addControlPoint(i);
        displaySelectedPoints(viewer, mesh);
    }
    else if (key == 'R')
    {
        int nbCP = mesh.getControlPointCount();
        for (const auto& i : selection)
            mesh.removeControlPoint(i);
        displaySelectedPoints(viewer, mesh);
        removedCP = nbCP != mesh.getControlPointCount();    // the number of cp is different -> one has been removed
    }
    else if (key == 'X')
        setMoveDirection(Eigen::Vector3d(1, 0, 0), isShiftPressed, viewer, mesh);
    
    else if (key == 'Y')
        setMoveDirection(Eigen::Vector3d(0, 1, 0), isShiftPressed, viewer, mesh);
 
    else if (key == 'Z')
        setMoveDirection(Eigen::Vector3d(0, 0, 1), isShiftPressed, viewer, mesh);
}

void InterfaceManager::setMoveDirection(const Eigen::Vector3d& direction, const bool& isShiftPressed, igl::opengl::glfw::Viewer& viewer, const Mesh& mesh)
{
    firstMoveDirection = direction;
    moveOnLine = !isShiftPressed;
    displaySelectedPoints(viewer, mesh);
}


void InterfaceManager::displayKeyBindOnConsole()
{
    std::cout << "\n\nAs-Rigid-As-Posible Interface usage:\n";
    std::cout << "  A,a     Starts an ARAP deformation (Key Q,q on azerty keyboards)\n";
    std::cout << "  1,&     Set the ARAP intialisation method on 'Last Frame'\n";
    std::cout << "  2,�     Set the ARAP intialisation method on 'Laplacian'\n";
    std::cout << "  G,g     Toggle interface mode between selection and Grabing\n\n";

    std::cout << "\t\t--- Selection Mode ---\n";
    std::cout << "  click   Set selection to clicked point\n";
    std::cout << "  click[shift]   Add/Remove clicked point to selection, or set selection to none if no clicked point\n";
    std::cout << "  C,c     Add selected points to mesh's control points\n";
    std::cout << "  R,r     Remove selected points to mesh's control points (Triggers ARAP with 'Laplacian' intialisation method)\n\n";

    std::cout << "\t\t--- Grab Mode ---\n";
    std::cout << "  [drag]  Move selected control points among set axis (Triggers ARAP if intialisation method is 'Last Frame')\n";
    std::cout << "  X,x     Set move axis to X\n";
    std::cout << "  Y,y     Set move axis to Y\n";
    std::cout << "  Z,z     Set move axis to Z (Key W,w on azerty keyboards)\n";
    std::cout << "  X,x[shift]     Set move plane to (Y,Z)\n";
    std::cout << "  Y,y[shift]     Set move plane to (X,Z)\n";
    std::cout << "  Z,z[shift]     Set move plane to (X,Y) (Key W,w on azerty keyboards)\n\n" << std::endl;
}