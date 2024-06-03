# %%
from dataclasses import dataclass
from enum import Enum
from typing import Callable

import numpy as np
import typer
from control.matlab import *


app = typer.Typer()

class Architecture(Enum):
    PID = "PID"
    PI_D = "PI_D"
    I_PD = "I_PD"

    def __str__(self) -> str:
        return self.value

    @classmethod
    def from_str(cls, value: str) -> "Architecture":
        return {
            "PID": cls.PID,
            "PI_D": cls.PI_D,
            "I_PD": cls.I_PD,
        }[value]


@dataclass(frozen=True)
class Param:
    kp: float
    ki: float
    kd: float


@dataclass(frozen=True)
class System:
    feedback: TransferFunction
    feedforward: TransferFunction


@dataclass(frozen=True)
class Input:
    g: float
    sp: float
    n: int


@dataclass(frozen=True)
class Result:
    y: np.ndarray
    r: np.ndarray
    Td: float


def pid_tf(param: Param) -> TransferFunction:
    return tf([param.kd, param.kp, param.ki], [1, 0])


def forward_d_tf(param: Param) -> TransferFunction:
    return tf([param.kp, param.ki], [param.kd, param.kp, param.ki])


def forward_pd_tf(param: Param) -> TransferFunction:
    return tf([param.ki], [param.kd, param.kp, param.ki])


def identity_tf(system: System) -> TransferFunction:
    return tf([1], [1])


def total_tf(system: System) -> TransferFunction:
    num_delay, den_delay = pade(0.01, 10)
    delay = 0.5 * tf(num_delay, den_delay)
    return system.feedforward * feedback(system.feedback, delay)


def step_signal(input: Input) -> tuple[np.ndarray, np.ndarray]:
    t = input.sp * np.arange(input.n)
    r = input.g * (t > 0)
    return r, t


def create_system(arch: Architecture, param: Param) -> System:
    feedback = pid_tf(param)
    feedforward = {
        Architecture.PID: identity_tf,
        Architecture.PI_D: forward_d_tf,
        Architecture.I_PD: forward_pd_tf,
    }[arch](param)
    return System(feedback, feedforward)


def simulate(system: System, input: Input) -> np.ndarray:
    T = total_tf(system)
    r, t = step_signal(input)
    y, _, _ = lsim(T, U=r, T=t)
    return Result(y, r, t)


def dump_as_c_code(name: str, arch: Architecture, param: Param, input: Input,  result: Result):
    print(
        f"""#include<array>

namespace testcases {{
    namespace {name} {{
        constexpr const char* arch{{"{str(arch)}"}};
        constexpr double kp{{{param.kp}}};
        constexpr double ki{{{param.ki}}};
        constexpr double kd{{{param.kd}}};
        constexpr double g{{{input.g}}};
        constexpr double sp{{{input.sp}}};
    
        constexpr std::array<double, {result.r.size}> input{{
            {",".join(str(v) for v in result.r)}
        }};
        constexpr std::array<double, {result.y.size}> output{{
            {",".join(str(v) for v in result.y)}
        }};
    }}
}}"""
    )

@app.command()
def main(
    name: str = typer.Argument(..., help="Name of the testcase"),
    kp: float = typer.Option(1.0, help="Proportional gain"),
    ki: float = typer.Option(0.0, help="Integral gain"),
    kd: float = typer.Option(0.0, help="Derivative gain"),
    g: float = typer.Option(1.0, help="Step gain"),
    sp: float = typer.Option(0.01, help="Sampling period"),
    n: int = typer.Option(200, help="Number of points"),
    arch: Architecture = typer.Option("PID", help="System architecture"),
):
    param = Param(kp=kp, ki=ki, kd=kd)
    input = Input(g=g, sp=sp, n=n)

    system = create_system(arch, param)
    result = simulate(system, input)
    dump_as_c_code(name, arch, param, input, result)


if __name__ == "__main__":
    app()
