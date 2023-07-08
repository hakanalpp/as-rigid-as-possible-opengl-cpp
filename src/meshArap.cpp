#include <iostream>
#include <fstream>
#include <tuple>
#include <string>

#include "meshArap.h"

void MeshArap::mesh_frame_update()
{

    for (int i = 0; i < verts.size(); i++)
    {

        for (int j = 0; j < 3; j++)
        {
            // TODO
            verts[i]->coords[j] = verts[i]->new_coords[j];

            // cout << verts[i]->new_coords[j] << endl;
            // verts[i]->new_coords[j] = new_deformed_verts(i,j);
        }
    }
}

Eigen::Matrix3d MeshArap::compute_covariance_matrix(Eigen::MatrixXd weight, int index)
{
    int neighbor_size = verts[index]->vertList.size();
    vector<int> neighbor_vertlist = verts[index]->vertList;

    Eigen::MatrixXd diagonal = Eigen::MatrixXd::Zero(neighbor_size, neighbor_size);
    Eigen::MatrixXd points = Eigen::MatrixXd::Zero(3, neighbor_size);
    Eigen::MatrixXd deformed_points = Eigen::MatrixXd::Zero(3, neighbor_size);

    for (int i = 0; i < neighbor_size; i++)
    {
        int neighbor_index = neighbor_vertlist[i];
        diagonal(i, i) = weight(index, neighbor_index);

        double edge_x = verts[index]->coords[0] - verts[neighbor_index]->coords[0];
        double edge_y = verts[index]->coords[1] - verts[neighbor_index]->coords[1];
        double edge_z = verts[index]->coords[2] - verts[neighbor_index]->coords[2];

        points.block(0, i, 3, 1) = Eigen::Vector3d(edge_x, edge_y, edge_z);

        double deformed_edge_x = verts[index]->new_coords[0] - verts[neighbor_index]->new_coords[0];
        double deformed_edge_y = verts[index]->new_coords[1] - verts[neighbor_index]->new_coords[1];
        double deformed_edge_z = verts[index]->new_coords[2] - verts[neighbor_index]->new_coords[2];

        deformed_points.block(0, i, 3, 1) = Eigen::Vector3d(deformed_edge_x, deformed_edge_y, deformed_edge_z);
    }

    Eigen::Matrix3d s_matrix = points * diagonal * (deformed_points.transpose());

    return s_matrix;
}

Eigen::MatrixXd MeshArap::compute_weight()
{
    Eigen::MatrixXd weight = Eigen::MatrixXd::Zero(verts.size(), verts.size());

    for (int i = 0; i < tris.size(); i++)
    {
        // Compute edges vectors CCW

        int v1i = tris[i]->v1i;
        int v2i = tris[i]->v2i;
        int v3i = tris[i]->v3i;

        Eigen::Vector3d v1 = Eigen::Vector3d(verts[v1i]->coords[0], verts[v1i]->coords[1], verts[v1i]->coords[2]);
        Eigen::Vector3d v2 = Eigen::Vector3d(verts[v2i]->coords[0], verts[v2i]->coords[1], verts[v2i]->coords[2]);
        Eigen::Vector3d v3 = Eigen::Vector3d(verts[v3i]->coords[0], verts[v3i]->coords[1], verts[v3i]->coords[2]);

        Eigen::Vector3d e1 = v2 - v1;
        Eigen::Vector3d e2 = v3 - v2;
        Eigen::Vector3d e3 = v1 - v3;

        double norm1 = (e1.norm() * e3.norm());
        double norm2 = (e1.norm() * e2.norm());
        double norm3 = (e2.norm() * e3.norm());

        double angle1 = acos(e1.dot(-e3) / norm1);
        double angle2 = acos(-e1.dot(e2) / norm2);
        double angle3 = acos(-e2.dot(e3) / norm3);

        weight(v1i, v2i) += cos(angle3) / sin(angle3);
        weight(v2i, v1i) += cos(angle3) / sin(angle3);

        weight(v2i, v3i) += cos(angle1) / sin(angle1);
        weight(v3i, v2i) += cos(angle1) / sin(angle1);

        weight(v3i, v1i) += cos(angle2) / sin(angle2);
        weight(v1i, v3i) += cos(angle2) / sin(angle2);
    }

    weight = (1.0 / 2) * weight;

    for (int i = 0; i < verts.size(); i++)
    {

        for (int j = 0; j < verts.size(); j++)
        {

            if (weight(i, j) < 0.00001)
            {
                weight(i, j) = 0;
            }
        }
    }

    return weight;
}

