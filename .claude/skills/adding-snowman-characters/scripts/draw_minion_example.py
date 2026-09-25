"""EXAMPLE for drawing an original character with Pillow (produced minion_left.png).
Copy and adapt; keep the layout constraints below.

Draw a Minion, 3/4 view facing left, sized for the 47x95 snowman grid
(draw with pic_offset 0,0).

Layout constraints (src/Snowmen.cpp):
  - code's top hat (small_hat_0.png at 7,-2) covers x14-34, y8-23, centre x~24
    -> head top at y20, body centred on x24, goggle below the brim
  - snowball (eatSnowball) travels x=0..20, rows ~42..54, ends at mouth (17,51)
  - hit-hat snowball travels x=0..16, rows ~7..17 -> keep clear
  - snow pile grows in x0-13, y81-94 -> keep clear
  - idle mouth must be smaller than the opened mouth circle (r~3.2)
Pure black is transparent on the display, so dark details use dark gray.
"""
import sys
from PIL import Image, ImageDraw

W, H = 47, 95
YEL = (255, 205, 0)
YEL_SHADE = (205, 150, 0)
BLUE = (30, 90, 210)
BLUE_DARK = (15, 50, 140)
STRAP = (90, 90, 90)
SILVER = (190, 190, 200)
SILVER_DARK = (110, 110, 125)
WHITE = (255, 255, 255)
IRIS = (140, 80, 20)
PUPIL = (40, 25, 10)
HAIR = (80, 80, 80)
GLOVE = (70, 70, 70)
SHOE = (60, 60, 60)
LIP = (110, 40, 20)

im = Image.new("RGBA", (W, H), (0, 0, 0, 0))
d = ImageDraw.Draw(im)

# Body: capsule centred on x=24
L, R, TOP, BOT = 11, 37, 20, 86
d.ellipse([L, TOP, R, TOP + (R - L)], fill=YEL)
d.rectangle([L, TOP + (R - L) // 2, R, BOT - 8], fill=YEL)
d.ellipse([L, BOT - 16, R, BOT], fill=YEL)
# Shade the far (right) side for a 3/4 look
d.rectangle([R - 4, TOP + 12, R, BOT - 10], fill=YEL_SHADE)
d.ellipse([R - 8, TOP + 3, R, TOP + 20], fill=YEL_SHADE)
d.ellipse([L + 2, TOP + 1, R - 4, TOP + 22], fill=YEL)

# Hair sprouts: hidden under the hat, show when it is knocked off
for x, lean in [(20, -2), (24, 0), (28, 2)]:
    d.line([x, TOP + 1, x + lean, TOP - 5], fill=HAIR, width=1)

# Goggle strap across the head (behind goggle)
d.rectangle([L, TOP + 11, R, TOP + 15], fill=STRAP)

# Goggle on the front-left of the face, just below the hat brim
GX, GY, GR = 15, TOP + 13, 8
d.ellipse([GX - GR, GY - GR, GX + GR, GY + GR], fill=SILVER)
d.arc([GX - GR, GY - GR, GX + GR, GY + GR], 20, 200, fill=SILVER_DARK, width=2)
d.ellipse([GX - 5, GY - 5, GX + 5, GY + 5], fill=WHITE)
d.ellipse([GX - 5, GY - 2, GX - 1, GY + 2], fill=IRIS)   # looking left
d.rectangle([GX - 4, GY - 1, GX - 3, GY + 0], fill=PUPIL)

# Idle mouth: small closed smile centred on the mouth circle (17.5, 51)
for x, y in [(14, 50), (15, 51), (16, 52), (17, 52), (18, 52), (19, 51), (20, 50)]:
    d.point([x, y], fill=LIP)

# Overalls: bib, then pants
d.rectangle([L + 3, 58, R - 3, 66], fill=BLUE)
d.rectangle([L, 66, R, BOT - 8], fill=BLUE)
d.ellipse([L, BOT - 16, R, BOT], fill=BLUE)
d.rectangle([L + 9, 60, L + 17, 65], fill=BLUE_DARK)  # pocket
d.point([L + 13, 62], fill=SHOE)                       # Gru logo dot
# Straps up over the shoulders
d.line([L + 4, 58, L + 1, 54], fill=BLUE, width=2)
d.line([R - 4, 58, R - 1, 54], fill=BLUE, width=2)

# Arm hanging at the side (below snowball path, above snow pile), glove
d.line([L + 1, 60, L - 4, 70], fill=YEL, width=3)
d.ellipse([L - 8, 68, L - 2, 74], fill=GLOVE)

# Legs and shoes
d.rectangle([L + 6, BOT - 2, L + 11, BOT + 2], fill=BLUE)
d.rectangle([R - 12, BOT - 2, R - 7, BOT + 2], fill=BLUE)
d.rectangle([L + 3, BOT + 3, L + 11, BOT + 6], fill=SHOE)
d.rectangle([R - 15, BOT + 3, R - 7, BOT + 6], fill=SHOE)

im.save(sys.argv[1])
