import csv
import numpy as np
import matplotlib.pyplot as plt


def load_csv(filename):
    t, sig = [], []
    with open(filename) as f:
        for row in csv.reader(f):
            t.append(float(row[0]))
            sig.append(float(row[1]))
    return t, sig


def sample_rate(t):
    return len(t) / t[-1]


def plot_signal_and_fft(t, sig, title, Fs):
    t = np.array(t)
    sig = np.array(sig)
    n = len(sig)

    freq = np.fft.rfftfreq(n, d=1.0/Fs)
    Y = np.abs(np.fft.rfft(sig)) / n

    label_font = {'family': 'fantasy', 'size': 12}

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 7))
    fig.suptitle(title)

    ax1.plot(t, sig, color='brown')
    ax1.set_xlabel('Elapsed Time (seconds)', fontdict=label_font)
    ax1.set_ylabel('Signal Voltage (V)', fontdict=label_font)
    ax1.set_title('Signal vs Time')

    ax2.loglog(freq[1:], Y[1:], color='brown')
    ax2.set_xlabel('Frequency (Hz)', fontdict=label_font)
    ax2.set_ylabel('FFT Magnitude |Y(f)|', fontdict=label_font)
    ax2.set_title('FFT Magnitude Spectrum')

    plt.tight_layout()
    plt.savefig(title.replace(' ', '_') + '_fft.png', dpi=150)
    plt.show()


files = ['sigA.csv', 'sigB.csv', 'sigC.csv', 'sigD.csv']

for fname in files:
    t, sig = load_csv(fname)
    Fs = sample_rate(t)
    print(f'{fname}: {len(t)} samples, Fs = {Fs:.1f} Hz, duration = {t[-1]:.4f} s')
    plot_signal_and_fft(t, sig, fname.replace('.csv', ''), Fs)
