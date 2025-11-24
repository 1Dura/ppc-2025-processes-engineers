#include "ovchinnikov_m_max_values_in_matrix_rows/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

OvchinnikovMMaxValuesInMatrixRowsSEQ::OvchinnikovMMaxValuesInMatrixRowsSEQ(const InType &in) : BaseTask(in) {}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::ValidationImpl() {
  return !task_input_.empty() && !task_input_[0].empty();
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::PreProcessingImpl() {
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::RunImpl() {
  const auto &matrix = task_input_;

  size_t rows = matrix.size();
  size_t lines = matrix[0].size();

  result_.assign(lines, std::numeric_limits<int>::min());

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < lines; j++) {
      result_[j] = std::max(result_[j], matrix[i][j]);
    }
  }

  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::PostProcessingImpl() {
  task_output_ = result_;
  return true;
}

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
