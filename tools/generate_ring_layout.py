from dataclasses import dataclass
from dataclasses_json import DataClassJsonMixin
import json
import math
from pathlib import Path


@dataclass
class RingSpec:
    ring_radius: float
    led_radius: float
    led_count: int
    angle_offset_deg: float = 0.0


@dataclass
class LedSpec(DataClassJsonMixin):
    index: int
    led_radius: float
    u: float
    v: float


@dataclass
class LedLayout(DataClassJsonMixin):
    name: str
    coordinate_space: str
    leds: list[LedSpec]


def build_layout(name: str, rings: list[RingSpec]) -> LedLayout:
    """
    Build a normalized 2D LED layout.

    Input ring_radius and led_radius can be in any consistent units.
    The output coordinates are normalized into [0, 1].
    """
    raw_leds = []

    for ring_id, ring in enumerate(rings):
        if ring.led_count <= 0:
            continue

        if ring.led_count == 1:
            angles_deg = [ring.angle_offset_deg]
        else:
            step_deg = 360.0 / ring.led_count
            angles_deg = [ring.angle_offset_deg + i * step_deg for i in range(ring.led_count)]

        for ring_led_index, angle_deg in enumerate(angles_deg):
            angle_rad = math.radians(angle_deg)

            # 0 degrees points upward to match common LED layout intuition
            x = ring.ring_radius * math.sin(angle_rad)
            y = -ring.ring_radius * math.cos(angle_rad)

            raw_leds.append({
                "index": len(raw_leds),
                "ring": ring_id,
                "ring_led_index": ring_led_index,
                "x": x,
                "y": y,
                "led_radius": ring.led_radius,
            })

    if not raw_leds:
        return LedLayout(
            name=name,
            coordinate_space="normalized_2d",
            leds=[],
        )

    min_x = min(led["x"] - led["led_radius"] for led in raw_leds)
    max_x = max(led["x"] + led["led_radius"] for led in raw_leds)
    min_y = min(led["y"] - led["led_radius"] for led in raw_leds)
    max_y = max(led["y"] + led["led_radius"] for led in raw_leds)

    width = max_x - min_x
    height = max_y - min_y

    # Avoid divide-by-zero for degenerate layouts
    if width == 0:
        width = 1.0
    if height == 0:
        height = 1.0

    leds = []
    for led in raw_leds:
        u = (led["x"] - min_x) / width
        v = (led["y"] - min_y) / height

        leds.append(
            LedSpec(
                index=led["index"],
                led_radius=led["led_radius"] / width,
                u=round(u, 6),
                v=round(v, 6),
            )
        )

    return LedLayout(
        name=name,
        coordinate_space="normalized_2d",
        leds=leds,
    )


def format_json_limited_depth(obj, indent=2, max_depth=2, _depth=0):
    space = " " * (indent * _depth)
    next_space = " " * (indent * (_depth + 1))

    if _depth >= max_depth:
        return json.dumps(obj, separators=(", ", ": "))

    if isinstance(obj, dict):
        if not obj:
            return "{}"
        items = []
        for k, v in obj.items():
            formatted_v = format_json_limited_depth(v, indent, max_depth, _depth + 1)
            items.append(f'{next_space}{json.dumps(k)}: {formatted_v}')
        return "{\n" + ",\n".join(items) + f"\n{space}" + "}"

    if isinstance(obj, list):
        if not obj:
            return "[]"
        items = []
        for v in obj:
            formatted_v = format_json_limited_depth(v, indent, max_depth, _depth + 1)
            items.append(f"{next_space}{formatted_v}")
        return "[\n" + ",\n".join(items) + f"\n{space}" + "]"

    return json.dumps(obj)


def save_layout(path: str | Path, layout: dict) -> None:
    path = Path(path)
    path.write_text(format_json_limited_depth(layout, indent=2, max_depth=2) + "\n", encoding="utf-8")


def main() -> None:
    rings = [
        RingSpec(ring_radius=0.0, led_radius=1.0, led_count=1, angle_offset_deg=0.0),
        RingSpec(ring_radius=12.0, led_radius=1.0, led_count=8, angle_offset_deg=0.0),
        RingSpec(ring_radius=22.0, led_radius=1.0, led_count=12, angle_offset_deg=0.0),
        RingSpec(ring_radius=32.0, led_radius=1.0, led_count=16, angle_offset_deg=0.0),
        RingSpec(ring_radius=44.0, led_radius=1.0, led_count=24, angle_offset_deg=0.0),
    ]

    layout = build_layout("concentric_5_ring", rings)
    save_layout("ring_layout.json", layout.to_dict(encode_json=True))
    print("Wrote ring_layout.json")


if __name__ == "__main__":
    main()
