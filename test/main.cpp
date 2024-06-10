#include <format>
#include <functional>
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

#define STEP_RESPONSE_TEST(testcase, T, min_v, max_v) STEP_RESPONSE_TEST_(testcase, T, min_v, max_v, T)

#define STEP_RESPONSE_TEST_WITH_SUBNAME(testcase, T, min_v, max_v, SUBNAME)                                  \
  STEP_RESPONSE_TEST_(testcase, T, min_v, max_v, T##_##SUBNAME)

#define STEP_RESPONSE_TEST_(testcase, T, min_v, max_v, NAME)                                                 \
  UTEST(testcase, NAME)                                                                                      \
  {                                                                                                          \
    using testcase = testcases::testcase;                                                                    \
                                                                                                             \
    const auto output = run_test<testcase, T>((min_v), (max_v));                                             \
    ASSERT_ARRNEAR_MSG(testcase::output, output, 1e-3);                                                      \
  }

template<typename T>
concept TestCase = requires(T t) {
  { t.kp } -> std::convertible_to<double>;
  { t.ki } -> std::convertible_to<double>;
  { t.kd } -> std::convertible_to<double>;
  { t.g } -> std::convertible_to<double>;
  { t.sp } -> std::convertible_to<double>;
  { t.order_of_lag } -> std::convertible_to<int>;
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

template<TestCase TC, typename T>
auto
run_test(T min_value, T max_value)
{
  mamePID::PID<T> pid(TC::kp, TC::ki, TC::kd, TC::sp, min_value, max_value, TC::g);

  T                   acc{ 0 };
  std::function<T(T)> identity        = [](T v) -> T { return v; };
  std::function<T(T)> first_order_lag = [&acc](T v) -> T { return acc += v; };
  std::function<T(T)> obj             = TC::order_of_lag == 0 ? identity : first_order_lag;

  std::array<T, TC::output.size()> output;
  for (auto i : std::views::iota(0, static_cast<int>(TC::output.size()))) {
    output[i] = obj(pid.calculate(TC::g, get_with_default(output, i - 1, 0.0)));
  }

  return output;
}

STEP_RESPONSE_TEST(simple_p, double, -100, 100)
STEP_RESPONSE_TEST(simple_i, double, -100, 100)
STEP_RESPONSE_TEST(simple_d, double, -100, 100)

UTEST_MAIN()