"""Simulate a right-hand snowman character on the 47x95 grid and report problems.

Mirrors src/Snowmen.cpp: drawSnowmen (picture + small_hat_0.png at 7,-2),
eatSnowball/drawMouth (ball frames + black mouth circle), hitHat/throwHitHat,
and the missed-snow pile. Pure black (0,0,0) is transparent, like showPictureNow.

Usage:
  python check_character.py --entry "Minion" out.png          # offsets from Snowmen.cpp
  python check_character.py --image new.png out.png --ox 0 --oy 0 --mox 2 --moy 17 --soy 13
  (with --entry, any offset flag overrides the value read from the code)

Panels: idle+hat | ball path (cyan) | mouth open | hat knocked off | max snow pile
"""
import argparse
import math
import re
from pathlib import Path

from PIL import Image

REPO = Path(__file__).resolve().parents[4]
CODE = REPO / "GregsLights" / "src" / "Snowmen.cpp"
RES = REPO / "GregsLights" / "resources"

W, H = 47, 95
POS_Y = [32, 33, 34, 34, 36, 38]      # eatSnowball pos_y, visible frames x=0,4..20
BALL_R = 3                             # BALL_SIZE_1IN
HAT_X, HAT_Y = 7, -2                   # drawSnowmen hat position
HAT_CENTER_X = 24                      # small_hat_0.png pixels land on x14-34
PILE_C, PILE_R = (1, H - 2), 12        # drawGroundSnow at level 4
CYAN, WHITE, BLACK = (0, 255, 255), (255, 255, 255), (0, 0, 0)


def read_entry(name):
    src = CODE.read_text()
    for block in src.split("new SnowmanPicture();")[1:]:
        block = block.split("push_back")[0]
        if f'name = "{name}"' not in block:
            continue
        num = lambda f, d: float(m.group(1)) if (m := re.search(rf"{f}\s*=\s*(-?[\d.]+)", block)) else d
        return {
            "image": RES / re.search(r'getPicture\("([^"]+)"\)', block).group(1),
            "ox": int(num("pic_offset_x", 0)), "oy": int(num("pic_offset_y", 0)),
            "mox": int(num("mouth_offset_x", 0)), "moy": int(num("mouth_offset_y", 0)),
            "mult": num("mouth_multiplier", 1), "soy": int(num("splash_offset_y", 0)),
            "misses": "show_misses = false" not in block,
        }
    raise SystemExit(f'No SnowmanPicture named "{name}" in {CODE}')


