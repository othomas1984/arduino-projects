import serial
import time
import subprocess
from threading import Thread, Lock
import sys

# === CONFIGURATION ===
SERIAL_PORT = "/dev/cu.usbserial-0001"  # Update as needed
BAUD_RATE = 9600
SONG_LENGTH_SECONDS = 200
MAX_LOG_LINES = 10

# === GLOBAL LOG BUFFER ===
log_lines = []
LOG_LOCK = Lock()

def log(message):
    with LOG_LOCK:
        log_lines.append(message)
        if len(log_lines) > MAX_LOG_LINES:
            log_lines.pop(0)
        redraw_console()

def redraw_console():
    # Clear screen and reset cursor
    sys.stdout.write("\033[2J")  # Clear entire screen
    sys.stdout.write("\033[H")   # Move cursor to top left
    for line in log_lines:
        print(line)
    print("\n" + "-" * 40)
    print("> ", end="", flush=True)  # Input prompt (will be reused by input())

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

# === FUNCTION: Read Input from Terminal and Send to Arduino ===
def read_user_input(ser):
    while True:
        try:
            user_input = input("> ")
            if user_input.strip():  # skip empty input
                raw = user_input.strip()
                message = raw + "\n"
                ser.write(message.encode())
                log(f"[Host →] (manual) Sent: {message.strip()}")
                if raw == "SHOW:1":
                    track_uri = "spotify:track:5Z01UMMf7V1o0MzF86s6WJ"
                    subprocess.run(["osascript", "-e", f'tell application "Spotify" to play track "{track_uri}"'])
        except Exception as e:
            log(f"[Input Error] {e}")
            break

# === Start Spotify Prep ===
subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
subprocess.run(["osascript", "-e", 'tell application "Spotify" to previous track'])

# === Open Serial First (Arduino resets) ===
log("Connecting to Arduino...")
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)  # Allow Arduino time to reboot

# === Start Serial Listener Thread ===
Thread(target=read_serial, args=(ser,), daemon=True).start()

# === Start User Input Thread ===
Thread(target=read_user_input, args=(ser,), daemon=True).start()

# === Send 's' to Start the Show ===
log("[Host →] Sending: s")
ser.write(b's\n')

# === Start Spotify AFTER Arduino is ready ===
log("Starting Spotify playback...")
subprocess.run(["osascript", "-e", 'tell application "Spotify" to play'])

# === Main Loop: Send Time Syncs in Milliseconds ===
start_time = time.time()

try:
    while True:
        elapsed_sec = time.time() - start_time
        elapsed_ms = int(elapsed_sec * 1000)
        message = f"T:{elapsed_ms}\n"
        ser.write(message.encode())
        log(f"[Host →] Sending: {message.strip()}")
        time.sleep(10)

        if elapsed_sec > SONG_LENGTH_SECONDS + 5:
            log("Done syncing. Closing serial.")
            break

except KeyboardInterrupt:
    log("Stopped by user.")

finally:
    ser.close()
