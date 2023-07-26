#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#include "mesh.h"

using namespace std;

struct Vertex
{
    double *coords, *new_coords, *normals, *target_coords; // 3d coordinates etc
    int idx;                                               // who am i; verts[idx]
    bool isConstraint = false;

    vector<int> vertList; // adj vvertices;
    vector<double> adjVertLengthList;
    vector<int> triList;
    vector<int> edgeList;

    Vertex(int i, double *c, double *new_c) : idx(i), coords(c), new_coords(new_c){};
};

struct Edge
{
    int idx;      // edges[idx]
    int v1i, v2i; // endpnts
    double length;
    Edge(int id, int v1, int v2) : idx(id), v1i(v1), v2i(v2){};

    void computeLength(Vertex *v1, Vertex *v2)
    {
        length = sqrt(pow(v1->coords[0] - v2->coords[0], 2) + pow(v1->coords[1] - v2->coords[1], 2) + pow(v1->coords[2] - v2->coords[2], 2));
    }
};

struct Triangle
{
    int idx; // tris[idx]
    int v1i, v2i, v3i;
    Triangle(int id, int v1, int v2, int v3) : idx(id), v1i(v1), v2i(v2), v3i(v3){};
};

class MeshArap
{

public:
    vector<Vertex *> verts;
    vector<Triangle *> tris;
    vector<Edge *> edges;

    void addTriangle(int v1, int v2, int v3);
    void addEdge(int v1, int v2);
    void addVertex(double x, double y, double z);
    bool makeVertsNeighbor(int v1i, int v2i);
    void addConstraint(int index, double x, double y, double z);

    void guiToAlgorithm(std::vector<ControlPoint> controlPoints);

    void loadOff(char *name);
    void print_off(string outfilename);

    Eigen::MatrixXd compute_weight();
    Eigen::Matrix3d compute_covariance_matrix(Eigen::MatrixXd weight, int index);
    Eigen::Matrix3d compute_rotation_matrix(Eigen::Matrix3d covariance_matrix);
    Eigen::MatrixXd compute_L(Eigen::MatrixXd weight);
    Eigen::MatrixXd compute_b(vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight);
    void mesh_vertices_update(Eigen::MatrixXd new_deformed_verts);
    double compute_energy(vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight);
    void addControlPoints();
    void ARAP(int iteration_num);
    void mesh_frame_update();
    void addControlPointsVideo2(vector<int> left_foot_indexes, vector<int> right_foot_indexes, vector<tuple<double, double>> frame_offsets, int frame_number, int is_left);
};
