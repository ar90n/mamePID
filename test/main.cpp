#include <format>
#include <ranges>

#include <mamePID.hpp>

#include "testcases/simple_d.hpp"
#include "testcases/simple_i.hpp"
#include "testcases/simple_p.hpp"
#include "utest.h"

#define ASSERT_ARRNEAR_MSG(xs, ys, abs_error)                               \
    do                                                                      \
    {                                                                       \
        ASSERT_EQ(xs.size(), ys.size());                                    \
        for (int i : std::views::iota(0, static_cast<int>(xs.size())))      \
        {                                                                   \
            ASSERT_NEAR_MSG(xs[i], ys[i], abs_error,                        \
                            std::format("found in {} th data", i).c_str()); \
        }                                                                   \
    } while (0)

UTEST(simple_p, double)
{
    mamePID::PID<double> pid(testcases::simple_p::kp, testcases::simple_p::ki,
                             testcases::simple_p::kd, testcases::simple_p::sp,
                             -100.0, 100.0);

    std::array<double, testcases::simple_p::output.size()> output;
    for (int i : std::views::iota(
             0, static_cast<int>(testcases::simple_p::output.size())))
    {
        if (i == 0)
            output[i] = pid.calculate(testcases::simple_p::g, 0.0);
        else
            output[i] = pid.calculate(testcases::simple_p::g, output[i - 1]);
    }

    ASSERT_ARRNEAR_MSG(testcases::simple_p::output, output, 1e-6);
}

UTEST(simple_i, double)
{
    mamePID::PID<double> pid(testcases::simple_i::kp, testcases::simple_i::ki,
                             testcases::simple_i::kd, testcases::simple_i::sp,
                             -100.0, 100.0);

    std::array<double, testcases::simple_i::output.size()> output;
    for (int i : std::views::iota(
             0, static_cast<int>(testcases::simple_i::output.size())))
    {
        if (i == 0)
            output[i] = pid.calculate(testcases::simple_i::g, 0.0);
        else
            output[i] = pid.calculate(testcases::simple_i::g, output[i - 1]);
    }

    ASSERT_ARRNEAR_MSG(testcases::simple_i::output, output, 1e-3);
}

UTEST(simple_d, double)
{
    mamePID::PID<double> pid(testcases::simple_d::kp, testcases::simple_d::ki,
                             testcases::simple_d::kd, testcases::simple_d::sp,
                             -100.0, 100.0);

    std::array<double, testcases::simple_d::output.size()> output;
    for (int i : std::views::iota(
             0, static_cast<int>(testcases::simple_d::output.size())))
    {
        if (i == 0)
            output[i] = pid.calculate(testcases::simple_d::g, 0.0);
        else
            output[i] =
                pid.calculate(testcases::simple_d::g, output[i - 1]) + output[i - 1];
    }

    ASSERT_ARRNEAR_MSG(testcases::simple_d::output, output, 1e-3);
}

UTEST_MAIN()