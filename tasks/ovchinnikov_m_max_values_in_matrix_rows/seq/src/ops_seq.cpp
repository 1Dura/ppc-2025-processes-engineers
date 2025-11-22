#include "ovchinnikov_m_max_values_in_matrix_rows/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

OvchinnikovMMaxValuesInMatrixRowsSEQ::OvchinnikovMMaxValuesInMatrixRowsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput();
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::ValidationImpl() {
  // return (GetInput() > 0) && (GetOutput() == 0);
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::PreProcessingImpl() {
  // GetOutput() = 2 * GetInput();
  // return GetOutput() > 0;
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::RunImpl() {
  const auto &matrix = GetInput();
  size_t rows = matrix.size();
  if (rows == 0) {
    return true;
  }
  size_t lines = matrix[0].size();

  OutType result(lines, std::numeric_limits<int>::min());
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < lines; j++) {
      result[j] = std::max(result[j], matrix[i][j]);
    }
  }
  GetOutput() = result;
  return true;
}

bool OvchinnikovMMaxValuesInMatrixRowsSEQ::PostProcessingImpl() {
  // GetOutput() -= GetInput();
  // return GetOutput() > 0;
  return true;
}

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
