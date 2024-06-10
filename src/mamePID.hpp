#include <iostream>

namespace mamePID {
template<typename T>
class PID
{
public:
  PID(T kp, T ki, T kd, T dt, T min, T max, T pre_pv = 0, T integral = 0)
    : kp(kp)
    , ki(ki * dt)
    , kd(kd / dt)
    , dt(dt)
    , min(min)
    , max(max)
    , pre_pv(pre_pv)
    , integral(integral)
  {
  }

  T calculate(T setpoint, T pv)
  {
    const T error      = setpoint - pv;
    const T derivative = pv - pre_pv;

    T output = kp * error + ki * integral - kd * derivative;
    output   = std::clamp(output, min, max);

    integral += error;
    pre_pv    = pv;
    return output;
  }

private:
  const T kp;
  const T ki;
  const T kd;
  const T dt;
  const T min;
  const T max;
  T       pre_pv;
  T       integral;
};
} // namespace mamePID
