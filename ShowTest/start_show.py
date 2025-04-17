import serial
import time
import subprocess
from threading import Thread, Lock
import sys
import termios
import tty
import select
import pygame

# === CONFIGURATION ===
SERIAL_PORT = "/dev/cu.usbserial-0001"  # Update as needed
BAUD_RATE = 9600
SONG_LENGTH_SECONDS = 200
MAX_LOG_LINES = 25
WE_ARE_AUDIO_FILE = "TheUAisYourFriend.mp3"
LOSE_YOURSELF_AUDIO_FILE = "Eminem_Lose_Yourself.mp3"

log_lines = []
LOG_LOCK = Lock()
input_buffer = ""
ser = None

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
                if line == "BTN:0":
                    stop_audio()
                    ser.write(b'x\n')
                if line == "BTN:1":
                    play_show_4()
                if line == "BTN:7":
                    play_show_1()
                if line == "BTN:2":
                    seek(20000)
                if line == "BTN:3":
                    seek(-10000)
        except Exception as e:
            log(f"[Serial Read Error] {e}")
            break

# === FUNCTION: Handle Input Character-by-Character ===
def read_input_custom():
    global input_buffer
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
                        if raw == "x":
                            # subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                            stop_audio()
                        elif raw == "SHOW:1":
                            play_show_1()
                        elif raw == "SHOW:2":
                            stop_audio()
                            # subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                        elif raw == "SHOW:3":
                            stop_audio()
                            # subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
                        elif raw == "SHOW:4":
                            play_show_4()
                        elif raw.startswith("SEEK:"):
                            log(f"[HOST] {raw}")
                            try:
                                global last_seek_pos, needs_seek_update
                                val = raw[5:].strip()
                                current_pos = pygame.mixer.music.get_pos() + last_seek_pos
                                if val.startswith("+"):
                                    offset = int(val[1:])
                                    if offset < 500:
                                        offset = offset * 1000
                                    seek(offset)
                                elif val.startswith("-"):
                                    offset = int(val[1:])
                                    if offset < 500:
                                        offset = offset * 1000
                                    seek(offset)
                                else:
                                    offset = int(val)
                                    if offset < 500:
                                        offset = offset * 1000
                                    seek_to(max(0, offset))
                            except Exception as e:
                                log(f"[Seek Error] {e}")
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

def seek(offset):
    if(pygame.mixer.music.get_pos() < 0):
        return
    global last_seek_pos
    current_pos = pygame.mixer.music.get_pos() + last_seek_pos
    new_pos = max(0, current_pos + offset)
    seek_to(new_pos)

def seek_to(new_pos):
    global last_seek_pos
    pygame.mixer.music.stop()
    pygame.mixer.music.play(start=new_pos / 1000.0)
    set_last_seek_pos(new_pos)

def play_show_1():
    ser.write(b'SHOW:1\n')
    stop_audio()
    pygame.mixer.music.load(LOSE_YOURSELF_AUDIO_FILE)
    pygame.mixer.music.play()
    set_last_seek_pos(0)
    log("[Audio] Playing Eminem_Lose_Yourself.mp3")

def play_show_4():
    ser.write(b'SHOW:4\n')
    stop_audio()
    pygame.mixer.music.load(WE_ARE_AUDIO_FILE)
    pygame.mixer.music.play()
    set_last_seek_pos(0)
    log("[Audio] Playing TheUAisYourFriend.mp3")

def stop_audio():
    try:
        pygame.mixer.music.stop()
    except Exception as e:
        log(f"[Audio Error] Failed to stop playback: {e}")

def set_last_seek_pos(last_pos):
    global last_seek_pos
    last_seek_pos = last_pos
    log(f"[Audio] Seeked to {last_pos} ms")
    time.sleep(0.25)  # Let playback resume before next sync
    needs_seek_update = True

# === Start Spotify Prep ===
# subprocess.run(["osascript", "-e", 'tell application "Spotify" to pause'])
# subprocess.run(["osascript", "-e", 'tell application "Spotify" to previous track'])

# === Open Serial First (Arduino resets) ===
log("Connecting to Arduino...")
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)

# === Initialize pygame mixer ===
pygame.mixer.init()
# === Start Threads ===
Thread(target=read_serial, args=(ser,), daemon=True).start()
Thread(target=read_input_custom, daemon=True).start()
time.sleep(1)

# === Send 's' to Start the Show ===
log("[Host →] Sending: s")
play_show_4()

# === Start Spotify AFTER Arduino is ready ===
# log("Starting Spotify playback...")
# track_uri = "spotify:track:5Z01UMMf7V1o0MzF86s6WJ"
# subprocess.run(["osascript", "-e", f'tell application \"Spotify\" to play track \"{track_uri}\"'])

# # === Main Loop: Send Time Syncs ===
# subprocess.run(["osascript", "-e", 'tell application "System Events" to set frontmost of process "iTerm2" to true'])

last_seek_pos = 0  # Track absolute position of playback
play_start_time = time.time()
needs_seek_update = False
last_sync = 0
try:
    while True:
        # get_pos returns time in milliseconds since playback started
        if(pygame.mixer.music.get_pos() < 0): 
            continue
        pos_ms = last_seek_pos + pygame.mixer.music.get_pos()
        if pos_ms >= 0:
            if abs(pos_ms - last_sync) > 5000 or needs_seek_update:
                needs_seek_update = False
                message = f"T:{pos_ms}\n"
                ser.write(message.encode())
                log(f"[Host →] Sending: {message.strip()}")
                last_sync = pos_ms
        time.sleep(0.02)
except KeyboardInterrupt:
    log("Stopped by user.")
finally:
    ser.close()
    pygame.mixer.quit()
