#include <format>
#include <functional>
#include <limits>
#include <ranges>

#include <mamePID.hpp>

#include "testcases/general_pid.hpp"
#include "testcases/general_pi_d.hpp"
#include "testcases/general_i_pd.hpp"
#include "testcases/occilate_p.hpp"
#include "testcases/simple_d.hpp"
#include "testcases/simple_i.hpp"
#include "testcases/simple_i_pd.hpp"
#include "testcases/simple_p.hpp"
#include "testcases/simple_pd.hpp"
#include "testcases/simple_pi.hpp"
#include "testcases/simple_pi_d.hpp"
#include "testcases/simple_pid.hpp"
#include "utest.h"

#define ASSERT_ARRNEAR_MSG(xs, ys, abs_error)                                                                \
  do {                                                                                                       \
    ASSERT_EQ(xs.size(), ys.size());                                                                         \
    for (int i : std::views::iota(0, static_cast<int>(xs.size()))) {                                         \
      ASSERT_NEAR_MSG(xs[i], ys[i], abs_error, std::format("found in {} th data", i).c_str());               \
    }                                                                                                        \
  } while (0)

#define PID_STEP_RESPONSE_TEST(testcase, T, min_v, max_v)                                                    \
  STEP_RESPONSE_TEST_(run_pid_test, testcase, T, min_v, max_v, T)
#define PI_STEP_RESPONSE_TEST(testcase, T, min_v, max_v)                                                     \
  STEP_RESPONSE_TEST_(run_pi_test, testcase, T, min_v, max_v, T)
#define PD_STEP_RESPONSE_TEST(testcase, T, min_v, max_v)                                                     \
  STEP_RESPONSE_TEST_(run_pd_test, testcase, T, min_v, max_v, T)
#define PI_D_STEP_RESPONSE_TEST(testcase, T, min_v, max_v)                                                   \
  STEP_RESPONSE_TEST_(run_pi_d_test, testcase, T, min_v, max_v, T)
#define I_PD_STEP_RESPONSE_TEST(testcase, T, min_v, max_v)                                                   \
  STEP_RESPONSE_TEST_(run_i_pd_test, testcase, T, min_v, max_v, T)

#define PID_STEP_RESPONSE_TEST_WITH_SUBNAME(testcase, T, min_v, max_v, SUBNAME)                              \
  STEP_RESPONSE_TEST_(run_pid_test, testcase, T, min_v, max_v, T##_##SUBNAME)
#define PI_STEP_RESPONSE_TEST_WITH_SUBNAME(testcase, T, min_v, max_v, SUBNAME)                               \
  STEP_RESPONSE_TEST_(run_pi_test, testcase, T, min_v, max_v, T##_##SUBNAME)
#define PD_STEP_RESPONSE_TEST_WITH_SUBNAME(testcase, T, min_v, max_v, SUBNAME)                               \
  STEP_RESPONSE_TEST_(run_pd_test, testcase, T, min_v, max_v, T##_##SUBNAME)

#define STEP_RESPONSE_TEST_(test_func, testcase, T, min_v, max_v, NAME)                                      \
  UTEST(testcase, NAME)                                                                                      \
  {                                                                                                          \
    using testcase = testcases::testcase;                                                                    \
                                                                                                             \
    const auto output = test_func<T, testcase>((min_v), (max_v));                                            \
    ASSERT_ARRNEAR_MSG(testcase::output, output, 1e-3);                                                      \
  }

template<typename T, typename U>
concept TestCase = requires(T t) {
  { t.kp } -> std::convertible_to<U>;
  { t.ki } -> std::convertible_to<U>;
  { t.kd } -> std::convertible_to<U>;
  { t.g } -> std::convertible_to<U>;
  { t.sp } -> std::convertible_to<U>;
  { t.arch } -> std::convertible_to<const char*>;
  { t.output } -> std::ranges::range;
};

template<std::ranges::range Range>
auto
get_with_default(const Range& arr, size_t i, std::ranges::range_value_t<Range> default_value)
{
  if (i < 0 || arr.size() <= i) {
    return default_value;
  }
  return arr[i];
}

template<typename T, TestCase<T> TC>
auto
run_pid_test(T min_value, T max_value)
{
  auto pid = mamePID::pid(TC::kp, TC::ki, TC::kd, TC::sp, min_value, max_value);

  std::array<T, TC::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TC::output.size()))) {
    output[i] = pid.calculate(TC::g, get_with_default(output, i - 1, 0.0));
  }

  return output;
}

template<typename T, TestCase<T> TC>
auto
run_pi_test(T min_value, T max_value)
{
  auto pi = mamePID::pi(TC::kp, TC::ki, TC::sp, min_value, max_value);

  std::array<T, TC::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TC::output.size()))) {
    output[i] = pi.calculate(TC::g, get_with_default(output, i - 1, 0.0));
  }

  return output;
}

template<typename T, TestCase<T> TC>
auto
run_pd_test(T min_value, T max_value)
{
  auto pd = mamePID::pd(TC::kp, TC::kd, TC::sp, min_value, max_value);

  std::array<T, TC::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TC::output.size()))) {
    output[i] = pd.calculate(TC::g, get_with_default(output, i - 1, 0.0));
  }

  return output;
}

template<typename T, TestCase<T> TC>
auto
run_pi_d_test(T min_value, T max_value)
{
  auto pi_d = mamePID::pi_d(TC::kp, TC::ki, TC::kd, TC::sp, min_value, max_value);

  std::array<T, TC::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TC::output.size()))) {
    output[i] = pi_d.calculate(TC::g, get_with_default(output, i - 1, 0.0));
  }

  return output;
}

template<typename T, TestCase<T> TC>
auto
run_i_pd_test(T min_value, T max_value)
{
  auto i_pd = mamePID::i_pd(TC::kp, TC::ki, TC::kd, TC::sp, min_value, max_value);

  std::array<T, TC::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TC::output.size()))) {
    output[i] = i_pd.calculate(TC::g, get_with_default(output, i - 1, 0.0));
  }

  return output;
}

PID_STEP_RESPONSE_TEST(
  simple_p,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PID_STEP_RESPONSE_TEST(
  simple_i,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PID_STEP_RESPONSE_TEST(
  simple_d,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PI_STEP_RESPONSE_TEST(
  simple_pi,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PD_STEP_RESPONSE_TEST(
  simple_pd,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PID_STEP_RESPONSE_TEST(
  simple_pid,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PI_D_STEP_RESPONSE_TEST(
  simple_pi_d,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
I_PD_STEP_RESPONSE_TEST(
  simple_i_pd,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PID_STEP_RESPONSE_TEST(
  general_pid,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PI_D_STEP_RESPONSE_TEST(
  general_pi_d,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
I_PD_STEP_RESPONSE_TEST(
  general_i_pd,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)
PID_STEP_RESPONSE_TEST(
  occilate_p,
  double,
  std::numeric_limits<double>::lowest(),
  std::numeric_limits<double>::max()
)

UTEST_MAIN()