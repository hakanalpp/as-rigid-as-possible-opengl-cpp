#include "ARAPSolver.h"


#define tol 1e-3

/* Do the Laplacian initialization
 * mesh : Initial mesh
 *
 * Out : Updated mesh
 */
MatrixXd laplacian_init(const Mesh& mesh) {
    #define V mesh.V
    #define W mesh.W

    // Get control points
    const std::vector<ControlPoint>& C = mesh.getControlPoints();

    // Init L
    MatrixXd laplacian = -W;

    // Add diagonal value
    for (int i = 0; i < laplacian.rows(); i++) {
        laplacian(i, i) += -laplacian.row(i).sum();
    }

    // We want to solve A'Ax = A'(-By + weight_ * p), A and B being a part of L, y the constraint vertices and p the initial vertices

    // Build A, B and y
    MatrixXd A = MatrixXd::Zero(V.rows(), V.rows() - C.size());
    MatrixXd B = MatrixXd::Zero(V.rows(), C.size());
    MatrixXd y = MatrixXd::Zero(C.size(), V.cols());

    int a = 0;
    int b = 0;
    for (int i = 0; i < V.rows(); i++) {
        std::pair<bool, Vector3d> constraint = isConstraint(C, i);

        // B
        if (constraint.first) {
            B.col(b) = laplacian.col(i);
            y.row(b) = constraint.second;
            b++;
        }
        // A
        else {
            A.col(a) = laplacian.col(i);
            a++;
        }
    }

    // Build A' * A
    MatrixXd left = A.transpose() * A;

    // Build A' * (-By + L * p)
    MatrixXd right = A.transpose() * (-B * y + laplacian * V);

    // Solve
    MatrixXd x = left.ldlt().solve(right);

    // Get the result matrix
    MatrixXd new_V = MatrixXd::Zero(V.rows(), V.cols());
    a = 0;
    for (int i = 0; i < V.rows(); i++) {
        std::pair<bool, Vector3d> constraint = isConstraint(C, i);

        if (constraint.first) {
            new_V.row(i) = constraint.second;
        }
        else {
            new_V.row(i) = x.row(a);
            a++;
        }
    }

    return new_V;
    #undef V
    #undef W
}

/* Compute the covariance matrix of a cell
 * W : Weight matrix
 * N : Neighbors of all the verticess
 * V : Mesh before rotation
 * new_V : Mesh after rotation
 * index : index of the vertex
 *
 * Out : Covariance matrix Si
 */
MatrixXd compute_covariance_matrix(const Eigen::MatrixXd& W, const std::vector<std::list<int>>& N, const MatrixXd& V, const MatrixXd& new_V, const int& index) {
    MatrixXd Si(V.cols(), V.cols());

    // Retrieve neighbors of v
    std::list<int> neighbors_v = N[index];

    // Init P, D and P'
    MatrixXd P_init = MatrixXd::Zero(V.cols(), neighbors_v.size());
    MatrixXd P_new = MatrixXd::Zero(V.cols(), neighbors_v.size());
    DiagonalMatrix<double, Eigen::Dynamic> D(neighbors_v.size());

    // Vertices of initial and final mesh
    Vector3d v_init = V.row(index);
    Vector3d v_new = new_V.row(index);

    // For each neighbor compute edges (in initial and result meshes)
    int k = 0;
    for (std::list<int>::iterator it = neighbors_v.begin(); it != neighbors_v.end(); ++it, ++k) {
        // Initial mesh
        Vector3d neighbor_init = V.row(*it);
        Vector3d edge_init = v_init - neighbor_init;
        P_init.col(k) = edge_init;

        // Updated mesh
        Vector3d neighbor_new = new_V.row(*it);
        Vector3d edge_new = v_new - neighbor_new;
        P_new.col(k) = edge_new;

        // Diagonal mesh
        D.diagonal()[k] = W(index, *it);
    }    

    // Compute covariance matrix
    Si = P_init * D * P_new.transpose();
    return Si;
}

/* Tell if a vertex is a constraint vertex
 * C : Constraints vertices
 * index : index of the vertex
 *
 * Out : true if constraint + the desired position 
 */
std::pair<bool, Vector3d> isConstraint(const std::vector<ControlPoint>& C, int index) {

    for (const ControlPoint& c : C) {
        if (index == c.vertexIndexInMesh) {
            return std::pair<bool, Vector3d>(true, c.wantedVertexPosition);
        }
    }
    return std::pair<bool, Vector3d>(false, Vector3d(0,0,0));
}

/* Compute the right-hand side of the equation
 * W : Weight matrix
 * N : Neighbors of all the vertices
 * V : Mesh before rotation
 * R : Rotations of each cell
 * C : Constraints vertices
 *
 * Out : Right-hand side of the equation
 */
MatrixXd compute_b(const Eigen::MatrixXd& W, const std::vector<std::list<int>>& N, const MatrixXd& V, const std::vector<MatrixXd>& R, const std::vector<ControlPoint>& C) {

    // Initialize b
    MatrixXd b = MatrixXd::Zero(V.rows(), V.cols());

    for (int i = 0; i < V.rows(); i++) {
        // Retrieve vertex
        VectorXd vi = V.row(i);

        // Retrieve neighbors of v
        std::list<int> neighbors_v = N[i];

        // Rotation matrix of i-th vertex
        MatrixXd Ri = R[i];

        // For each neighbor add the corresponding term
        // Check if the point is a constraint
        std::pair<bool, Vector3d> constraint = isConstraint(C, i);

        // If it's a constraint
        if (constraint.first) {
            b.row(i) = (VectorXd) constraint.second;
        }
        else {
            // For each neighbor
            for (std::list<int>::iterator it = neighbors_v.begin(); it != neighbors_v.end(); ++it) {
                std::pair<bool, Vector3d> constraint_n = isConstraint(C, *it);

                VectorXd neighbor = V.row(*it);
                
                // Compensate the term of the left part
                if (constraint_n.first) {
                    b.row(i) += (double)W(i, *it) * constraint_n.second;
                }

                // Weight of the edge
                double wij = W(i, *it);

                // Neighbor Rotation matrix
                MatrixXd Rj = R[*it];

                // Add the term
                b.row(i) += (double)wij / 2 * (Ri + Rj) * (vi - neighbor);

            }
        }
    }

    return b;
}

