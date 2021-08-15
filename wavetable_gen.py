#!/usr/bin/env python
import math as m
import sys

import matplotlib.pyplot as plt
import numpy as np
from numpy.fft import irfft


def gen_saw_spectrum(length, amp=1.0):
    tab = [(-1) ** i / i for i in range(1, length - 1)]
    tab = np.concatenate(([0], tab))
    # multiply by j to change from sin to cos
    return amp * tab.astype(complex) * (0 + 1j)


def gen_square_spectrum(length, amp=1.0):
    tab = np.array(length * [0.0])
    for i in range(1, length, 2):
        tab[i] = 1 / i
    return amp * tab.astype(complex) * (0 - 1j)


def gen_triangle_spectrum(length, amp=1.0):  # TODO: this doesn't work yet
    tab = np.array(length * [0.0])
    for i in range(1, length // 2):
        n = 2 * i + 1
        tab[2 * i - 1] = ((-1) ** i) / (n ** 2)
    return amp * tab.astype(complex) * (0 + 1j)


def normalize(table, max_val=1.0):
    return table * (max_val / table.max())


def write_wavetables(tables, name, f_lowest, f_highest):
    with open(name, 'w') as f:
        for i, table in enumerate(tables):
            f_low = f_lowest * 2 ** i
            f_high = f_low * 2 if (f_low * 2 < f_highest) else f_highest
            f.write(str(f_low) + '\n')
            f.write(str(f_high) + '\n')
            f.write(str(len(table)) + '\n')
            f.writelines('\n'.join([str(i) for i in table]))
            f.write('\n')


def main():
    name = sys.argv[1]
    if name == 'saw':
        spectrum = gen_saw_spectrum(2048)
    elif name == 'square':
        spectrum = gen_square_spectrum(2048)
    elif name == 'triangle':
        spectrum = gen_triangle_spectrum(2048)
    else:
        spectrum = np.genfromtxt(name, dtype=complex)
    f_low = int(sys.argv[2])
    f_high = int(sys.argv[3])
    amp = 0.8
    octaves = m.ceil(m.log2(f_high / f_low))
    fig, axs = plt.subplots(m.ceil(octaves / 2), 2, sharex='all', sharey='all')
    fig.suptitle('Wavetables for frequency range ' + str(f_low) + "Hz - " + str(f_high) + "Hz")
    table = octaves * [None]
    for octave in range(octaves):
        n = m.floor(14000 / (f_low * 2 ** octave))
        s = spectrum
        s[n + 1:] = 0
        print("harmonics for " + str(octave) + " octave: " + str(n))
        table[octave] = normalize(irfft(s, 2048) * amp)

        axs[octave // 2, octave % 2].set_title('octave ' + str(octave))
        axs[octave // 2, octave % 2].plot(table[octave])

    plt.show()

    write_wavetables(table, name + '.synthwave', f_low, f_high)


if __name__ == '__main__':
    main()