void MeshArap::loadOff(char *name)
{

    FILE *fPtr = fopen(name, "r");
    char str[334];

    fscanf(fPtr, "%s", &str);

    int nVerts, nTris, n, i = 0, j = 0;
    double temp, x, y, z;

    fscanf(fPtr, "%d %d %d", &nVerts, &nTris, &n);

    while (i++ < nVerts)
    {
        fscanf(fPtr, "%lf %lf %lf", &x, &y, &z);
        addVertex(x, y, z);
    }

    while (j++ < nTris)
    {
        fscanf(fPtr, "%lf %lf %lf %lf", &temp, &x, &y, &z);
        addTriangle((int)x, (int)y, (int)z);
    }

    fclose(fPtr);
}

void MeshArap::addTriangle(int v1, int v2, int v3)
{
    int idx = tris.size();
    tris.push_back(new Triangle(idx, v1, v2, v3));

    // set up structure

    verts[v1]->triList.push_back(idx);
    verts[v2]->triList.push_back(idx);
    verts[v3]->triList.push_back(idx);

    if (!makeVertsNeighbor(v1, v2))
        addEdge(v1, v2);

    if (!makeVertsNeighbor(v1, v3))
        addEdge(v1, v3);

    if (!makeVertsNeighbor(v2, v3))
        addEdge(v2, v3);
}

bool MeshArap::makeVertsNeighbor(int v1i, int v2i)
{
    // returns true if v1i already neighbor w/ v2i; false o/w

    for (unsigned int i = 0; i < verts[v1i]->vertList.size(); i++)
        if (verts[v1i]->vertList[i] == v2i)
            return true;

    verts[v1i]->vertList.push_back(v2i);
    verts[v2i]->vertList.push_back(v1i);
    return false;
}

void MeshArap::addVertex(double x, double y, double z)
{
    int idx = verts.size();
    double *c = new double[3];
    c[0] = x;
    c[1] = y;
    c[2] = z;

    double *new_c = new double[3];
    new_c[0] = x;
    new_c[1] = y;
    new_c[2] = z;

    verts.push_back(new Vertex(idx, c, new_c));
}

void MeshArap::addConstraint(int index, double x, double y, double z)
{
    verts[index]->isConstraint = true;
    double *c = new double[3];
    c[0] = x;
    c[1] = y;
    c[2] = z;

    verts[index]->target_coords = c;
}

void MeshArap::addEdge(int v1, int v2)
{
    int idx = edges.size();
    Edge *newEdge = new Edge(idx, v1, v2);
    newEdge->computeLength(verts[v1], verts[v2]);

    edges.push_back(newEdge);

    verts[v1]->edgeList.push_back(idx);
    verts[v2]->edgeList.push_back(idx);
}

void MeshArap::guiToAlgorithm(std::vector<ControlPoint> controlPoints)
{
    for (const auto &cp : controlPoints)
    {
        verts[cp.vertexIndexInMesh]->isConstraint = true;
        double *c = new double[3];
        c[0] = cp.wantedVertexPosition[0];
        c[1] = cp.wantedVertexPosition[1];
        c[2] = cp.wantedVertexPosition[2];
        verts[cp.vertexIndexInMesh]->target_coords = c;
    }
}

