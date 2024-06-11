#include <limits>

namespace mamePID {

template<typename T, typename U>
concept Component = requires(T t) {
  { t.calculate(U{}, U{}) } -> std::convertible_to<U>;
  requires std::is_convertible_v<typename T::value_type, U>;
};

template<typename T>
concept CoeffMutable = requires(T t) {
  { t.set } -> std::invocable<T, typename T::value_type>;
};

template<typename T>
class P
{
public:
  using value_type = T;

  P(T kp, T dt)
    : kp(kp)
  {
  }

  T calculate(T setpoint, T pv)
  {
    const T error = setpoint - pv;
    return kp * error;
  }

private:
  const T kp;
};

template<typename T>
class I
{
public:
  using value_type = T;
  I(T ki, T dt)
    : ki(ki * dt)
    , integral(0)
  {
  }

  T calculate(T setpoint, T pv)
  {
    const T error  = setpoint - pv;
    const T ret    = ki * integral;
    integral      += error;
    return ret;
  }

private:
  const T ki;
  T       integral;
};

template<typename T>
class D
{
public:
  using value_type = T;
  D(T kd, T dt, T pre_pv = 0)
    : kd(kd / dt)
    , pre_pv(pre_pv)
  {
  }

  T calculate(T setpoint, T pv)
  {
    const T derivative = pv - pre_pv;
    const T ret        = -kd * derivative;
    pre_pv             = pv;
    return ret;
  }

private:
  const T kd;
  T       pre_pv;
};

template<typename T, Component<T> PT, Component<T> IT, Component<T> DT>
class PID
{
public:
  using value_type = T;

  PID(PT p, IT i, DT d, T min = std::numeric_limits<T>::min, T max = std::numeric_limits<T>::max)
    : p(p)
    , i(i)
    , d(d)
    , min(min)
    , max(max)
  {
  }

  T calculate(T setpoint, T pv)
  {
    T output = p.calculate(setpoint, pv) + i.calculate(setpoint, pv) + d.calculate(setpoint, pv);
    return std::clamp(output, min, max);
  }

  void setKp(T kp)
    requires CoeffMutable<PT>
  {
    p.set(kp);
  }

  void setKi(T ki)
    requires CoeffMutable<IT>
  {
    i.set(ki);
  }

  void setKd(T kd)
    requires CoeffMutable<DT>
  {
    d.set(kd);
  }

private:
  PT      p;
  IT      i;
  DT      d;
  const T min;
  const T max;
};
} // namespace mamePID
