import board
import pwmio
import analogio
import time

servo_out = pwmio.PWMOut(board.GP16, frequency=50, duty_cycle=0)
sensor_in = analogio.AnalogIn(board.GP26)


def set_servo_angle(angle_deg):
    duty_frac = 0.05 + (angle_deg / 180.0) * 0.05
    servo_out.duty_cycle = int(duty_frac * 65535)


def sweep_servo(start_deg, end_deg, step_deg):
    for pos in range(start_deg, end_deg, step_deg):
        set_servo_angle(pos)
        time.sleep(0.01)


while True:
    # report sensor voltage
    voltage = sensor_in.value / 65535 * 3.3
    print(f"{voltage:.6f}")

    # sweep servo back and forth
    sweep_servo(10, 170, 1)
    sweep_servo(170, 10, -1)