Eigen::Matrix3d MeshArap::compute_rotation_matrix(Eigen::Matrix3d covariance_matrix)
{

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(covariance_matrix, Eigen::ComputeThinU | Eigen::ComputeThinV);

    Eigen::Matrix3d rotation_matrix = svd.matrixV() * (svd.matrixU()).transpose();

    if (rotation_matrix.determinant() == -1)
    {
        Eigen::Matrix3d temp_mult;
        temp_mult << 1, 0, 0,
            0, 1, 0,
            0, 0, -1;
        rotation_matrix = svd.matrixV() * temp_mult * (svd.matrixU()).transpose();
    }

    return rotation_matrix;
}

Eigen::MatrixXd MeshArap::compute_L(Eigen::MatrixXd weight)
{
    Eigen::MatrixXd L = -weight;

    for (int i = 0; i < verts.size(); i++)
    {
        double weight_sum = 0;
        for (int j = 0; j < verts.size(); j++)
        {
            weight_sum += L(i, j);
        }

        L(i, i) = -weight_sum;
    }

    return L;
}

Eigen::MatrixXd MeshArap::compute_b(vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight)
{
    Eigen::MatrixXd b = Eigen::MatrixXd::Zero(3, verts.size());

    for (int i = 0; i < verts.size(); i++)
    {

        if (verts[i]->isConstraint)
        {

            Eigen::Vector3d row_b_i = Eigen::Vector3d(verts[i]->target_coords[0],
                                                      verts[i]->target_coords[1],
                                                      verts[i]->target_coords[2]);

            b.block(0, i, 3, 1) = row_b_i;
            continue;
        }

        int neighbor_size = verts[i]->vertList.size();
        vector<int> neighbor_vertlist = verts[i]->vertList;

        Eigen::Vector3d row_b_i = Eigen::Vector3d::Zero();

        Eigen::Vector3d p_i = Eigen::Vector3d(verts[i]->coords[0], verts[i]->coords[1], verts[i]->coords[2]);

        for (auto j : neighbor_vertlist)
        {

            Eigen::Vector3d p_j = Eigen::Vector3d(verts[j]->coords[0], verts[j]->coords[1], verts[j]->coords[2]);
            row_b_i += (weight(i, j) / 2) * (rotation_matrices[i] + rotation_matrices[j]) * (p_i - p_j);

            if (verts[j]->isConstraint)
            {

                Eigen::Vector3d row_b_j = weight(i, j) * Eigen::Vector3d(verts[j]->target_coords[0],
                                                                         verts[j]->target_coords[1],
                                                                         verts[j]->target_coords[2]);

                b.block(0, i, 3, 1) += row_b_j;
            }
        }

        b.block(0, i, 3, 1) += row_b_i;
    }

    return b.transpose();
}

void MeshArap::mesh_vertices_update(Eigen::MatrixXd new_deformed_verts)
{
    for (int i = 0; i < verts.size(); i++)
    {

        for (int j = 0; j < 3; j++)
        {
            // TODO
            // verts[i]->coords[j] = verts[i]->new_coords[j];
            verts[i]->new_coords[j] = new_deformed_verts(i, j);
        }
    }
}

double MeshArap::compute_energy(vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight)
{

    double energy = 0;

    for (int i = 0; i < verts.size(); i++)
    {

        int neighbor_size = verts[i]->vertList.size();
        vector<int> neighbor_vertlist = verts[i]->vertList;

        Eigen::Vector3d p_i = Eigen::Vector3d(verts[i]->coords[0], verts[i]->coords[1], verts[i]->coords[2]);
        Eigen::Vector3d p_i_prime = Eigen::Vector3d(verts[i]->new_coords[0], verts[i]->new_coords[1], verts[i]->new_coords[2]);

        for (auto j : neighbor_vertlist)
        {
            Eigen::Vector3d p_j = Eigen::Vector3d(verts[j]->coords[0], verts[j]->coords[1], verts[j]->coords[2]);
            Eigen::Vector3d p_j_prime = Eigen::Vector3d(verts[j]->new_coords[0], verts[j]->new_coords[1], verts[j]->new_coords[2]);

            // cout << weight(i,j) <<endl;
            energy += weight(i, j) * pow(((p_i_prime - p_j_prime) - (rotation_matrices[i] * (p_i - p_j))).norm(), 2);
        }
    }

    return energy;
}

