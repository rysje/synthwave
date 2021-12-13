# synthwave

is a subtractive synthesizer designed to work on Raspberry Pi 4. Features include:
* basic waveform generation using wavetable synthesis
* a lowpass filter with controlled resonance and cutoff frequency
* controlling ADSR, filter and LFO parameters using MIDI CC commands
* low latency playback using [JACK](https://jackaudio.org)

To run synthwave on a Raspberry Pi, an external audio interface is necessary, since the 
built-in audio does not seem to support JACK and the latency is unbearable. \
In its current form, the synthesizer can be used on any Linux PC. Future relases will include 
features specific for Raspberry Pi, like controlling parameters via potentiometers connected to 
GPIO or displaying information on a screen connected via I2C.

## Dependencies

### Raspberry Pi OS (Bullseye or newer required)

* cmake *(build only)*
* libjack-jackd2-dev *(build only)*
* libstk0-dev *(build only)*
* stk
* jackd2

### Arch Linux

* cmake *(build only)*
* stk
* jack2

## Building

    git clone https://github.com/rysje/synthwave.git
    cd synthwave
    cmake -S . -B build
    cmake --build build --target synthwave

## Generating wavetables

Before running the synthesizer, at least one wavetable needs to be generated and placed in 
`build/wavetables`. The script for generating wavetables - `wavetable_gen.py` - currently 
supports three common wave shapes (saw, square, triangle).

    python wavetable_gen.py saw 20 14000
    mkdir -p build/wavetables
    cp saw.synthwave build/wavetables/

A text file containing Fourier series coefficients (each in a separate line) can be given 
instead of the wave name. Note that currently only 2048 harmonics are supported. Other values 
will give unexpected behavior. This will be improved in a future release. \
\
`wavetable_gen.py` requires python and libraries: numpy and matplotlib

## Running synthwave

If you've never used JACK server before, run it first and set appropriate settings. Make sure
that physical MIDI ports are available - this is best achieved with MIDI Driver "seq" option. \
After that, the settings should be saved to `~/.jackdrc`. synthwave is capable of starting JACK 
server using these default settings. \
To run synthwave, make sure the wavetables are present and simply start the executable.

## Acknowledgements

Wavetable oscillator was implemented following this
[series of blog posts](https://www.earlevel.com/main/category/digital-audio/oscillators/wavetable-oscillators/?order=ASC)
by Nigel Redmon \
synthwave uses filters and envelopes from [STK](https://ccrma.stanford.edu/software/stk/faq.html#license) library. \
synthwave uses [JACK API](https://jackaudio.org/api/) which is licensed under LGPL
