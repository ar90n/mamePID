#include <format>
#include <ranges>

#include <mamePID.hpp>

#include "testcases/simple_d.hpp"
#include "testcases/simple_i.hpp"
#include "testcases/simple_p.hpp"
#include "utest.h"

#define ASSERT_ARRNEAR_MSG(xs, ys, abs_error)                                                                \
  do {                                                                                                       \
    ASSERT_EQ(xs.size(), ys.size());                                                                         \
    for (int i : std::views::iota(0, static_cast<int>(xs.size()))) {                                         \
      ASSERT_NEAR_MSG(xs[i], ys[i], abs_error, std::format("found in {} th data", i).c_str());               \
    }                                                                                                        \
  } while (0)

template<std::ranges::range Range>
auto
get_with_default(const Range& arr, size_t i, std::ranges::range_value_t<Range> default_value)
{
  if (i < 0 || arr.size() <= i) {
    return default_value;
  }
  return arr[i];
}

template<class TestCase, typename T>
auto
run_test(T min_value, T max_value)
{
  mamePID::PID<T> pid(TestCase::kp, TestCase::ki, TestCase::kd, TestCase::sp, min_value, max_value);

  std::array<T, TestCase::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TestCase::output.size()))) {
    output[i] = pid.calculate(TestCase::g, get_with_default(output, i - 1, 0.0));
  }

  return output;
}

UTEST(simple_p, double)
{
  using testcase = testcases::simple_p;

  const auto output = run_test<testcase, double>(-100.0, 100.0);
  ASSERT_ARRNEAR_MSG(testcase::output, output, 1e-3);
}

UTEST(simple_i, double)
{
  using testcase = testcases::simple_i;

  const auto output = run_test<testcase, double>(-100.0, 100.0);
  ASSERT_ARRNEAR_MSG(testcase::output, output, 1e-3);
}

UTEST(simple_d, double)
{
  mamePID::PID<double> pid(
    testcases::simple_d::kp,
    testcases::simple_d::ki,
    testcases::simple_d::kd,
    testcases::simple_d::sp,
    -100.0,
    100.0
  );

  std::array<double, testcases::simple_d::output.size()> output;
  for (int i : std::views::iota(0, static_cast<int>(testcases::simple_d::output.size()))) {
    if (i == 0)
      output[i] = pid.calculate(testcases::simple_d::g, 0.0);
    else
      output[i] = pid.calculate(testcases::simple_d::g, output[i - 1]) + output[i - 1];
  }

  ASSERT_ARRNEAR_MSG(testcases::simple_d::output, output, 1e-3);
}

UTEST_MAIN()