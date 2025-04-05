import serial
import time
import subprocess
from threading import Thread

# === CONFIGURATION ===
SERIAL_PORT = "/dev/cu.usbserial-0001"  # Update as needed
BAUD_RATE = 9600
SONG_LENGTH_SECONDS = 200

# === FUNCTION: Read Serial Responses ===
def read_serial(ser):
    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                print(f"[Arduino →] {line}")
        except Exception as e:
            print(f"[Serial Read Error] {e}")
            break
subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
subprocess.run(["osascript", "-e", 'tell application "Spotify" to previous track'])

# === Open Serial First (Arduino resets) ===
print("Connecting to Arduino...")
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)  # Allow Arduino time to reboot

# === Start Serial Listener Thread ===
serial_thread = Thread(target=read_serial, args=(ser,))
serial_thread.daemon = True
serial_thread.start()

# === Send 's' to Start the Show ===
print("[Host →] Sending: s")
ser.write(b's\n')

# === Start Spotify AFTER Arduino is ready ===
print("Starting Spotify playback...")
subprocess.run(["osascript", "-e", 'tell application "Spotify" to play'])

# === Main Loop: Send Time Syncs in Milliseconds ===
start_time = time.time()

try:
    while True:
        elapsed_sec = time.time() - start_time
        elapsed_ms = int(elapsed_sec * 1000)
        message = f"T:{elapsed_ms}\n"
        ser.write(message.encode())
        print(f"[Host →] Sending: {message.strip()}")
        time.sleep(10)

        if elapsed_sec > SONG_LENGTH_SECONDS + 5:
            print("Done syncing. Closing serial.")
            break

except KeyboardInterrupt:
    print("Stopped by user.")

finally:
    ser.close()
