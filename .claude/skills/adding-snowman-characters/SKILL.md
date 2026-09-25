---
name: adding-snowman-characters
description: Use when adding, replacing, repositioning, or suggesting a character for the right-hand "snowman" in GregsLights (SnowmanPicture entries in Snowmen.cpp), or when a character looks wrong during the snowball fight — hat floating or misplaced, mouth not matching the face, snowball passing through the body, snow pile covering the character.
---

# Adding Snowman Characters

## Overview
The right-hand snowman can be any picture (`SnowmanPicture` in `GregsLights/src/Snowmen.cpp`, `createSnowmanPictures()`), but the animations are drawn at **fixed grid positions** — not relative to the picture. A character works only if its image is laid out around those positions. Fix layout problems **in the image first**; offsets only slide the whole picture.

## Fixed geometry (47x95 grid, x right, y down)

| Element | Where | Implication for the image |
|---|---|---|
| Transparency | pure black (0,0,0) is skipped; alpha is premultiplied | near-black noise lights up as dim specks — remove it; dark details need dark gray, not black |
| Hat (`small_hat_0.png` at 7,-2) | pixels x14-34, y8-23, centre x≈24 | **no hat in the image**; head top ≈ y20, head-top centre ≈ x24 |
| Eat-snowball ball | frames x=0,4..20, y=32..38+`splash_offset_y`, r=3 | face **left**; nothing protruding left at mouth height (snouts, long noses, hands) |
| Mouth | black circle at (15+`mouth_offset_x`, 34+`mouth_offset_y`), r up to 2.7×`mouth_multiplier` | ball's last frame (x=20) should land on it; face behind it must be lit |
| Drawn mouth in image | covered by that circle only | **no big grin/open mouth**; at most a small closed mouth ≤7px wide centred on the circle |
| Hit-hat ball | x=0..16, y≈7-17 | keep the goggles/eyes/etc. below y≈18 |
| Missed-snow pile (`show_misses`) | circle at (1,93), r up to 12 → x0-13, y81-94 | keep that corner empty, or set `show_misses = false` |

## Picking a character
Needs: profile or 3/4 view facing left, lit face with a small or closed mouth, recognizable at 47px wide in bold colour. Rejected examples: Snoopy/Bluey/Rudolph (snout in ball path), Kevin McCallister (front-facing, hands on cheeks), Mario (nose), Creeper/Pikachu (only recognizable front-on), penguins (black head hides black mouth). Original characters drawn with Pillow (see `scripts/draw_minion_example.py`) can be designed to the table exactly.

## Workflow
1. **Python env**: create a venv in the scratchpad first (`python3 -m venv venv && venv/bin/pip install pillow`) — the user wants a venv, not system pip.
2. **Get the image** into `GregsLights/resources/<name>_left.png`:
   - downloaded: `python scripts/prepare_image.py in.png out_left.png [--flip] [--bg 255,255,255]`
   - drawn: copy/adapt `scripts/draw_minion_example.py` (47x95, offsets 0,0).
3. **Check**: `python scripts/check_character.py --image out_left.png check.png --ox .. --oy .. --mox .. --moy .. --soy ..` (or `--entry "Name"` to read offsets from Snowmen.cpp). **Read the PNG** — 5 panels: idle+hat, ball path, mouth open, hat knocked off, max pile — and the printed report.
4. **Fix in the image, then re-check** (see Common fixes). Iterate until every panel looks right.
5. **Add the entry** at the **end** of `createSnowmanPictures()` — vote IDs (`/christmas/vote/snowmenQueue`) are list indices, so never insert in the middle; tell the user the new ID for the voting site. `name_offset_x ≈ (96 - 8*len(name))/2`. `splash_end_y ≈ splash_offset_y + 5`.
6. **Startup character** = the last entry with `this->who_right = who;` uncommented; comment it on the others you add.
7. **Build & run**: from `GregsLights/`, `make graphics` then `./GregsLights` (WSLg shows the window; no VM needed). If linking fails with wx undefined references, stale objects from an older wxWidgets: `make clean_graphics && make graphics`. The app uses `~/greglights_config.json` (real MQTT broker and FPP IPs) — ask before launching it yourself.

## Common fixes

| Symptom | Fix |
|---|---|
| Opening mouth shows edges of a drawn grin | paint the grin out (replace dark pixels with averaged neighbouring face colour), add small closed mouth centred on the circle |
| Two hats / hat looks wrong | remove the image's hat, paint a rounded head top ≈ y20 |
| Hat too far forward/back | shift `pic_offset_x` by the error and `mouth_offset_x` by the same; re-check ball overshoot |
| Pile covers body | `show_misses = false` (Grinch, Mary, Emily, Cousin Eddie do this) |
| Ball overshoots mouth by >6px | move mouth nearer x≈17-20 (image layout), not just offsets |

## Calibrating the report
Minion and The Grinch were tuned with this check and pass cleanly. Older entries (Clark, Tails) trip some warnings yet are acceptable on the display — treat thresholds as advisory and compare panels against Minion/Grinch.
