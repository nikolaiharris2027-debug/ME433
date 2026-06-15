import serial
import serial.tools.list_ports
import numpy as np
import matplotlib.pyplot as plt

PORT_NAME = '/dev/cu.usbmodem2101'
BAUD = 115200
NUM_SAMPLES = 800

try:
    ser = serial.Serial(PORT_NAME, BAUD, timeout=1)
except serial.SerialException:
    print(f'Could not open {PORT_NAME}. Available ports:')
    for port in serial.tools.list_ports.comports():
        print(f'  {port.device}')
    raise SystemExit(1)

ser.reset_input_buffer()
ser.write(f'{NUM_SAMPLES}\n'.encode())

force_raw = np.zeros(NUM_SAMPLES)
force_filt = np.zeros(NUM_SAMPLES)
time_ms = np.zeros(NUM_SAMPLES)

idx = 0
while idx < NUM_SAMPLES:
    line = ser.readline().decode().strip()
    if line == '':
        continue
    raw_str, filt_str, ms_str = line.split(',')
    force_raw[idx] = float(raw_str)
    force_filt[idx] = float(filt_str)
    time_ms[idx] = float(ms_str)
    idx += 1

ser.close()

time_s = (time_ms - time_ms[0]) / 1000.0
fs = (len(time_s) - 1) / (time_s[-1] - time_s[0])
print(f'Collected {NUM_SAMPLES} samples, Fs = {fs:.2f} Hz')

freqs = np.fft.rfftfreq(len(time_s), d=1.0 / fs)
mag_raw = np.abs(np.fft.rfft(force_raw - np.mean(force_raw))) / len(force_raw)
mag_filt = np.abs(np.fft.rfft(force_filt - np.mean(force_filt))) / len(force_filt)

fig, axes = plt.subplots(2, 1, figsize=(10, 7))
fig.suptitle('HW14 - Force Sensor (HX711)')

axes[0].plot(time_s, force_raw, 'k', label='raw', alpha=0.6)
axes[0].plot(time_s, force_filt, color='brown', label=f'IIR filtered (A={0.8})')
axes[0].set_xlabel('Time (s)')
axes[0].set_ylabel('HX711 reading')
axes[0].set_title('Signal vs Time')
axes[0].legend()

axes[1].plot(freqs, mag_raw, 'k', label='raw', alpha=0.6)
axes[1].plot(freqs, mag_filt, color='brown', label='IIR filtered')
axes[1].set_xlim(0, fs / 2)
axes[1].set_xlabel('Frequency (Hz)')
axes[1].set_ylabel('FFT Magnitude')
axes[1].set_title('FFT (Nyquist ~ {:.1f} Hz)'.format(fs / 2))
axes[1].legend()

plt.tight_layout()
plt.savefig('HW14_force_sensor.png', dpi=150)
plt.show()
