#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "ovchinnikov_m_max_values_in_matrix_rows/common/include/common.hpp"
#include "ovchinnikov_m_max_values_in_matrix_rows/mpi/include/ops_mpi.hpp"
#include "ovchinnikov_m_max_values_in_matrix_rows/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace ovchinnikov_m_max_values_in_matrix_rows {

class OvchinnikovMMaxValuesInMatrixRowsFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<0>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    OutType expected = CalcExpected(input_data_);
    return output_data == expected;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;

  static OutType CalcExpected(const InType &m) {
    if (m.empty() || m[0].empty()) {
      return {};
    }

    int lines = m[0].size();
    OutType result(lines, std::numeric_limits<int>::min());

    for (const auto &row : m) {
      for (int j = 0; j < lines; j++) {
        result[j] = std::max(result[j], row[j]);
      }
    }
    return result;
  }
};

namespace {

TEST_P(OvchinnikovMMaxValuesInMatrixRowsFuncTests, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 4> kTestParam = {std::make_tuple(InType{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, "matrix_3x3"),
                                            std::make_tuple(InType{{-1, -5}, {4, 0}}, "negatives"),
                                            std::make_tuple(InType{{10}}, "single_element"),
                                            std::make_tuple(InType{{1, 10, 3}, {7, 0, 100}}, "random")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<OvchinnikovMMaxValuesInMatrixRowsMPI, InType>(
                                               kTestParam, PPC_SETTINGS_ovchinnikov_m_max_values_in_matrix_rows),
                                           ppc::util::AddFuncTask<OvchinnikovMMaxValuesInMatrixRowsSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_ovchinnikov_m_max_values_in_matrix_rows));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    OvchinnikovMMaxValuesInMatrixRowsFuncTests::PrintFuncTestName<OvchinnikovMMaxValuesInMatrixRowsFuncTests>;

INSTANTIATE_TEST_SUITE_P(MaxValuesTests, OvchinnikovMMaxValuesInMatrixRowsFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace ovchinnikov_m_max_values_in_matrix_rows
