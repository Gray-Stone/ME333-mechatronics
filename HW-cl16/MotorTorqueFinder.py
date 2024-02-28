import math


def output_torque_from_speed(motor_stall_torque: float, motor_noload_speed: float,
                             gear_ratio: float, gear_efficiency: float,
                             output_target_speed: float) -> float:
    # First find the motor speed.
    motor_speed = output_target_speed * gear_ratio
    if motor_speed > motor_noload_speed:
        print(f"Motor speed {motor_speed} > free running speed {motor_noload_speed}")
        return 0
    motor_torque = motor_stall_torque * (1 - motor_speed / motor_noload_speed)
    return motor_torque * gear_ratio * gear_efficiency

if __name__ == "__main__":

    # Motor in sequence of 3 10 20 90
    # All unit in Meter, N
    motor_name = ["3", "10", "20", "90"]
    motor_stall_torques = [0.01 , 0.131 , 0.225 , 0.929]
    motor_noload_speeds = [13400 , 4980 , 9660 , 7180]

    # Gear info
    gear_ratio = [1,10,20,50,100]
    gear_efficiency = [1,0.75,0.75,0.75,0.75]

    output_speed = 47.97
    required_torque = 0.17406

    print(f"| motor name | gear ratio | output_torque| meets requirement? | ")
    print(f"| --- |--- |---| --- |  ")
    for name , t , free_speed in zip(motor_name,motor_stall_torques,motor_noload_speeds):
        for ratio,eff in zip(gear_ratio , gear_efficiency):
            out_torque = output_torque_from_speed(t,free_speed,ratio,eff,output_speed)
            print(f"| motor {name} | {ratio} | {out_torque} | {out_torque> required_torque} |")
