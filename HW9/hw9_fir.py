import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter


def load_csv(filename):
    t, sig = [], []
    with open(filename) as f:
        for row in csv.reader(f):
            t.append(float(row[0]))
            sig.append(float(row[1]))
    return t, sig


def sample_rate(t):
    return len(t) / t[-1]


def apply_fir(sig, Fs, cutoff_hz, numtaps, window='hamming'):
    coeffs = firwin(numtaps, cutoff_hz, fs=Fs, window=window)
    return lfilter(coeffs, 1.0, sig)


def fft_magnitude(sig, Fs):
    n = len(sig)
    freq = np.fft.rfftfreq(n, d=1.0/Fs)
    Y = np.abs(np.fft.rfft(sig)) / n
    return freq, Y


configs = {
    'sigA.csv': (100,  101, 'hamming'),
    'sigB.csv': (50,   151, 'blackman'),
    'sigC.csv': (500,  51,  'hamming'),
    'sigD.csv': (200,  101, 'hamming'),
}

label_font = {'family': 'fantasy', 'size': 12}

for fname, (cutoff, numtaps, window) in configs.items():
    t, sig = load_csv(fname)
    Fs = sample_rate(t)
    t = np.array(t)
    sig = np.array(sig)

    filtered = apply_fir(sig, Fs, cutoff, numtaps, window)

    freq_raw,  Y_raw  = fft_magnitude(sig,      Fs)
    freq_filt, Y_filt = fft_magnitude(filtered, Fs)

    title = (f'{fname}  —  FIR low-pass  '
             f'cutoff={cutoff} Hz, {numtaps} taps, {window} window')

    fig, axes = plt.subplots(2, 1, figsize=(10, 7))
    fig.suptitle(title, fontsize=9)

    axes[0].plot(t, sig,      'k', label='raw',      alpha=0.6)
    axes[0].plot(t, filtered, color='brown', label='FIR')
    axes[0].set_xlabel('Elapsed Time (seconds)', fontdict=label_font)
    axes[0].set_ylabel('Signal Voltage (V)', fontdict=label_font)
    axes[0].set_title('Signal vs Time')
    axes[0].legend()

    axes[1].loglog(freq_raw[1:],  Y_raw[1:],  'k', label='raw',  alpha=0.6)
    axes[1].loglog(freq_filt[1:], Y_filt[1:], color='brown', label='FIR')
    axes[1].axvline(cutoff, color='gray', linestyle='--', label=f'cutoff {cutoff} Hz')
    axes[1].set_xlabel('Frequency (Hz)', fontdict=label_font)
    axes[1].set_ylabel('FFT Magnitude |Y(f)|', fontdict=label_font)
    axes[1].set_title('FFT — before and after FIR')
    axes[1].legend()

    plt.tight_layout()
    outname = fname.replace('.csv', '') + f'_fir_{cutoff}hz_{numtaps}tap_{window}.png'
    plt.savefig(outname, dpi=150)
    plt.show()
    print(f'Saved {outname}')
