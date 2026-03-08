#!/usr/bin/env python3
"""Pre-slice and resize sprite sheets for Alien Multiplicity."""

from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parent.parent
ASSETS = ROOT / "assets"
SPRITES = ASSETS / "sprites"
FRAMES = ASSETS / "frames"

FRAMES.mkdir(parents=True, exist_ok=True)

ALIEN_SHEET = SPRITES / "alien_sheet.png"
SOLDIER_SHEET = SPRITES / "soldier_sheet.png"

ALIEN_CELL = 185
ALIEN_GAP = 10
ALIEN_STRIDE = ALIEN_CELL + ALIEN_GAP
SOLDIER_CELL = 256

PLAYER_SIZE = (48, 64)
ALIEN_SIZES = {
    "t1": (64, 64),
    "t2": (40, 40),
    "t3": (24, 24),
}
BEAM_SIZE = (256, 2)
HEART_SIZE = (16, 14)
GROUND_SIZE = (800, 60)


def save_resized(image: Image.Image, size: tuple[int, int], name: str) -> None:
    image.resize(size, Image.Resampling.NEAREST).save(FRAMES / f"{name}.png")
    print(name)


def save_mirror_variants(image: Image.Image, size: tuple[int, int], name: str) -> None:
    right = image.resize(size, Image.Resampling.NEAREST)
    left = right.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    right.save(FRAMES / f"{name}_right.png")
    left.save(FRAMES / f"{name}_left.png")
    print(f"{name}_right")
    print(f"{name}_left")


def crop_alien(sheet: Image.Image, row: int, col: int) -> Image.Image:
    x = col * ALIEN_STRIDE
    y = row * ALIEN_STRIDE
    return sheet.crop((x, y, x + ALIEN_CELL, y + ALIEN_CELL)).convert("RGBA")


def crop_soldier(sheet: Image.Image, row: int, col: int) -> Image.Image:
    x = col * SOLDIER_CELL
    y = row * SOLDIER_CELL
    return sheet.crop((x, y, x + SOLDIER_CELL, y + SOLDIER_CELL)).convert("RGBA")


def build_beam() -> None:
    beam = Image.new("RGBA", BEAM_SIZE, (0, 0, 0, 0))
    draw = ImageDraw.Draw(beam)
    draw.rectangle((0, 0, BEAM_SIZE[0] - 1, BEAM_SIZE[1] - 1), fill=(255, 226, 96, 255))
    beam.save(FRAMES / "beam.png")
    print("beam")


def build_heart() -> None:
    heart = Image.new("RGBA", HEART_SIZE, (0, 0, 0, 0))
    draw = ImageDraw.Draw(heart)
    pixels = [
        "..11..11......",
        ".111111111.....",
        "11111111111....",
        "111111111111...",
        "111111111111...",
        ".1111111111....",
        "..11111111.....",
        "...111111......",
        "....1111.......",
        ".....11........",
    ]
    for y, row in enumerate(pixels):
        for x, value in enumerate(row):
            if value == "1":
                draw.point((x + 1, y + 2), fill=(255, 78, 78, 255))
    heart.save(FRAMES / "hud_heart.png")
    print("hud_heart")


def build_ground() -> None:
    ground = Image.new("RGBA", GROUND_SIZE, (20, 28, 52, 255))
    draw = ImageDraw.Draw(ground)
    draw.rectangle((0, 0, GROUND_SIZE[0] - 1, 4), fill=(70, 90, 140, 255))
    for x in range(0, GROUND_SIZE[0], 16):
        draw.line((x, 18, x + 8, 28), fill=(36, 48, 86, 255), width=2)
        draw.line((x + 8, 28, x, 38), fill=(28, 40, 72, 255), width=2)
    ground.save(FRAMES / "ground.png")
    print("ground")


def slice_aliens() -> None:
    alien = Image.open(ALIEN_SHEET).convert("RGBA")
    alien_frames = {
        (0, 0): ("alien_t1_float0", ALIEN_SIZES["t1"]),
        (0, 1): ("alien_t1_float1", ALIEN_SIZES["t1"]),
        (0, 2): ("alien_t1_hit0", ALIEN_SIZES["t1"]),
        (0, 3): ("alien_t1_hit1", ALIEN_SIZES["t1"]),
        (0, 4): ("alien_t1_split", ALIEN_SIZES["t1"]),
        (1, 0): ("alien_t2_float0", ALIEN_SIZES["t2"]),
        (1, 1): ("alien_t2_float1", ALIEN_SIZES["t2"]),
        (1, 2): ("alien_t2_hit0", ALIEN_SIZES["t2"]),
        (1, 3): ("alien_t2_hit1", ALIEN_SIZES["t2"]),
        (1, 4): ("alien_t2_split", ALIEN_SIZES["t2"]),
        (3, 0): ("alien_t3_float0", ALIEN_SIZES["t3"]),
        (3, 1): ("alien_t3_float1", ALIEN_SIZES["t3"]),
        (3, 2): ("alien_t3_float2", ALIEN_SIZES["t3"]),
        (3, 3): ("alien_t3_death", ALIEN_SIZES["t3"]),
        (3, 4): ("alien_t3_vaporize", ALIEN_SIZES["t3"]),
    }

    for (row, col), (name, size) in alien_frames.items():
        save_resized(crop_alien(alien, row, col), size, name)


def slice_soldier() -> None:
    soldier = Image.open(SOLDIER_SHEET).convert("RGBA")
    soldier_frames = [
        ((0, 0), "soldier_idle"),
        ((0, 1), "soldier_run0"),
        ((0, 2), "soldier_run1"),
        ((0, 3), "soldier_run2"),
        ((1, 1), "soldier_run3"),
        ((0, 3), "soldier_jump"),
        ((3, 1), "soldier_shoot"),
        ((2, 3), "soldier_hit"),
        ((3, 3), "soldier_death"),
    ]

    for (row, col), name in soldier_frames:
        save_mirror_variants(crop_soldier(soldier, row, col), PLAYER_SIZE, name)


def main() -> None:
    slice_aliens()
    slice_soldier()
    build_beam()
    build_heart()
    build_ground()
    print("done")


if __name__ == "__main__":
    main()