/* Compute local rigidity energy
 * W : Weight matrix
 * N : Neighbors of all vertices
 * V : Mesh before rotation
 * new_V : Mesh after rotation
 * R : Rotation for all cells
 *
 * Out : Local rigidity energy
 */
float compute_energy(const Eigen::MatrixXd& W, const std::vector<std::list<int>>& N, const MatrixXd& V, const MatrixXd& new_V, const std::vector<MatrixXd>& R) {
    // Init energy
    float energy = 0;
    for (int i = 0; i < V.rows(); i++) {
        // Retrieve vi and v'i
        VectorXd vi = V.row(i);
        VectorXd vi_prime = new_V.row(i);

        // Retrieve neighbors of vi
        std::list<int> neighbors_v = N[i];

        // Rotation matrix of i-th vertex
        MatrixXd Ri = R[i];

        // For each neigbor vertex
        for (std::list<int>::iterator it = neighbors_v.begin(); it != neighbors_v.end(); ++it) {

            // Retrieve neighbors
            VectorXd neighbor = V.row(*it);
            VectorXd neighbor_prime = new_V.row(*it);

            // Weight of the edge
            double wij = W(i, *it);

            // Add the term
            energy += (float)wij * pow(((vi_prime - neighbor_prime) - Ri * (vi - neighbor)).norm(), 2);

        }
    }
    return energy;
}


/* Apply arap algo
 * mesh : Mesh
 * init : Type of initialization (interactive or naive Laplacian)
 *
 * Out : Updated V + number of iteration and energy after the first and last iteration
 */
MatrixXd arap(const Mesh& mesh, const int& kmax, const EInitialisationType& init, int* outInterationNumber, float* outInitialEnergy, float* outFinalEnergy)
{
    #define V mesh.V
    #define N mesh.N
    #define W mesh.W

    // Get control points
    const std::vector<ControlPoint>&  C = mesh.getControlPoints();

    // Update L
    MatrixXd L = mesh.getL_withCP();

    // Init mesh before rotation
    MatrixXd previous_V = V;

    // Make an intial guess of new_V according to the initialisation type choosen:
    MatrixXd new_V;
    // User interaction
    if (init == EInitialisationType::e_LastFrame) {
        new_V = V;
        //std::cout << "Initiated with last frame" << std::endl;
    }
    // Laplacian initialization
    else if (init == EInitialisationType::e_Laplace) {
        new_V = laplacian_init(mesh);
        //std::cout << "Initiated with laplacian" << std::endl;
    }

    // Initialize energies
    float old_energy = 0;
    float new_energy = 0;


    // ITERATE
    int k = 0;
    do {

        // Find optimal Ri for each cell
        std::vector<MatrixXd> R(V.rows()); // Matrix of local rotations
        for (int i = 0; i < V.rows(); i++) {

            // Compute covariance matrix
            MatrixXd Si = compute_covariance_matrix(W, N, previous_V, new_V, i);

            JacobiSVD<MatrixXd> svd(Si, ComputeThinU | ComputeThinV);

            // Compute rotation Ri
            DiagonalMatrix<double, 3> D(1, 1, (svd.matrixV() * svd.matrixU().transpose()).determinant());
            MatrixXd Ri = svd.matrixV() * D * svd.matrixU().transpose();

            // =====    Alternative for Ri determination    =====
            /*MatrixXd svdU = svd.matrixU();
            MatrixXd svdV = svd.matrixV();
            MatrixXd Ri = svdV * svdU.transpose();    // Initial Guest
            int a;
            if (Ri.determinant() <= 0)
            {
                // Get the smallest singular value
                const VectorXd& singVals = svd.singularValues();
                double i_SmallestSingVal = 0;
                for (int i = 1; i < singVals.rows(); i++)
                    if (singVals(i) < singVals(i_SmallestSingVal))
                        i_SmallestSingVal = i;

                // change the corresponding U's col sign
                svdU.col(i_SmallestSingVal) = -1 * svdU.col(i_SmallestSingVal);

                // recompute Ri
                Ri = svdV * svdU.transpose();
            }*/

            // Store Ri
            R[i] = Ri;
        }

        // Compute right-hand side
        MatrixXd b = compute_b(W, N, previous_V, R, C);

        // Update mesh before rotation
        previous_V = new_V;

        // Find new optimal mesh
        new_V = L.ldlt().solve(b);

        // Update energies
        old_energy = new_energy;
        new_energy = compute_energy(W, N, previous_V, new_V, R);

        if (k == 0 && outInitialEnergy != nullptr)
            *outInitialEnergy = new_energy;

        k++;
    } while (k < kmax && abs(old_energy - new_energy) > tol);

    if (outInterationNumber != nullptr)
        *outInterationNumber = k;
    if (outFinalEnergy != nullptr)
        *outFinalEnergy = new_energy;

    return new_V;
    #undef V
    #undef N
    #undef W
}
