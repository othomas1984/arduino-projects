import pyaudio
import numpy as np
import time
import shutil

# === Config ===
CHUNK = 1024
RATE = 44100
DEVICE_INDEX = 0
NUM_BANDS = 16
BAR_HEIGHT = 20
SLEEP_TIME = 0.05
SMOOTHING = 0.7  # 0 = fast, 1 = slow smoothing
NOISE_FLOOR = 5  # magnitude below this is considered silence
TARGET_MAX = 15  # baseline max value for normalization

# === Setup PyAudio ===
p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16,
                channels=1,
                rate=RATE,
                input=True,
                input_device_index=DEVICE_INDEX,
                frames_per_buffer=CHUNK)

# === FFT band mapping ===
def calculate_fft_bands(signal, rate, num_bands):
    fft = np.fft.rfft(signal)
    magnitude = np.abs(fft)

    # ✅ Clamp low-level noise
    magnitude[magnitude < NOISE_FLOOR] = 0

    # ✅ Log compression
    magnitude = np.log1p(magnitude)

    freqs = np.fft.rfftfreq(len(signal), 1.0 / rate)
    band_edges = np.logspace(np.log10(20), np.log10(rate / 2), num_bands + 1)
    band_values = []

    for i in range(num_bands):
        low = band_edges[i]
        high = band_edges[i + 1]
        indices = np.where((freqs >= low) & (freqs < high))[0]
        if len(indices) > 0:
            value = np.mean(magnitude[indices])
            band_values.append(value)
        else:
            band_values.append(0)

    return band_values

# === Bar drawing ===
def draw_bars(values, max_height, width):
    # ✅ Normalize with fixed target max
    max_val = max(max(values), TARGET_MAX)
    normalized = [int((v / max_val) * max_height) for v in values]

    lines = []
    for row in range(max_height, 0, -1):
        line = ""
        for val in normalized:
            if val >= row:
                line += "█".ljust(width)
            else:
                line += " ".ljust(width)
        lines.append(line)
    return "\n".join(lines)

# === Clear screen helper ===
def clear():
    print("\033[H\033[J", end="")

# === Main loop ===
print("🎧 EQ Visualizer (Ctrl+C to stop)")
bar_width = shutil.get_terminal_size((80, 20)).columns // NUM_BANDS
previous_values = [0] * NUM_BANDS  # for smoothing

try:
    while True:
        data = stream.read(CHUNK, exception_on_overflow=False)
        signal = np.frombuffer(data, dtype=np.int16).astype(np.float32)
        bands = calculate_fft_bands(signal, RATE, NUM_BANDS)

        # ✅ Smooth the bars
        bands = [
            SMOOTHING * previous_values[i] + (1 - SMOOTHING) * bands[i]
            for i in range(NUM_BANDS)
        ]
        previous_values = bands

        # ✅ Zero out residual noise
        bands = [0 if b < 1 else b for b in bands]

        clear()
        print(draw_bars(bands, BAR_HEIGHT, bar_width))
        time.sleep(SLEEP_TIME)

except KeyboardInterrupt:
    print("\n🛑 Stopping.")
    stream.stop_stream()
    stream.close()
    p.terminate()
