from scipy.io import wavfile
import matplotlib.pyplot as plt
import math
import cmath

def fft(samples: list[complex]) -> list[complex]:

    sample_size = len(samples)

    # ensure that sample size is a power of 2
    assert math.log(sample_size, 2).is_integer()

    if sample_size == 1:
        return
    
    half_sample_size = sample_size // 2

    even_samples = samples[::2]
    odd_samples = samples[1::2]

    assert len(even_samples) == len(odd_samples) == half_sample_size

    fft(even_samples)
    fft(odd_samples)

    omega = cmath.exp(-2j * cmath.pi / sample_size)

    for k in range(half_sample_size):
        w = omega ** k
        even_k = even_samples[k]
        odd_k = w * odd_samples[k]
        samples[k] = even_k + odd_k
        samples[k + half_sample_size] = even_k - odd_k

samplerate, data = wavfile.read('../Resources/encoding.wav')

sample_count = 8192

assert sample_count <= samplerate // 2

# Highest frequency shown in spectrum plot
max_spectrum_frequency = 6000

spectrum = [d + 0j for d in data[:sample_count]]
fft(spectrum)

max_frequency_component = round(max_spectrum_frequency / samplerate * sample_count)

time = [(1/samplerate) * i for i in range(sample_count)]
frequency = [i * samplerate / sample_count for i in range(max_frequency_component)]

# plt.plot(time, data[:sample_count])
plt.plot(frequency, [abs(c) / abs(max([abs(c) for c in spectrum[:max_frequency_component]])) for c in spectrum[:max_frequency_component]])
plt.show()
