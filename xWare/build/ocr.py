import easyocr
import pyautogui
import time
import torch
import os
import numpy as np
import ctypes
import mmap
import sys
from rapidfuzz import fuzz, process # type: ignore

# Define the C-compatible structure
class InternalFlags(ctypes.Structure):
    _fields_ = [
        ("weaponName", ctypes.c_char * 64)
    ]

# Try to open the existing shared memory created by MidWareExternal
try:
    mem = mmap.mmap(
        -1,
        ctypes.sizeof(InternalFlags),
        tagname="MySharedMemory",
        access=mmap.ACCESS_WRITE
    )
except Exception as e:
    print(f"[ERROR] Failed to open shared memory: {e}")
    sys.exit(1)

# OCR setup
use_gpu = torch.cuda.is_available()
reader = easyocr.Reader(['en'], gpu=use_gpu)
region = (225, 480, 375, 180)

# Known weapon names and acceptable variants
weapon_aliases = {
    "F2": ["f2", "fz", "f z", "r2"],
    "G36C": ["g36c", "g3sc", "g36", "gs6c", "636C"],
    "R4C": ["r4c", "r4-c", "r 4 c", "r-4c", "r4 c"],
    "9x19VSN": ["9x19vsn", "9x19", "9x1gvsn", "9xigvsn", "9×19vsn", "9×19", "9x19v5n", "9x19v$n", "9xl9vsn", "9x19vsm", "9x19usn", "9x19vsu", "gx19vsn", "gxl9vsn", "9x19vsr",],
    "SMG12": ["5MG12", "SM91S"],
    "MP5": ["MPS", "M95", "M9S"],
    "ARX": ["ARX", "A5X"],
    "COMMANDO 9": ["C0mmando 9", "Command0 9", "C0mmand0 9", "Commando S"],
    "MP7": ["MP"],
    "BEARING 9": [""],
    "SMG 11": [""],
    "UMP45": [""],
    "AK-12": [""],
    "552 COMMANDO": [""],
    "416-C CARBINE": [""],
    "6P41": [""],
    "9mm C1": [""],
    "SCORPION EVO A1": [""],
    "T-5 SMG": [""],
    "DP27": [""],
}

# Create alias lookup table
alias_lookup = []
for canonical, aliases in weapon_aliases.items():
    for alias in aliases:
        alias_lookup.append((alias.lower().replace(" ", "").replace("-", ""), canonical))

def normalize(text):
    return text.lower().replace(" ", "").replace("-", "")

def match_weapon_name(ocr_text):
    norm_input = normalize(ocr_text)
    candidates = [a[0] for a in alias_lookup]
    match, score, _ = process.extractOne(norm_input, candidates, scorer=fuzz.ratio)
    if score >= 80:
        for alias, canonical in alias_lookup:
            if alias == match:
                return canonical
    return None

print(f"[INFO] Starting OCR loop (GPU: {'Yes' if use_gpu else 'No'})... Press Ctrl+C to stop.")

try:
    while True:
        screenshot = pyautogui.screenshot(region=region)
        image_np = np.array(screenshot)
        results = reader.readtext(image_np)

        os.system('cls' if os.name == 'nt' else 'clear')

        if results:
            text = results[0][1]
            print(f"[RAW OCR] {text} ({results[0][2]:.2f})")

            matched = match_weapon_name(text)

            if matched:
                print(f"[MATCHED] {matched}")
                flags = InternalFlags()
                text_bytes = matched.encode('utf-8')[:63]
                flags.weaponName = text_bytes + b'\x00' * (64 - len(text_bytes))
                mem.seek(0)
                mem.write(bytes(flags))
            else:
                print(f"[IGNORED] '{text}' (no close match)")
        else:
            print("[INFO] No text detected.")

        time.sleep(1)

except KeyboardInterrupt:
    print("\n[INFO] OCR loop stopped by user.")
finally:
    mem.close()