def load(path):
    im = Image.open(path).convert("RGBA")
    px = im.load()
    return [[tuple(c * px[x, y][3] // 255 for c in px[x, y][:3]) for y in range(im.height)]
            for x in range(im.width)], im.width, im.height


def paste(grid, pic, px, py):
    img, w, h = pic
    for x in range(w):
        for y in range(h):
            if img[x][y] != BLACK and 0 <= x + px < W and 0 <= y + py < H:
                grid[x + px][y + py] = img[x][y]
    return grid


def circle_pts(cx, cy, r):
    return [(x, y) for x in range(int(cx - r), int(cx + r) + 1)
            for y in range(int(cy - r), int(cy + r) + 1)
            if math.hypot(x - cx, y - cy) <= r and 0 <= x < W and 0 <= y < H]


def fill(grid, pts, color):
    for x, y in pts:
        grid[x][y] = color
    return grid


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    src = ap.add_mutually_exclusive_group(required=True)
    src.add_argument("--entry", help="SnowmanPicture name in Snowmen.cpp")
    src.add_argument("--image", type=Path)
    for f in ("ox", "oy", "mox", "moy", "soy"):
        ap.add_argument(f"--{f}", type=int)
    ap.add_argument("--mult", type=float)
    ap.add_argument("--no-misses", action="store_true")
    a = ap.parse_args()

    cfg = read_entry(a.entry) if a.entry else {
        "image": a.image, "ox": 0, "oy": 0, "mox": 0, "moy": 0, "mult": 1, "soy": 0, "misses": True}
    for f in ("ox", "oy", "mox", "moy", "soy", "mult"):
        if getattr(a, f) is not None:
            cfg[f] = getattr(a, f)
    if a.no_misses:
        cfg["misses"] = False

    pic = load(cfg["image"])
    hat = load(RES / "small_hat_0.png")
    body = paste([[BLACK] * H for _ in range(W)], pic, cfg["ox"], cfg["oy"])
    lit = lambda x, y: body[x][y] != BLACK
    fresh = lambda with_hat=True: paste([col[:] for col in body], hat, HAT_X, HAT_Y) if with_hat else [col[:] for col in body]

    mx, my = 15 + cfg["mox"], 34 + cfg["moy"]
    mouth_r = 2.7 * cfg["mult"]
    mouth_pts = set(circle_pts(mx, my, mouth_r)) | set(circle_pts(mx + 1, my, mouth_r))
    balls = [circle_pts(i * 4, py + cfg["soy"], BALL_R) for i, py in enumerate(POS_Y)]

    path_g = fresh()
    for b in balls:
        fill(path_g, b, CYAN)
    mouth_g = fill(fresh(), mouth_pts, BLACK)
    fill(mouth_g, balls[-1], CYAN)
    hit_g = paste(fresh(False), load(RES / "small_hat_40.png"), 19, -2)
    for i, y in enumerate([10, 11, 12, 13, 14]):
        fill(hit_g, circle_pts(i * 4, y, BALL_R), CYAN)
    pile_g = fresh()
    if cfg["misses"]:
        fill(pile_g, circle_pts(*PILE_C, PILE_R), WHITE)

    # ---- report ----
    print(f"image {cfg['image'].name}  offsets ox={cfg['ox']} oy={cfg['oy']} "
          f"mox={cfg['mox']} moy={cfg['moy']} mult={cfg['mult']} soy={cfg['soy']} misses={cfg['misses']}")
    ball_end = (20, POS_Y[-1] + cfg["soy"])
    print(f"mouth centre ({mx}.5,{my})  ball ends {ball_end}  "
          f"dx={ball_end[0] - mx - 0.5:+.1f} dy={ball_end[1] - my:+d}  (want dx 0..6, |dy|<=3)")
    if not all(lit(x, y) for x, y in mouth_pts):
        print("  WARN mouth circle is partly off the character (black mouth on black = invisible)")
    in_path = {p for b in balls for p in b if lit(*p) and p[0] < mx - 3}
    print(f"character pixels the ball crosses before the mouth: {len(in_path)}"
          + ("  WARN something protrudes into the ball path" if len(in_path) > 40 else ""))
    # first row at least 6 px wide, so thin hair/antennae under the hat are ignored
    top = next((y for y in range(H) if sum(lit(x, y) for x in range(W)) >= 6), None)
    if top is not None:
        rows = [x for y in range(top, min(top + 6, H)) for x in range(W) if lit(x, y)]
        head_c = (min(rows) + max(rows)) / 2
        print(f"head top y={top} (hat brim ~y23; want ~18-22)  head-top centre x={head_c:.1f} "
              f"(hat centre x={HAT_CENTER_X}; want within ~2)")
    hit_path = {p for i, y in enumerate([10, 11, 12, 13, 14]) for p in circle_pts(i * 4, y, BALL_R) if lit(*p)}
    print(f"character pixels hit by the hit-hat snowball: {len(hit_path)}")
    if cfg["misses"]:
        pile = [p for p in circle_pts(*PILE_C, PILE_R) if lit(*p)]
        print(f"character pixels covered by max snow pile: {len(pile)}"
              + ("  WARN pile overlays the body; clear x0-13,y81-94 or use show_misses=false" if len(pile) > 10 else ""))

    panels, s, gap = [fresh(), path_g, mouth_g, hit_g, pile_g], 5, 4
    img = Image.new("RGB", (len(panels) * (W + gap) * s, H * s), (40, 40, 40))
    for k, g in enumerate(panels):
        for x in range(W):
            for y in range(H):
                img.paste(g[x][y], ((k * (W + gap) + x) * s, y * s, (k * (W + gap) + x + 1) * s - 1, (y + 1) * s - 1))
    img.save(a.out)
    print(f"wrote {a.out}")


if __name__ == "__main__":
    main()
