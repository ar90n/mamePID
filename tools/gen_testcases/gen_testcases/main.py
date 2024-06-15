# %%
from dataclasses import dataclass
from enum import Enum
from typing import Callable, Any

import sympy as sym
from sympy.physics.control.lti import TransferFunction, backward_diff, Parallel, Series, Feedback
from sympy.abc import s, T, z
import numpy as np
import scipy
import typer


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


def tf(num_coeffs, den_coeffs) -> TransferFunction:
    num_poly = sym.Poly(num_coeffs, s)
    den_poly = sym.Poly(den_coeffs, s)
    return TransferFunction(num_poly.as_expr(), den_poly.as_expr(), s).doit()

def pid_tf(param: Param) -> TransferFunction:
    return tf([param.kd, param.kp, param.ki], [1, 0])


def forward_d_tf(param: Param) -> TransferFunction:
    return tf([param.kp, param.ki], [param.kd, param.kp, param.ki])


def forward_pd_tf(param: Param) -> TransferFunction:
    return tf([param.ki], [param.kd, param.kp, param.ki])


def identity_tf(system: System) -> TransferFunction:
    return tf([1], [1])


def total_tf(system: System) -> TransferFunction:
    delay = TransferFunction(1, z, s).doit()
    return Series(system.feedforward, Feedback(system.feedback, delay)).doit()


def create_system(arch: Architecture, param: Param) -> System:
    feedback = pid_tf(param)
    feedforward = {
        Architecture.PID: identity_tf,
        Architecture.PI_D: forward_d_tf,
        Architecture.I_PD: forward_pd_tf,
    }[arch](param)
    return System(feedback, feedforward)

def step_response(tf: TransferFunction, g: float, sp: float, N: int) -> np.array:
    num, den = backward_diff(tf, sp)
    G = TransferFunction(sym.Poly(num, z).as_expr(), sym.Poly(den, z).as_expr(), z)
    Gdiscrete = scipy.signal.dlti([g * float(v) for v in sym.Poly(G.num).all_coeffs()], [float(v) for v in sym.Poly(G.den).all_coeffs()], dt=sp)
    _, response = Gdiscrete.step(n=N)
    return response[0].flatten()

def simulate(system: System, g: float, sp: float, n: int, minv: float, maxv: float) -> np.ndarray:
    T = total_tf(system)
    y = step_response(T, g, sp, n)
    y = np.clip(y, minv, maxv)
    return Result(y)


def dump_as_c_code(name: str, arch: Architecture, param: Param, g: float, sp: float,  result: Result):
    print(
        f"""#include<array>

namespace testcases {{
    struct {name} {{
        static constexpr const char* arch{{"{str(arch)}"}};
        static constexpr double kp{{{param.kp}}};
        static constexpr double ki{{{param.ki}}};
        static constexpr double kd{{{param.kd}}};
        static constexpr double g{{{g}}};
        static constexpr double sp{{{sp}}};
    
        static constexpr std::array<double, {result.y.size}> output{{
            {",".join(str(v) for v in result.y)}
        }};
    }};
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
    n: int = typer.Option(32, help="Number of points"),
    minv: float = typer.Option(-float('inf'), help="Minimum value"),
    maxv: float = typer.Option(float('inf'), help="Maximum value"),
    arch: Architecture = typer.Option("PID", help="System architecture"),
):
    param = Param(kp=kp, ki=ki, kd=kd)

    system = create_system(arch, param)
    result = simulate(system, g, sp, n, minv, maxv)
    dump_as_c_code(name, arch, param, g, sp, result)


if __name__ == "__main__":
    app()

# %%
