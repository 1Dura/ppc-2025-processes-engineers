#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"
#include "ovchinnikov_m_max_values_in_matrix_rows/mpi/include/ops_mpi.hpp"
#include "ovchinnikov_m_max_values_in_matrix_rows/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

class OvchinnikovMMaxValuesInMatrixRowsPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
 public:
  OvchinnikovMMaxValuesInMatrixRowsPerfTest() = default;

 protected:
  void SetUp() override {
    rows_ = 2000;
    cols_ = 20000;
    data_.resize(rows_ * cols_);

    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        data_[i * cols_ + j] = i * j;
      }
    }

    input_data_ = std::make_tuple(rows_, cols_, data_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == CalcExpected(input_data_);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  std::vector<int> data_;
  int rows_ = 0;
  int cols_ = 0;

  static OutType CalcExpected(const InType &input) {
    int rows = std::get<0>(input);
    int cols = std::get<1>(input);
    const std::vector<int> &data = std::get<2>(input);

    if (rows == 0 || cols == 0) {
      return {};
    }

    OutType result(cols, std::numeric_limits<int>::min());

    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        result[j] = std::max(result[j], data[i * cols + j]);
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
