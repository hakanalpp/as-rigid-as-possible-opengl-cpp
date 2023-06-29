#include "scene.h"
#include "mesh.h"

std::vector<int> Scene::getNonSelectedControlPointsIndex(const Mesh &mesh) const
{
    std::vector<int> selection_cp = std::vector<int>();
    std::vector<int> all_cp = mesh.getControlPointsIndex();
    for (const auto &i : all_cp)
    {
        bool notInSelection = true;
        for (const auto &j : selection)
            if (i == j)
            {
                notInSelection = false;
                break;
            }
        if (notInSelection)
            selection_cp.push_back(i);
    }

    return selection_cp;
}

std::vector<int> Scene::getSelectedControlPointsIndex(const Mesh &mesh, bool invert) const
{
    std::vector<int> selection_cp = std::vector<int>();
    for (const auto &i : selection)
        if (mesh.isAControlPoint(i) ^ invert)
            selection_cp.push_back(i);

    return selection_cp;
}

void Scene::displaySelectedPoints(igl::opengl::glfw::Viewer &viewer, const Mesh &mesh) const
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

    // if (moveSelectionMode)
    // {
    //     viewer.data().set_edges(cpNotSelected_edgesVert, cpNotSelected_edges, Eigen::RowVector3d(0, 0.5, 0));
    //     viewer.data().add_edges(cpSelected, cppSelected, Eigen::RowVector3d(0, 1, 0.5));
    // viewer.data().set_points(cpNotSelected, Eigen::RowVector3d(0, 0.5, 0));
    // viewer.data().add_points(cppNotSelected, Eigen::RowVector3d(0, 0.5, 0));
    //     viewer.data().add_points(cpSelected, Eigen::RowVector3d(0, 1, 0.5));
    //     viewer.data().add_points(cppSelected, Eigen::RowVector3d(0, 1, 0.5));
    //     viewer.data().add_points(notCpSelected, Eigen::RowVector3d(1, 0.5, 0));
    // }
    // else
    // {
    viewer.data().set_edges(cpNotSelected_edgesVert, cpNotSelected_edges, Eigen::RowVector3d(0, 0.5, 0));
    viewer.data().add_edges(cpSelected, cppSelected, Eigen::RowVector3d(0, 1, 0));
    viewer.data().set_points(cpNotSelected, Eigen::RowVector3d(0, 0.5, 0));
    viewer.data().add_points(cppNotSelected, Eigen::RowVector3d(0, 0.5, 0));
    viewer.data().add_points(cpSelected, Eigen::RowVector3d(0, 1, 0));
    viewer.data().add_points(cppSelected, Eigen::RowVector3d(0, 1, 0));
    viewer.data().add_points(notCpSelected, Eigen::RowVector3d(1, 0, 0));

    // if (!moveSelectionMode)
    //     return;

    // if (moveOnLine)
    // {
    //     displayMoveAxis(viewer, firstMoveDirection, cppSelected);
    // }
    // else
    // {
    //     if (firstMoveDirection.x() != 0)
    //     {
    //         displayMoveAxis(viewer, Eigen::Vector3d(0, 1, 0), cppSelected);
    //         displayMoveAxis(viewer, Eigen::Vector3d(0, 0, 1), cppSelected);
    //     }
    //     else if (firstMoveDirection.y() != 0)
    //     {
    //         displayMoveAxis(viewer, Eigen::Vector3d(1, 0, 0), cppSelected);
    //         displayMoveAxis(viewer, Eigen::Vector3d(0, 0, 1), cppSelected);
    //     }
    //     else
    //     {
    //         displayMoveAxis(viewer, Eigen::Vector3d(1, 0, 0), cppSelected);
    //         displayMoveAxis(viewer, Eigen::Vector3d(0, 1, 0), cppSelected);
    //     }
    // }
}
