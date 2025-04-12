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

# === FUNCTION: Read Input from Terminal and Send to Arduino ===
def read_user_input(ser):
    while True:
        try:
            user_input = input()  # Blocking call
            if user_input.strip():  # skip empty input
                raw = user_input.strip()
                message = raw + "\n"
                ser.write(message.encode())
                print(f"[Host →] (manual) Sent: {message.strip()}")
                if raw == "SHOW:1":
                    track_uri = "spotify:track:5Z01UMMf7V1o0MzF86s6WJ"  # Replace with your song
                    subprocess.run(["osascript", "-e", f'tell application "Spotify" to play track "{track_uri}"'])
                    # subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                    # subprocess.run(["osascript", "-e", 'tell application "Spotify" to previous track'])
                    # subprocess.run(["osascript", "-e", 'tell application "Spotify" to play'])
        except Exception as e:
            print(f"[Input Error] {e}")
            break

# === Start Spotify Prep ===
subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
subprocess.run(["osascript", "-e", 'tell application "Spotify" to previous track'])

# === Open Serial First (Arduino resets) ===
print("Connecting to Arduino...")
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)  # Allow Arduino time to reboot

# === Start Serial Listener Thread ===
Thread(target=read_serial, args=(ser,), daemon=True).start()

# === Start User Input Thread ===
Thread(target=read_user_input, args=(ser,), daemon=True).start()

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
