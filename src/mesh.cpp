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

// This is deprecated since meshArap does not work with this.
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

void Mesh::algorithmToGui(MeshArap meshArap)
{
	for (int i = 0; i < meshArap.verts.size(); i++)
	{
		Eigen::Vector3d p = Eigen::Vector3d(meshArap.verts[i]->new_coords[0], meshArap.verts[i]->new_coords[1], meshArap.verts[i]->new_coords[2]);
		V.row(i) = p;
	}
}
