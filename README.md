# mamePID

mamePID is a header-only library written in C++20 that provides easy-to-use PID controller functionality.
It also supports PI-D and I-PD style controllers, making it a versatile tool for your control systems needs.

## Features

- **Header-only**: Simply include the header file in your project, and you're ready to go.
- **Various type PID Controllers**: Implement a standard PID, PI-D and I-PD style controllers with ease.
- **C++20**: Utilizes modern C++20 features for clean and efficient code.

## Installation

Since mamePID is a header-only library, you can simply download the [`mamePID.hpp`](./src/mamePID.hpp) and include it in your project.

```cpp
#include "mamePID.hpp"
```

## Usage

### Basic PID Controller

Here's a basic example of how to use the PID controller in your project:

```cpp
#include "mamePID.h"

int main() {
    auto pid = mamePID::pid(1.0, 0.1, 0.01);
    double setpoint = 100;
    double measured_value = 90;
    double control_signal = pid.calculate(setpoint, measured_value);
    return 0;
}
```

### PI-D Controller

To use the PI-D controller, simply specify the appropriate controller type:

```cpp
#include "mamePID.h"

int main() {
    auto pi_d = mamePID::pi_d(1.0, 0.1, 0.01);
    double setpoint = 100;
    double measured_value = 90;
    double control_signal = pi_d.calculate(setpoint, measured_value);
    return 0;
}
```

### I-PD Controller

To use the I-PD controller, specify the appropriate controller type:

```cpp
#include "mamePID.h"

int main() {
    auto i_pd = mamePID::i_pd(1.0, 0.1, 0.01, mamePID::ControllerType::I_PD);
    double setpoint = 100;
    double measured_value = 90;
    double control_signal = i_pd.calculate(setpoint, measured_value);
    return 0;
}
```
## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes.