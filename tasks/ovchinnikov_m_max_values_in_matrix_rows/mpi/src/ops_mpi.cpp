#include "ovchinnikov_m_max_values_in_matrix_rows/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

OvchinnikovMMaxValuesInMatrixRowsMPI::OvchinnikovMMaxValuesInMatrixRowsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  static_cast<void>(GetOutput());
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::ValidationImpl() {
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::PreProcessingImpl() {
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t cols = std::get<1>(GetInput());
  size_t rows = std::get<0>(GetInput());
  if (rows == 0 || cols == 0) {
    return true;
  }

  const auto &matrix = std::get<2>(GetInput());

  const int base = rows / size;
  const int extra = rows % size;
  const int my_start = (rank * base) + std::min(rank, extra);
  const int my_end = my_start + base + (rank < extra ? 1 : 0);

  std::vector<int> local_max(cols, std::numeric_limits<int>::min());

  for (int i = my_start; i < my_end; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      local_max[j] = std::max(local_max[j], matrix[(i * cols) + j]);
    }
  }

  std::vector<int> global_max(cols);
  MPI_Allreduce(local_max.data(), global_max.data(), cols, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  GetOutput() = global_max;

  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
