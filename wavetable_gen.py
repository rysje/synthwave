#!/usr/bin/env python
import math as m
import matplotlib.pyplot as plt
import sys

def gen_saw(t, n_harmonics, amp=1):
	sum = 0
	for i in range(1, n_harmonics + 1):
		sum += (-1)**i * m.sin(2*m.pi*t*i) / i
	return amp * (- 1/m.pi * sum)

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
	f_low = int(sys.argv[1])
	f_high = int(sys.argv[2])
	amp = 0.8
	octaves = m.ceil(m.log2(f_high/f_low))
	fig, axs = plt.subplots(m.ceil(octaves / 2), 2, sharex=True, sharey=True)
	fig.suptitle('Wavetables for frequency range ' + str(f_low) + "Hz - " + str(f_high) + "Hz")
	table = octaves*[None]
	for octave in range(octaves):
		n = m.floor(16000 / (f_low * 2 ** octave))
		print("harmonics for " + str(octave) + " octave: " + str(n))
		table[octave] = [gen_saw(i/2048, n, amp) for i in range(2048)]

		axs[octave // 2, octave % 2].set_title('octave ' + str(octave))
		axs[octave // 2, octave % 2].plot(table[octave])

	#plt.show()

	write_wavetables(table, 'saw.synthwave', f_low, f_high)

if __name__ == '__main__':
	main()
