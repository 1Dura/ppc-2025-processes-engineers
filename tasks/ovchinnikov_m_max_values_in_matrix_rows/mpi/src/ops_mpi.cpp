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
  int cols = matrix[0].size();

  std::vector<int> matrix_flat;
  if (rank == 0) {
    matrix_flat.reserve(rows * cols);
    for (const auto &row : matrix) {
      matrix_flat.insert(matrix_flat.end(), row.begin(), row.end());
    }
  }
  std::vector<int> sendcounts(size);
  std::vector<int> displs(size);
  int base = rows / size;
  int extra = rows % size;
  int offset = 0;

  for (int i = 0; i < size; ++i) {
    int block_rows = base + (i < extra ? 1 : 0);
    sendcounts[i] = block_rows * cols;  // кол-во элементов
    displs[i] = offset;
    offset += sendcounts[i];
  }
  std::vector<int> local_block(sendcounts[rank]);

  MPI_Scatterv(rank == 0 ? matrix_flat.data() : nullptr, sendcounts.data(), displs.data(), MPI_INT, local_block.data(),
               sendcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
  std::vector<int> local_max(cols, std::numeric_limits<int>::min());
  int local_rows = sendcounts[rank] / cols;
  for (int i = 0; i < local_rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      local_max[j] = std::max(local_max[j], local_block[i * cols + j]);
    }
  }
  std::vector<int> global_max(cols);
  MPI_Reduce(local_max.data(), global_max.data(), cols, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    GetOutput() = global_max;
  }
  MPI_Bcast(global_max.data(), global_max.size(), MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = global_max;

  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsMPI::PostProcessingImpl() {
  // GetOutput() -= GetInput();
  // return GetOutput() > 0;
  return true;
}

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
