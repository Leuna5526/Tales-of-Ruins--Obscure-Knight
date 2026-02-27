import sys
from PIL import Image

def check_png(path):
    try:
        img = Image.open(path)
        print(f"File: {path}")
        print(f"Format: {img.format}")
        print(f"Mode: {img.mode}")
        print(f"Size: {img.size}")
        if img.mode == 'RGBA':
            extrema = img.getextrema()
            print(f"Alpha range: {extrema[3]}")
    except Exception as e:
        print(f"Error: {e}")

check_png("Obscure Knight/Assets/Level 2/Sparkle/sparkle (1).png")
