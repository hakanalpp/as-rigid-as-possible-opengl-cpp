#include "mesh.h"

void Mesh::moveControlPoints(Eigen::Vector3d moveVector)
{
	for (auto &cp : C)
	{
		if (cp.type == DYNAMIC)
		{
			cp.wantedVertexPosition += moveVector;
		}
	}
}

ControlPoint *Mesh::getControlPoint(int vertexIndex)
{
	for (auto &cp : C)
		if (cp.vertexIndexInMesh == vertexIndex)
			return &cp;
	return nullptr;
}

void Mesh::filterControlPoints(SelectionMode selection_mode)
{
	std::vector<ControlPoint> newC;
	for (auto &cp : C)
	{
		if (cp.type != selection_mode)
		{
			newC.push_back(cp);
		}
	}
	C = newC;
}

void Mesh::addControlPoints(SelectionMode selection_mode)
{
	filterControlPoints(selection_mode);

	for (int i = 0; i < selectedPoints.size(); i++)
	{
		ControlPoint *controlPoint = getControlPoint(selectedPoints[i]);
		if (controlPoint != nullptr)
		{
			controlPoint->type = selection_mode;
			continue;
		}

		ControlPoint cp = ControlPoint(selectedPoints[i], V.row(selectedPoints[i]), selection_mode);
		C.push_back(cp);
	}
}

void Mesh::addSelectedPoint(int fid, Eigen::Vector3d bc, bool kk, bool isShiftPressed)
{
	if (kk)
	{
		int closestVertex = 0;
		for (int i = 1; i < 3; i++)
			if (bc[i] > bc[closestVertex])
				closestVertex = i;

		if (isShiftPressed)
		{
			int selectedVertexIndex = F.row(fid)[closestVertex];
			int indexOnVectorIfExists = -1;
			for (int i = 0; i < selectedPoints.size(); i++)
				if (selectedPoints[i] == selectedVertexIndex)
				{
					indexOnVectorIfExists = i;
					break;
				}
			if (indexOnVectorIfExists < 0)
				selectedPoints.push_back(selectedVertexIndex); // not in selection : add it
			else
				selectedPoints.erase(std::next(selectedPoints.begin(), indexOnVectorIfExists)); // already in the selection : remove it
		}
		else
		{
			selectedPoints.clear();
			selectedPoints.push_back(F.row(fid)[closestVertex]);
		}
	}
	else if (isShiftPressed)
		selectedPoints.clear();
}

Eigen::MatrixXd Mesh::getVerticesFromIndex(const std::vector<int> &indexes) const
{
	Eigen::MatrixXd verts = Eigen::MatrixXd::Zero(indexes.size(), 3);
	for (int i = 0; i < verts.rows(); i++)
		verts.row(i) = V.row(indexes[i]);
	return verts;
}

std::vector<int> Mesh::getControlPoints(SelectionMode selection_mode)
{
	std::vector<int> indexes = std::vector<int>();
	for (int i = 0; i < C.size(); i++)
	{
		if (C[i].type == selection_mode)
			indexes.push_back(C[i].vertexIndexInMesh);
	}
	return indexes;
}

Mesh::Mesh()
{
	V = (Eigen::MatrixXd(8, 3) << 0.0, 0.0, 0.0,
		 0.0, 0.0, 1.0,
		 0.0, 1.0, 0.0,
		 0.0, 1.0, 1.0,
		 1.0, 0.0, 0.0,
		 1.0, 0.0, 1.0,
		 1.0, 1.0, 0.0,
		 1.0, 1.0, 1.0)
			.finished();
	F = (Eigen::MatrixXi(12, 3) << 1, 7, 5,
		 1, 3, 7,
		 1, 4, 3,
		 1, 2, 4,
		 3, 8, 7,
		 3, 4, 8,
		 5, 7, 8,
		 5, 8, 6,
		 1, 5, 6,
		 1, 6, 2,
		 2, 6, 8,
		 2, 8, 4)
			.finished()
			.array() -
		1;

	int subNb = 3;
	for (int i = 0; i < subNb; i++)
	{
		Eigen::MatrixXd V_sub(V.rows(), V.cols());
		Eigen::MatrixXi F_sub(F.rows(), F.cols());
		igl::upsample(V, F, V_sub, F_sub);

		V = V_sub;
		F = F_sub;
	}

	std::cout << "Using default mesh: Cube subdivided " << subNb << " times." << std::endl;

	V = V.rowwise() - V.colwise().mean();

	std::cout << V.rows() << " vertices loaded." << std::endl;
}

Mesh::Mesh(const std::string &filename)
{
	std::cout << "Reading input file: " << filename << std::endl;
	igl::readOFF(filename, V, F);
	std::cout << "Reading complete !" << std::endl;

	V = V.rowwise() - V.colwise().mean();

	std::cout << V.rows() << " vertices loaded." << std::endl;
}

