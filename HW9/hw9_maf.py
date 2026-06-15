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


def moving_average(sig, X):
    sig = np.array(sig)
    out = np.zeros(len(sig))
    pad_val = np.mean(sig)
    padded = np.concatenate([np.full(X - 1, pad_val), sig])
    for i in range(len(sig)):
        out[i] = np.mean(padded[i : i + X])
    return out


def fft_magnitude(sig, Fs):
    n = len(sig)
    freq = np.fft.rfftfreq(n, d=1.0/Fs)
    Y = np.abs(np.fft.rfft(sig)) / n
    return freq, Y


configs = {
    'sigA.csv': 50,
    'sigB.csv': 100,
    'sigC.csv': 5,
    'sigD.csv': 50,
}

label_font = {'family': 'fantasy', 'size': 12}

for fname, X in configs.items():
    t, sig = load_csv(fname)
    Fs = sample_rate(t)
    t = np.array(t)
    sig = np.array(sig)

    filtered = moving_average(sig, X)

    freq_raw, Y_raw = fft_magnitude(sig, Fs)
    freq_filt, Y_filt = fft_magnitude(filtered, Fs)

    fig, axes = plt.subplots(2, 1, figsize=(10, 7))
    fig.suptitle(f'{fname}  —  MAF  X = {X} points')

    axes[0].plot(t, sig,      'k',  label='raw',      alpha=0.6)
    axes[0].plot(t, filtered, color='brown', label=f'MAF X={X}')
    axes[0].set_xlabel('Elapsed Time (seconds)', fontdict=label_font)
    axes[0].set_ylabel('Signal Voltage (V)', fontdict=label_font)
    axes[0].set_title('Signal vs Time')
    axes[0].legend()

    axes[1].loglog(freq_raw[1:],  Y_raw[1:],  'k', label='raw',       alpha=0.6)
    axes[1].loglog(freq_filt[1:], Y_filt[1:], color='brown', label=f'MAF X={X}')
    axes[1].set_xlabel('Frequency (Hz)', fontdict=label_font)
    axes[1].set_ylabel('FFT Magnitude |Y(f)|', fontdict=label_font)
    axes[1].set_title('FFT — before and after MAF')
    axes[1].legend()

    plt.tight_layout()
    outname = fname.replace('.csv', '') + f'_maf_X{X}.png'
    plt.savefig(outname, dpi=150)
    plt.show()
    print(f'Saved {outname}')
