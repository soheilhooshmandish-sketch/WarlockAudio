#!/usr/bin/env python3
import os
import wave
import math
def build_warlock_assets():
    print("=== STARTING PROGRAMMATIC WARLOCK SOURCE RESOURCE INITIALIZER ===")
    os.makedirs("Resources", exist_ok=True)
    os.makedirs("BuildInstaller", exist_ok=True)
    # 1. Output a mock solid black panel faceplate asset file layout
    faceplate_path = os.path.join("Resources", "PedalFaceplate.png")
    if not os.path.exists(faceplate_path):
        with open(faceplate_path, "wb") as f:
            # Writing generic 64-byte file header to pass initialization checks safely
            f.write(b"\x89PNG\r\n\x1a\n" + b"\x00" * 56)
        print("-> Constructed faceplate mock matrix artifact layout.")
    # 2. Output programmatic high-gain cabinet impulse response files (44.1kHz / Mono)
    sr = 44100
    duration = 0.05
    num_samples = int(sr * duration)
    def write_mock_ir(filename, frequency_skew):
        path = os.path.join("Resources", filename)
        with wave.open(path, "wb") as w:
            w.setnchannels(1)
            w.setsampwidth(2)
            w.setframerate(sr)
            for i in range(num_samples):
                t = float(i) / sr
                # Exponential decay matching a high-gain cabinet resonance model
                decay = math.exp(-120.0 * t)
                val = math.sin(2.0 * math.pi * frequency_skew * (1.0 - t * 2.0) * t) * decay
                sample_data = int(max(-32768, min(32767, val * 32767)))
                w.writeframesraw(sample_data.to_bytes(2, byteorder="little", signed=True))
        print(f"-> Exported impulse file array target node: {path}")
    write_mock_ir("Cab_Center.wav", 440.0)
    write_mock_ir("Cab_Edge.wav", 220.0)
    print("=== RESOURCE ASSET GENERATION STAGE COMPLETION SUCCESSFULLY ===")
if __name__ == "__main__":
    build_warlock_assets()
