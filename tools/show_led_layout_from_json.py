from pathlib import Path
import json
import matplotlib.pyplot as plt
from matplotlib.patches import Circle


def load_layout(path: str | Path) -> dict:
    path = Path(path)
    if not path.exists():
        raise FileNotFoundError(f"Layout file not found: {path}")
    with path.open("r", encoding="utf-8") as f:
        return json.load(f)


def draw_layout(layout: dict, show_indices: bool = True) -> None:
    name = layout.get("name", "LED Layout")
    coordinate_space = layout.get("coordinate_space", "")
    leds = layout.get("leds", [])

    if coordinate_space != "normalized_2d":
        raise ValueError(
            f"Unsupported coordinate_space: {coordinate_space!r}. "
            "Expected 'normalized_2d'."
        )

    fig, ax = plt.subplots()

    for led in leds:
        u = float(led["u"])
        v = float(led["v"])
        r = float(led["led_radius"])
        index = int(led["index"])

        circle = Circle((u, v), r, fill=False, linewidth=1.5)
        ax.add_patch(circle)

        if show_indices:
            ax.text(u, v, str(index), ha="center", va="center", fontsize=8)

    ax.set_title(name)
    ax.set_aspect("equal")
    ax.set_xlim(0.0, 1.0)
    ax.set_ylim(1.0, 0.0)  # invert so smaller v appears toward the top
    ax.set_xlabel("u")
    ax.set_ylabel("v")
    ax.grid(True)

    plt.show()


def main() -> None:
    layout_path = Path("ring_layout.json")
    layout = load_layout(layout_path)
    draw_layout(layout, show_indices=True)


if __name__ == "__main__":
    main()
