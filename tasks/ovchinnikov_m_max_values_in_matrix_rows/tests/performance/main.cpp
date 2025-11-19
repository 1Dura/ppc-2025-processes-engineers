#include <gtest/gtest.h>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"
#include "ovchinnikov_m_max_values_in_matrix_rows/mpi/include/ops_mpi.hpp"
#include "ovchinnikov_m_max_values_in_matrix_rows/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

class OvchinnikovMMaxValuesInMatrixRowsPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    rows_ = 1000;
    lines_ = 1000;

    input_data_.resize(rows_);
    for (int i = 0; i < rows_; i++) {
      input_data_[i].resize(lines_);
      for (int j = 0; j < lines_; j++) {
        input_data_[i][j] = rand() % 10000;
      }
    }
  }

  bool CheckTestOutputData(OutType& output_data) final {
    return output_data == CalcExpected(input_data_);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  int rows_;
  int lines_;
  static OutType CalcExpected(const InType& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return {};
    }

    int lines = matrix[0].size();
    OutType result(lines, std::numeric_limits<int>::min());

    for (const auto& row : matrix) {
      for (int j = 0; j < lines; j++) {
        result[j] = std::max(result[j], row[j]);
      }
    }
    return result;
  }
};

TEST_P(OvchinnikovMMaxValuesInMatrixRowsPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, OvchinnikovMMaxValuesInMatrixRowsMPI, OvchinnikovMMaxValuesInMatrixRowsSEQ>(
        PPC_SETTINGS_ovchinnikov_m_max_values_in_matrix_rows);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = OvchinnikovMMaxValuesInMatrixRowsPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, OvchinnikovMMaxValuesInMatrixRowsPerfTest, kGtestValues, kPerfTestName);

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
