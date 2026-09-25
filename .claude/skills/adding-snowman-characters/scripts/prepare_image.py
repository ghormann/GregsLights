"""Convert a downloaded character image into a display-ready RGBA PNG.

- scales to a target height (default 95 = grid height) with LANCZOS
- optional horizontal flip so the character faces LEFT
- makes the background transparent: pixels close to --bg (default black) and any
  near-black noise (max channel < --dark) become transparent, because the display
  lights near-black pixels as dim specks and skips only pure black

Usage:
  python prepare_image.py in.png out_left.png [--height 95] [--flip] [--bg 255,255,255] [--dark 30]
"""
import argparse

from PIL import Image

ap = argparse.ArgumentParser()
ap.add_argument("src")
ap.add_argument("dst")
ap.add_argument("--height", type=int, default=95)
ap.add_argument("--flip", action="store_true")
ap.add_argument("--bg", default="0,0,0", help="background colour to remove, r,g,b")
ap.add_argument("--bg-tol", type=int, default=40)
ap.add_argument("--dark", type=int, default=30)
a = ap.parse_args()

im = Image.open(a.src).convert("RGBA")
if im.height != a.height:
    im = im.resize((round(im.width * a.height / im.height), a.height), Image.LANCZOS)
if a.flip:
    im = im.transpose(Image.FLIP_LEFT_RIGHT)

bg = tuple(int(v) for v in a.bg.split(","))
px = im.load()
for x in range(im.width):
    for y in range(im.height):
        r, g, b, al = px[x, y]
        near_bg = all(abs(c - t) <= a.bg_tol for c, t in zip((r, g, b), bg))
        if al < 128 or near_bg or max(r, g, b) < a.dark:
            px[x, y] = (0, 0, 0, 0)
        else:
            px[x, y] = (r, g, b, 255)
im.save(a.dst)
print(f"wrote {a.dst} {im.size}")
