module gen_testcases

using ControlSystems
using DataStructures
using Parameters
#using Typing
using JSON
using ArgParse

abstract type Architecture end
struct PID <: Architecture end
struct PI_D <: Architecture end
struct I_PD <: Architecture end

function str(::PID)::String
    return "PID"
end

function str(::PI_D)::String
    return "PI_D"
end

function str(::I_PD)::String
    return "I_PD"
end

function from_str(value::String)::Architecture
    return Dict(
        "PID" => PID(),
        "PI_D" => PI_D(),
        "I_PD" => I_PD()
    )[value]
end

@with_kw struct Param
    kp::Float64
    ki::Float64
    kd::Float64
end

@with_kw struct System
    obj::TransferFunction
    feedback::TransferFunction
    feedforward::TransferFunction
end

@with_kw struct Input
    g::Float64
    sp::Float64
    n::Int
end

@with_kw struct Result
    y::Vector{Float64}
end

function pid_tf(param::Param)::TransferFunction
    return tf([param.kd, param.kp, param.ki], [1, 0])
end

function forward_d_tf(param::Param)::TransferFunction
    return tf([param.kp, param.ki], [param.kd, param.kp, param.ki])
end

function forward_pd_tf(param::Param)::TransferFunction
    return tf([param.ki], [param.kd, param.kp, param.ki])
end

function identity_tf(param::Param)::TransferFunction
    return tf([1], [1])
end

function total_tf(system::System, sp::Float64)::DelayLtiSystem
    return system.feedforward * feedback(system.feedback * system.obj, delay(sp))
end

function create_system(arch::Architecture, order_of_lag::Int, param::Param)::System
    feedback = pid_tf(param)
    feedforward = arch isa PID ? identity_tf :
                  arch isa PI_D ? forward_d_tf :
                  arch isa I_PD ? forward_pd_tf :
                  identity_tf
    object = order_of_lag == 0 ? tf([1], [1]) : ((1 / tf("s")) ^ order_of_lag)
    return System(object, feedback, feedforward(param))
end

function simulate(system::System, input::Input)::Result
    T = total_tf(system, input.sp)
    t = input.sp * collect(0:input.n-1)

    u(x,t) = input.g
    y, _, _ = lsim(c2d(T, input.sp), u, t)
    return Result(round.(vec(y), digits=6))
end

function dump_as_c_code(name::String, arch::Architecture, order_of_lag::Int, param::Param, input::Input, result::Result)
    println("""#include<array>

namespace testcases {
    namespace $name {
        constexpr const char* arch{"$(str(arch))"};
        constexpr int order_of_lag{$order_of_lag};
        constexpr double kp{$(param.kp)};
        constexpr double ki{$(param.ki)};
        constexpr double kd{$(param.kd)};
        constexpr double g{$(input.g)};
        constexpr double sp{$(input.sp)};

        constexpr std::array<double, $(length(result.y))> output{
            $(join(map(string, result.y), ","))
        };
    }
}""")
end

function main(args)
    s = ArgParseSettings()
    @add_arg_table! s begin
        "--name"
            arg_type=String
            required=true
            help="Name of the testcase"
        "--kp"
            arg_type=Float64
            default=1.0
            help="Proportional gain"
        "--ki"
            arg_type=Float64
            default=0.0
            help="Integral gain"
        "--kd"
            arg_type=Float64
            default=0.0
            help="Derivative gain"
        "--g"
            arg_type=Float64
            default=1.0
            help="Step gain"
        "--sp"
            arg_type=Float64
            default=0.01
            help="Sampling period"
        "--ol"
            arg_type=Int
            default=0
            help="Order of Lag of object"
        "--n"
            arg_type=Int
            default=200
            help="Number of points"
        "--arch"
            arg_type=String
            default="PID"
            help="System architecture"
    end

    parsed_args = parse_args(args, s)

    param = Param(kp=parsed_args["kp"], ki=parsed_args["ki"], kd=parsed_args["kd"])
    input = Input(g=parsed_args["g"], sp=parsed_args["sp"], n=parsed_args["n"])
    arch_type = from_str(parsed_args["arch"])
    order_of_lag = parsed_args["ol"]

    system = create_system(arch_type, order_of_lag, param)
    result = simulate(system, input)
    dump_as_c_code(parsed_args["name"], arch_type, order_of_lag, param, input, result)
end


if abspath(PROGRAM_FILE) == @__FILE__
    main(ARGS)
end

end