void Mesh::algorithmToGui(MeshArap meshArap) {
	for (int i = 0; i < meshArap.verts.size(); i++){
        Eigen::Vector3d p = Eigen::Vector3d(meshArap.verts[i]->new_coords[0], meshArap.verts[i]->new_coords[1], meshArap.verts[i]->new_coords[2]);
		V.row(i) = p;
	} 
}

// Onların

// void Mesh::ARAP(int iteration_num)
// {
// 	Eigen::MatrixXd weight = compute_weight();
// 	Eigen::MatrixXd L = compute_L(weight);

// 	for (int i = 0; i < C.size(); i++)
// 	{
// 		for (int j = 0; j < V.size(); j++)
// 		{

// 			L(C[i].vertexIndexInMesh, j) = 0;
// 			L(j, C[i].vertexIndexInMesh) = 0;
// 		}
// 		L(C[i].vertexIndexInMesh, C[i].vertexIndexInMesh) = 1;
// 	}

// 	double prev_energy = 0;

// 	for (int iteration = 0; iteration < iteration_num; iteration++)
// 	{
// 		std::vector<Eigen::Matrix3d> rotation_matrices;

// 		for (int i = 0; i < V.size(); i++)
// 		{
// 			Eigen::Matrix3d covariance_matrix = compute_covariance_matrix(weight, i);

// 			rotation_matrices.push_back(compute_rotation_matrix(covariance_matrix));
// 		}

// 		Eigen::MatrixXd b = compute_b(rotation_matrices, weight);

// 		Eigen::MatrixXd new_deformed_verts = L.ldlt().solve(b);

// 		mesh_vertices_update(new_deformed_verts);

// 		double energy = compute_energy(rotation_matrices, weight);

// 		// Comparison
// 		prev_energy = energy;
// 		std::cout << "energy: " << energy << std::endl;
// 	}
// }

// Eigen::MatrixXd Mesh::compute_weight()
// {
// 	Eigen::MatrixXd weight = Eigen::MatrixXd::Zero(V.size(), V.size());

// 	for (int i = 0; i < F.size(); i++)
// 	{
// 		// Compute edges vectors CCW

// 		int v1i = F.row(i)[0];
// 		int v2i = F.row(i)[1];
// 		int v3i = F.row(i)[2];

// 		Eigen::Vector3d v1 = V.row(v1i);
// 		Eigen::Vector3d v2 = V.row(v2i);
// 		Eigen::Vector3d v3 = V.row(v3i);

// 		Eigen::Vector3d e1 = v2 - v1;
// 		Eigen::Vector3d e2 = v3 - v2;
// 		Eigen::Vector3d e3 = v1 - v3;

// 		double norm1 = (e1.norm() * e3.norm());
// 		double norm2 = (e1.norm() * e2.norm());
// 		double norm3 = (e2.norm() * e3.norm());

// 		double angle1 = acos(e1.dot(-e3) / norm1);
// 		double angle2 = acos(-e1.dot(e2) / norm2);
// 		double angle3 = acos(-e2.dot(e3) / norm3);

// 		weight(v1i, v2i) += cos(angle3) / sin(angle3);
// 		weight(v2i, v1i) += cos(angle3) / sin(angle3);

// 		weight(v2i, v3i) += cos(angle1) / sin(angle1);
// 		weight(v3i, v2i) += cos(angle1) / sin(angle1);

// 		weight(v3i, v1i) += cos(angle2) / sin(angle2);
// 		weight(v1i, v3i) += cos(angle2) / sin(angle2);
// 	}

// 	weight = (1.0 / 2) * weight;

// 	return weight;
// }

// Eigen::MatrixXd Mesh::compute_L(Eigen::MatrixXd weight)
// {
// 	Eigen::MatrixXd L = -weight;

// 	for (int i = 0; i < V.size(); i++)
// 	{
// 		double weight_sum = 0;
// 		for (int j = 0; j < V.size(); j++)
// 		{
// 			weight_sum += L(i, j);
// 		}

// 		L(i, i) = -weight_sum;
// 	}

// 	return L;
// }

// Eigen::Matrix3d Mesh::compute_covariance_matrix(Eigen::MatrixXd weight, int index)
// {

// 	int neighbor_size = V[index]->vertList.size();
// 	vector<int> neighbor_vertlist = V[index]->vertList;

// 	Eigen::MatrixXd diagonal = Eigen::MatrixXd::Zero(neighbor_size, neighbor_size);
// 	Eigen::MatrixXd points = Eigen::MatrixXd::Zero(3, neighbor_size);
// 	Eigen::MatrixXd deformed_points = Eigen::MatrixXd::Zero(3, neighbor_size);

// 	for (int i = 0; i < neighbor_size; i++)
// 	{
// 		int neighbor_index = neighbor_vertlist[i];
// 		diagonal(i, i) = weight(index, neighbor_index);

// 		double edge_x = V[index]->coords[0] - V[neighbor_index]->coords[0];
// 		double edge_y = V[index]->coords[1] - V[neighbor_index]->coords[1];
// 		double edge_z = V[index]->coords[2] - V[neighbor_index]->coords[2];

