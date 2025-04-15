import serial
import time
import subprocess
from threading import Thread, Lock
import sys
import termios
import tty
import select


# for audio
from pydub import AudioSegment
import simpleaudio as sa

# Load audio (MP3, WAV, etc.)
play_obj = None

# === CONFIGURATION ===
SERIAL_PORT = "/dev/cu.usbserial-0001"  # Update as needed
BAUD_RATE = 9600
SONG_LENGTH_SECONDS = 200
MAX_LOG_LINES = 25

log_lines = []
LOG_LOCK = Lock()
input_buffer = ""
ser = None
start_time = 0

def log(message):
    with LOG_LOCK:
        log_lines.append(message)
        if len(log_lines) > MAX_LOG_LINES:
            log_lines.pop(0)
        redraw_console()

def redraw_console():
    sys.stdout.write("\033[2J")  # Clear screen
    sys.stdout.write("\033[H")   # Cursor to top-left
    for line in log_lines:
        print(line)
    print("\n" + "-" * 40)
    sys.stdout.write("> " + input_buffer)
    sys.stdout.flush()

# === FUNCTION: Read Serial Responses ===
def read_serial(ser):
    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                log(f"[Arduino →] {line}")
        except Exception as e:
            log(f"[Serial Read Error] {e}")
            break

# === FUNCTION: Handle Input Character-by-Character ===
def read_input_custom():
    global input_buffer, play_obj
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    tty.setcbreak(fd)
    try:
        while True:
            if select.select([sys.stdin], [], [], 0.1)[0]:
                c = sys.stdin.read(1)
                if c == '\n':
                    raw = input_buffer.strip()
                    if raw:
                        message = raw + "\n"
                        ser.write(message.encode())
                        log(f"[Host →] (manual) Sent: {raw}")
                        global start_time
                        if raw == "x":
                            subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                            stop_audio()
                        elif raw == "SHOW:1":
                            track_uri = "spotify:track:5Z01UMMf7V1o0MzF86s6WJ"
                            subprocess.run(["osascript", "-e", f'tell application \"Spotify\" to play track \"{track_uri}\"'])
                            start_time = time.time()
                            subprocess.run(["osascript", "-e", 'tell application "System Events" to set frontmost of process "iTerm2" to true'])
                        elif raw == "SHOW:2":
                            start_time = time.time()
                            subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                        elif raw == "SHOW:3":
                            start_time = time.time()
                            subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                        elif raw == "SHOW:4":
                            subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                            usIsYourFriend = AudioSegment.from_file("TheUAisYourFriend.mp3")
                            play_obj = sa.play_buffer(
                                usIsYourFriend.raw_data,
                                num_channels=usIsYourFriend.channels,
                                bytes_per_sample=usIsYourFriend.sample_width,
                                sample_rate=usIsYourFriend.frame_rate
                            )
                            # track_uri = "spotify:track:596KUmMbTifuYdDC14lQdD"
                            # subprocess.run(["osascript", "-e", f'tell application \"Spotify\" to play track \"{track_uri}\"'])
                            start_time = time.time()
                            # subprocess.run(["osascript", "-e", 'tell application "System Events" to set frontmost of process "iTerm2" to true'])
                    input_buffer = ""
                    redraw_console()
                elif c == '\x7f':  # Backspace
                    input_buffer = input_buffer[:-1]
                    redraw_console()
                elif c.isprintable():
                    input_buffer += c
                    redraw_console()
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

def stop_audio():
    global play_obj
    try:
        if play_obj:
            if play_obj.is_playing():
                play_obj.stop()
            # Whether stopped or done, drop the reference
            play_obj = None
    except Exception as e:
        log(f"[Audio Error] Failed to stop playback: {e}")
        play_obj = None

# === Start Spotify Prep ===
subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
subprocess.run(["osascript", "-e", 'tell application "Spotify" to previous track'])

# === Open Serial First (Arduino resets) ===
log("Connecting to Arduino...")
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)

# === Start Threads ===
Thread(target=read_serial, args=(ser,), daemon=True).start()
Thread(target=read_input_custom, daemon=True).start()

# === Send 's' to Start the Show ===
log("[Host →] Sending: s")
ser.write(b'SHOW:1\n')

# === Start Spotify AFTER Arduino is ready ===
log("Starting Spotify playback...")
track_uri = "spotify:track:5Z01UMMf7V1o0MzF86s6WJ"
subprocess.run(["osascript", "-e", f'tell application \"Spotify\" to play track \"{track_uri}\"'])

# === Main Loop: Send Time Syncs ===
start_time = time.time()

subprocess.run(["osascript", "-e", 'tell application "System Events" to set frontmost of process "iTerm2" to true'])

try:
    while True:
        elapsed_sec = time.time() - start_time
        elapsed_ms = int(elapsed_sec * 1000)
        message = f"T:{elapsed_ms}\n"
        ser.write(message.encode())
        log(f"[Host →] Sending: {message.strip()}")
        time.sleep(10)

        # if elapsed_sec > SONG_LENGTH_SECONDS + 5:
        #     log("Done syncing. Closing serial.")
        #     break

except KeyboardInterrupt:
    log("Stopped by user.")

finally:
    ser.close()
