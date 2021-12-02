#!/usr/bin/env python
"""Tools for generating wavetables
"""
import math as m
import sys

import matplotlib.pyplot as plt
import numpy as np
from numpy.fft import irfft


def gen_saw_spectrum(length: int, amp=1.0) -> np.ndarray:
    """Generate spectrum of real sawtooth wave signal.

    :param length: length of the resulting spectrum; half of window size
    :param amp: amplitude of the spectrum
    :return: a complex array containing spectrum values
    """
    tab = np.array([(-1) ** i / i for i in range(1, length - 1)])
    tab = np.concatenate(([0], tab))
    # multiply by j to change from sin to cos
    return amp * tab.astype(complex) * (0 + 1j)


def gen_square_spectrum(length: int, amp=1.0) -> np.ndarray:
    """Generate spectrum of real square wave signal.

    :param length: length of the resulting spectrum; half of window size
    :param amp: amplitude of the spectrum
    :return: a complex array containing spectrum values
    """
    tab = np.array(length * [0.0])
    for i in range(1, length, 2):
        tab[i] = 1 / i
    return amp * tab.astype(complex) * (0 - 1j)


def gen_triangle_spectrum(length: int, amp=1.0) -> np.ndarray:  # TODO: this doesn't work yet
    """Generate spectrum of real triangle wave signal.

    :param length: length of the resulting spectrum; half of window size
    :param amp: amplitude of the spectrum
    :return: a complex array containing spectrum values
    """
    tab = np.array(length * [0.0])
    for i in range(1, length // 2):
        n = 2 * i - 1
        tab[n] = ((-1) ** i) / (n ** 2)
    return amp * tab.astype(complex) * (0 + 1j)


def normalize(table: np.ndarray, max_val=1.0) -> np.ndarray:
    """Normalize contents input array to given max value.

    :param table: input array
    :param max_val: maximum array value
    :return: normalized array
    """
    return table * (max_val / np.abs(table).max(0))


def write_wavetables(tables: list, name: str, f_lowest: float, f_highest: float) -> None:
    """Write wavetables to the output file.

    :param tables: list of ndarrays containing wavetables
    :param name: output file name
    :param f_lowest: lowest frequency represented by the wavetables
    :param f_highest: highest frequency represented by the wavetables
    """
    with open(name, 'w', encoding='utf-8') as file:
        for i, table in enumerate(tables):
            f_low = f_lowest * 2 ** i
            f_high = f_low * 2 if (f_low * 2 < f_highest) else f_highest
            file.write(str(f_low) + '\n')
            file.write(str(f_high) + '\n')
            file.write(str(len(table)) + '\n')
            file.writelines('\n'.join([str(i) for i in table]))
            file.write('\n')


def main():
    """Generate wavetables

    This function can generate wavetables either based on name
    (saw, square, triangle), or given file containing spectrum values
    """
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
        spectrum_fragment = spectrum
        spectrum_fragment[n + 1:] = 0
        print('harmonics for ' + str(octave) + ' octave: ' + str(n))
        table[octave] = normalize(irfft(spectrum_fragment, 2048) * amp)

        axs[octave // 2, octave % 2].set_title('octave ' + str(octave))
        axs[octave // 2, octave % 2].plot(table[octave])

    plt.show()

    write_wavetables(table, name + '.synthwave', f_low, f_high)


if __name__ == '__main__':
    main()