// 		points.block(0, i, 3, 1) = Eigen::Vector3d(edge_x, edge_y, edge_z);

// 		double deformed_edge_x = V[index]->new_coords[0] - V[neighbor_index]->new_coords[0];
// 		double deformed_edge_y = V[index]->new_coords[1] - V[neighbor_index]->new_coords[1];
// 		double deformed_edge_z = V[index]->new_coords[2] - V[neighbor_index]->new_coords[2];

// 		deformed_points.block(0, i, 3, 1) = Eigen::Vector3d(deformed_edge_x, deformed_edge_y, deformed_edge_z);
// 	}

// 	Eigen::Matrix3d s_matrix = points * diagonal * (deformed_points.transpose());

// 	return s_matrix;
// }

// Eigen::Matrix3d Mesh::compute_rotation_matrix(Eigen::Matrix3d covariance_matrix)
// {

// 	JacobiSVD<Matrix3d> svd(covariance_matrix, ComputeThinU | ComputeThinV);

// 	Eigen::Matrix3d rotation_matrix = svd.matrixU() * (svd.matrixV()).transpose();

// 	if (rotation_matrix.determinant() == -1)
// 	{
// 		Eigen::Matrix3d temp_mult;
// 		temp_mult << 1, 0, 0,
// 			0, 1, 0,
// 			0, 0, -1;
// 		rotation_matrix = svd.matrixU() * temp_mult * (svd.matrixV()).transpose();
// 	}

// 	return rotation_matrix;
// }

// Eigen::MatrixXd Mesh::compute_b(vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight)
// {
// 	Eigen::MatrixXd b = Eigen::MatrixXd::Zero(3, V.size());

// 	for (int i = 0; i < V.size(); i++)
// 	{

// 		if (V[i]->isConstraint)
// 		{

// 			Eigen::Vector3d row_b_i = Eigen::Vector3d(V[i]->target_coords[0],
// 													  V[i]->target_coords[1],
// 													  V[i]->target_coords[2]);

// 			b.block(0, i, 3, 1) = row_b_i;
// 			continue;
// 		}

// 		int neighbor_size = V[i]->vertList.size();
// 		vector<int> neighbor_vertlist = V[i]->vertList;

// 		Eigen::Vector3d row_b_i = Eigen::Vector3d::Zero();

// 		Eigen::Vector3d p_i = Eigen::Vector3d(V[i]->coords[0], V[i]->coords[1], V[i]->coords[2]);

// 		for (auto j : neighbor_vertlist)
// 		{

// 			Eigen::Vector3d p_j = Eigen::Vector3d(V[j]->coords[0], V[j]->coords[1], V[j]->coords[2]);
// 			row_b_i += (weight(i, j) / 2) * (rotation_matrices[i] + rotation_matrices[j]) * (p_i - p_j);

// 			if (V[j]->isConstraint)
// 			{

// 				Eigen::Vector3d row_b_j = weight(i, j) * Eigen::Vector3d(V[j]->target_coords[0],
// 																		 V[j]->target_coords[1],
// 																		 V[j]->target_coords[2]);

// 				b.block(0, i, 3, 1) += row_b_j;
// 			}
// 		}

// 		b.block(0, i, 3, 1) += row_b_i;
// 	}

// 	return b.transpose();
// }

// double Mesh::compute_energy(vector<Eigen::Matrix3d> rotation_matrices, Eigen::MatrixXd weight)
// {

// 	double energy = 0;

// 	for (int i = 0; i < V.size(); i++)
// 	{

// 		int neighbor_size = V[i]->vertList.size();
// 		vector<int> neighbor_vertlist = V[i]->vertList;

// 		Eigen::Vector3d p_i = Eigen::Vector3d(V[i]->coords[0], V[i]->coords[1], V[i]->coords[2]);
// 		Eigen::Vector3d p_i_prime = Eigen::Vector3d(V[i]->new_coords[0], V[i]->new_coords[1], V[i]->new_coords[2]);

// 		for (auto j : neighbor_vertlist)
// 		{
// 			Eigen::Vector3d p_j = Eigen::Vector3d(V[j]->coords[0], V[j]->coords[1], V[j]->coords[2]);
// 			Eigen::Vector3d p_j_prime = Eigen::Vector3d(V[j]->new_coords[0], V[j]->new_coords[1], V[j]->new_coords[2]);

// 			energy += weight(i, j) * pow(((p_i_prime - p_j_prime) - (rotation_matrices[i] * (p_i - p_j))).norm(), 2);
// 		}
// 	}

// 	return energy;
// }

// void Mesh::mesh_vertices_update(Eigen::MatrixXd new_deformed_verts)
// {

// 	for (int i = 0; i < V.size(); i++)
// 	{

// 		for (int j = 0; j < 3; j++)
// 		{
// 			// TODO
// 			// V[i]->coords[j] = V[i]->new_coords[j];
// 			V[i]->new_coords[j] = new_deformed_verts(i, j);
// 		}
// 	}
// }