void MeshArap::ARAP(int iteration_num)
{

    Eigen::MatrixXd weight = compute_weight();

    Eigen::MatrixXd L = compute_L(weight);

    for (int i = 0; i < verts.size(); i++)
    {
        if (verts[i]->isConstraint)
        {

            for (int j = 0; j < verts.size(); j++)
            {

                L(i, j) = 0;
                L(j, i) = 0;
            }
            L(i, i) = 1;
        }
    }

    double prev_energy = 0;

    for (int iteration = 0; iteration < iteration_num; iteration++)
    {

        vector<Eigen::Matrix3d> rotation_matrices;

        for (int i = 0; i < verts.size(); i++)
        {

            Eigen::Matrix3d covariance_matrix = compute_covariance_matrix(weight, i);

            rotation_matrices.push_back(compute_rotation_matrix(covariance_matrix));
        }

        Eigen::MatrixXd b = compute_b(rotation_matrices, weight);

        Eigen::MatrixXd new_deformed_verts = L.ldlt().solve(b);

        mesh_vertices_update(new_deformed_verts);

        double energy = compute_energy(rotation_matrices, weight);

        // Comparison
        prev_energy = energy;
        cout << "energy: " << energy << endl;
    }
}

void MeshArap::print_off(string outfilename)
{
    fstream file;
    file.open(outfilename, ios::out);

    if (!file)
    {

        cout << "Error for creating output file" << endl;
        return;
    }

    file << "OFF" << endl;
    file << verts.size() << " " << tris.size() << " "
         << "0" << endl;
    for (int i = 0; i < verts.size(); i++)
    {
        file << verts[i]->new_coords[0] << " " << verts[i]->new_coords[1] << " " << verts[i]->new_coords[2] << endl;
    }

    for (int i = 0; i < tris.size(); i++)
    {
        file << "3 " << tris[i]->v1i << " " << tris[i]->v2i << " " << tris[i]->v3i << endl;
    }
}

void MeshArap::addControlPointsVideo2(vector<int> left_foot_indexes, vector<int> right_foot_indexes, vector<tuple<double, double>> frame_offsets, int frame_number, int is_left)
{
    for (int i = 0; i < left_foot_indexes.size(); i++)
    {
        int current_index = left_foot_indexes[i];

        if (is_left == 0)
        {
            addConstraint(current_index, verts[current_index]->coords[0], verts[current_index]->coords[1] + 0.4, verts[current_index]->coords[2] + 0.5);
        }
        else
        {
            addConstraint(current_index, verts[current_index]->coords[0], verts[current_index]->coords[1] + get<1>(frame_offsets[frame_number]), verts[current_index]->coords[2] - get<0>(frame_offsets[frame_number]));
        }
    }

    for (int i = 0; i < right_foot_indexes.size(); i++)
    {
        int current_index = right_foot_indexes[i];
        // addConstraint(current_index, verts[current_index]->coords[0], verts[current_index]->coords[1]+get<1>(frame_offsets[frame_number]), verts[current_index]->coords[2]-get<0>(frame_offsets[frame_number]));

        if (is_left == 0)
        {
            addConstraint(current_index, verts[current_index]->coords[0], verts[current_index]->coords[1] + 0.4, verts[current_index]->coords[2] + 0.5);
        }
        else
        {
            addConstraint(current_index, verts[current_index]->coords[0], verts[current_index]->coords[1], verts[current_index]->coords[2]);
        }

        addConstraint(398, 0.019995, 0.348865 + 1, -0.06327);
        addConstraint(746, 0.395689, 0.323604, -0.23446 - 0.3);
        // cout<<"abc:"<<get<0>(frame_offsets[frame_number])<<endl;

        // addConstraint(current_index, verts[current_index]->coords[0], verts[current_index]->coords[1], verts[current_index]->coords[2]-2);
    }
}
