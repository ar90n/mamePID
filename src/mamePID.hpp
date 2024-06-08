#include <iostream>

namespace mamePID {
template<typename T>
class PID
{
public:
  PID(T kp, T ki, T kd, T dt, T min, T max)
    : kp(kp)
    , ki(ki)
    , kd(kd)
    , dt(dt)
    , min(min)
    , max(max)
    , pre_error(0)
    , integral(0)
  {
  }

  T calculate(T setpoint, T pv)
  {
    T error      = setpoint - pv;
    T derivative = (error - pre_error) / dt;

    T output = kp * error + ki * integral + kd * derivative;
    if (output > max)
      output = max;
    else if (output < min)
      output = min;

    integral  += error * dt;
    pre_error  = error;
    return output;
  }

private:
  T kp, ki, kd, dt, min, max;
  T pre_error, integral;
};
} // namespace mamePID
