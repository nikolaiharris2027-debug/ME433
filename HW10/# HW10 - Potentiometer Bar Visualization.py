import pgzrun
import serial
import serial.tools.list_ports

PORT_NAME = '/dev/cu.usbmodem2101'
BAUD = 115200

SCREEN_W = 400
SCREEN_H = 600

try:
    ser = serial.Serial(PORT_NAME, BAUD, timeout=0)
    print(f'Connected to {PORT_NAME}')
except serial.SerialException:
    print(f'Could not open {PORT_NAME}. Available ports:')
    for port in serial.tools.list_ports.comports():
        print(f'  {port.device}')
    ser = None

sensor_value = 0

bar_x = 150
bar_w = 100
top_pad = 100
bottom_pad = 40
max_bar_h = SCREEN_H - top_pad - bottom_pad


def lerp_color(start, end, t):
    t = max(0.0, min(1.0, t))
    return (
        int(start[0] + (end[0] - start[0]) * t),
        int(start[1] + (end[1] - start[1]) * t),
        int(start[2] + (end[2] - start[2]) * t),
    )


def bar_color(ratio):
    if ratio < 0.5:
        return lerp_color((50, 100, 255), (50, 220, 80), ratio * 2)
    return lerp_color((50, 220, 80), (255, 60, 60), (ratio - 0.5) * 2)


def update():
    global sensor_value

    if not ser or ser.in_waiting <= 0:
        return

    try:
        line = ser.readline().decode('utf-8').strip()
        if line:
            sensor_value = int(line)
    except (ValueError, UnicodeDecodeError):
        pass


def draw():
    screen.fill((15, 15, 25))

    ratio = max(0.0, min(1.0, sensor_value / 4095.0))
    bar_h = int(ratio * max_bar_h)
    bar_top = SCREEN_H - bottom_pad - bar_h
    color = bar_color(ratio)

    screen.draw.filled_rect(Rect(bar_x, bar_top, bar_w, bar_h), color)
    screen.draw.rect(Rect(bar_x, top_pad, bar_w, max_bar_h), (80, 80, 100))

    pct = int(ratio * 100)
    center_x = bar_x + bar_w // 2

    screen.draw.text(f'{pct}%', center=(center_x, bar_top - 20),
                     color=color, fontsize=28)
    screen.draw.text(f'{sensor_value}', center=(center_x, SCREEN_H - 20),
                     color=(160, 160, 180), fontsize=22)
    screen.draw.text('Potentiometer', center=(center_x, 30),
                     color=(200, 200, 220), fontsize=24)


pgzrun.go()
