#include "ovchinnikov_m_max_values_in_matrix_rows/seq/include/ops_seq.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

OvchinnikovMMaxValuesInMatrixRowsSEQ::OvchinnikovMMaxValuesInMatrixRowsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  static_cast<void>(GetOutput());
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::ValidationImpl() {
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::PreProcessingImpl() {
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::RunImpl() {
  int rows = std::get<0>(GetInput());
  int cols = std::get<1>(GetInput());
  if (rows <= 0 || cols <= 0) {
    return true;
  }
  const auto &matrix = std::get<2>(GetInput());
  std::vector<int> result(cols, std::numeric_limits<int>::min());

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      result[j] = std::max(result[j], matrix[i * cols + j]);
    }
  }

  GetOutput() = result;
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
