#include "ovchinnikov_m_max_values_in_matrix_rows/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

OvchinnikovMMaxValuesInMatrixRowsMPI::OvchinnikovMMaxValuesInMatrixRowsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput();
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::ValidationImpl() {
  // return (GetInput() > 0) && (GetOutput() == 0);
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::PreProcessingImpl() {
  // GetOutput() = 2 * GetInput();
  // return GetOutput() > 0;
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::RunImpl() {
  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &matrix = GetInput();
  int rows = matrix.size();
  if (rows == 0) {
    return true;
  }
  int cols = matrix[0].size();

  std::vector<int> matrix_flat;
  int base = rows / size;
  int extra = rows % size;
  int my_start = rank * base + std::min(rank, extra);
  int my_end = my_start + base + (rank < extra ? 1 : 0);
  std::vector<int> local_max(cols, std::numeric_limits<int>::min());
  for (int i = my_start; i < my_end; ++i) {
    for (int j = 0; j < cols; ++j) {
      local_max[j] = std::max(local_max[j], matrix[i][j]);
    }
  }
  std::vector<int> global_max(cols);
  MPI_Allreduce(local_max.data(), global_max.data(), cols, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  // MPI_Bcast(global_max.data(), global_max.size(), MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = global_max;

  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::PostProcessingImpl() {
  // GetOutput() -= GetInput();
  // return GetOutput() > 0;
  return true;
}

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
