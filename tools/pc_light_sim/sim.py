import math
import time
import tkinter as tk
from dataclasses import dataclass


@dataclass
class Rgb:
    r: int = 0
    g: int = 0
    b: int = 0

    def clamp(self) -> "Rgb":
        return Rgb(max(0, min(255, int(self.r))), max(0, min(255, int(self.g))), max(0, min(255, int(self.b))))

    def to_hex(self) -> str:
        c = self.clamp()
        return f"#{c.r:02x}{c.g:02x}{c.b:02x}"


def scale(c: Rgb, s: float) -> Rgb:
    s = max(0.0, min(1.0, s))
    return Rgb(int(c.r * s), int(c.g * s), int(c.b * s))


def add_saturate(a: Rgb, b: Rgb) -> Rgb:
    return Rgb(min(255, a.r + b.r), min(255, a.g + b.g), min(255, a.b + b.b))


class PulseBlobEffect:
    def __init__(self) -> None:
        self.center_u = 0.5
        self.center_v = 0.5
        self.radius = 0.22
        self.pulse_hz = 1.2
        self.color = Rgb(255, 80, 20)

    def sample(self, u: float, v: float, now_ms: int) -> Rgb:
        du = u - self.center_u
        dv = v - self.center_v
        dist = math.sqrt(du * du + dv * dv)
        t = now_ms / 1000.0
        pulse = 0.5 + 0.5 * math.sin(2.0 * math.pi * self.pulse_hz * t)
        falloff = max(0.0, min(1.0, 1.0 - dist / max(self.radius, 1e-6)))
        falloff *= falloff
        intensity = max(0.0, min(1.0, 0.08 + pulse * falloff))
        return scale(self.color, intensity)


class StripeFieldEffect:
    def __init__(self) -> None:
        self.scroll_hz = 0.4
        self.stripe_width = 0.14
        self.color_a = Rgb(0, 0, 80)
        self.color_b = Rgb(0, 80, 255)

    def sample(self, u: float, v: float, now_ms: int) -> Rgb:
        _ = v
        t = now_ms / 1000.0
        x = (u + t * self.scroll_hz) % 1.0
        period = self.stripe_width * 2.0
        local = math.fmod(x, period)
        return self.color_a if local < self.stripe_width else self.color_b


class FlashFieldEffect:
    def __init__(self) -> None:
        self.center_u = 0.85
        self.center_v = 0.5
        self.radius = 0.25
        self.decay_ms = 180
        self.trigger_ms = 0
        self.color = Rgb(255, 220, 120)

    def trigger(self, now_ms: int) -> None:
        self.trigger_ms = now_ms

    def sample(self, u: float, v: float, now_ms: int) -> Rgb:
        dt = now_ms - self.trigger_ms
        if dt > self.decay_ms:
            return Rgb()
        du = u - self.center_u
        dv = v - self.center_v
        dist = math.sqrt(du * du + dv * dv)
        radial = max(0.0, min(1.0, 1.0 - dist / max(self.radius, 1e-6)))
        radial *= radial
        tf = max(0.0, min(1.0, 1.0 - dt / max(self.decay_ms, 1)))
        return scale(self.color, radial * tf)


LED_LAYOUT = [
    (0, 0.50, 0.05, 1.0),
    (1, 0.78, 0.14, 1.0),
    (2, 0.93, 0.38, 1.0),
    (3, 0.93, 0.62, 1.0),
    (4, 0.78, 0.86, 1.0),
    (5, 0.50, 0.95, 1.0),
    (6, 0.22, 0.86, 1.0),
    (7, 0.07, 0.62, 1.0),
    (8, 0.07, 0.38, 1.0),
    (9, 0.22, 0.14, 1.0),
]


class LightSimApp:
    def __init__(self) -> None:
        self.root = tk.Tk()
        self.root.title("Light Simulator")
        self.cell = 12
        self.canvas_w = 64
        self.canvas_h = 32
        self.show_led_overlay = True
        self.mode = "anim"
        self.effect_name = "pulse"
        self.start_time = time.perf_counter()
        self.last_manual_phase = 0.0

        self.pulse = PulseBlobEffect()
        self.stripes = StripeFieldEffect()
        self.flash = FlashFieldEffect()

        self.image_canvas = tk.Canvas(self.root, width=self.canvas_w * self.cell, height=self.canvas_h * self.cell, bg="black")
        self.image_canvas.grid(row=0, column=0, padx=10, pady=10)

        self.side = tk.Frame(self.root)
        self.side.grid(row=0, column=1, sticky="ns", padx=10, pady=10)

        self.status = tk.Label(self.side, justify="left", anchor="nw", font=("Consolas", 10), text="")
        self.status.pack(anchor="nw")

        self.led_canvas = tk.Canvas(self.side, width=220, height=220, bg="#101010")
        self.led_canvas.pack(pady=12)

        self.help_label = tk.Label(
            self.side,
            justify="left",
            anchor="nw",
            font=("Consolas", 9),
            text=(
                "Keys\n"
                "1 pulse\n2 stripes\n3 flash\n4 manual\n"
                "QWE/AD/ZXC move pulse\n"
                "[ ] radius\n- = speed\n"
                "Space trigger flash\n"
                "M toggle mode\nL toggle LED overlay\nEsc quit"
            ),
        )
        self.help_label.pack(anchor="nw")

        self.root.bind("<Key>", self.on_key)
        self.update()

    def now_ms(self) -> int:
        return int((time.perf_counter() - self.start_time) * 1000.0)

    def active_field(self):
        if self.effect_name == "pulse":
            return self.pulse
        if self.effect_name == "stripes":
            return self.stripes
        if self.effect_name == "flash":
            return self.flash
        return self.pulse

    def sample_field(self, u: float, v: float, now_ms: int) -> Rgb:
        if self.mode == "manual":
            return self.sample_manual(u, v, now_ms)
        base = self.active_field().sample(u, v, now_ms)
        if self.effect_name != "flash":
            base = add_saturate(base, self.flash.sample(u, v, now_ms))
        return base

    def sample_manual(self, u: float, v: float, now_ms: int) -> Rgb:
        phase = now_ms / 1000.0
        band = 0.5 + 0.5 * math.sin(2.0 * math.pi * (u * 1.8 + phase * 0.6))
        glow = max(0.0, 1.0 - abs(v - 0.5) / 0.45)
        r = int(255 * band)
        g = int(120 * glow)
        b = int(255 * (1.0 - band) * glow)
        return Rgb(r, g, b)

    def render_canvas(self, now_ms: int) -> None:
        self.image_canvas.delete("all")
        for y in range(self.canvas_h):
            v = y / max(1, self.canvas_h - 1)
            for x in range(self.canvas_w):
                u = x / max(1, self.canvas_w - 1)
                color = self.sample_field(u, v, now_ms).to_hex()
                x0 = x * self.cell
                y0 = y * self.cell
                self.image_canvas.create_rectangle(x0, y0, x0 + self.cell + 1, y0 + self.cell + 1, outline=color, fill=color)

        if self.show_led_overlay:
            for index, u, v, _scale in LED_LAYOUT:
                x = u * (self.canvas_w * self.cell)
                y = v * (self.canvas_h * self.cell)
                self.image_canvas.create_oval(x - 7, y - 7, x + 7, y + 7, outline="white", width=2)
                self.image_canvas.create_text(x, y - 14, text=str(index), fill="white", font=("Consolas", 9, "bold"))

    def render_leds(self, now_ms: int) -> None:
        self.led_canvas.delete("all")
        self.led_canvas.create_oval(20, 20, 200, 200, outline="#404040", width=2)
        for index, u, v, scale_factor in LED_LAYOUT:
            color = scale(self.sample_field(u, v, now_ms), scale_factor).to_hex()
            x = 20 + u * 180
            y = 20 + v * 180
            self.led_canvas.create_oval(x - 14, y - 14, x + 14, y + 14, outline="#303030", width=2, fill=color)
            self.led_canvas.create_text(x, y, text=str(index), fill="white", font=("Consolas", 8, "bold"))

    def update_status(self, now_ms: int) -> None:
        status = (
            f"mode: {self.mode}\n"
            f"effect: {self.effect_name}\n"
            f"time_ms: {now_ms}\n"
            f"pulse_center: ({self.pulse.center_u:.2f}, {self.pulse.center_v:.2f})\n"
            f"pulse_radius: {self.pulse.radius:.2f}\n"
            f"pulse_hz: {self.pulse.pulse_hz:.2f}\n"
            f"overlay: {self.show_led_overlay}"
        )
        self.status.config(text=status)

    def move_pulse(self, du: float, dv: float) -> None:
        self.pulse.center_u = max(0.0, min(1.0, self.pulse.center_u + du))
        self.pulse.center_v = max(0.0, min(1.0, self.pulse.center_v + dv))

    def on_key(self, event: tk.Event) -> None:
        key = event.keysym
        now_ms = self.now_ms()

        if key == "Escape":
            self.root.destroy()
            return
        if key == "1":
            self.mode = "anim"
            self.effect_name = "pulse"
        elif key == "2":
            self.mode = "anim"
            self.effect_name = "stripes"
        elif key == "3":
            self.mode = "anim"
            self.effect_name = "flash"
            self.flash.trigger(now_ms)
        elif key == "4":
            self.mode = "manual"
        elif key.lower() == "m":
            self.mode = "manual" if self.mode != "manual" else "anim"
            if self.mode == "anim" and self.effect_name == "manual":
                self.effect_name = "pulse"
        elif key.lower() == "l":
            self.show_led_overlay = not self.show_led_overlay
        elif key == "space":
            self.flash.trigger(now_ms)
        elif key.lower() == "q":
            self.move_pulse(-0.03, -0.03)
        elif key.lower() == "w":
            self.move_pulse(0.0, -0.03)
        elif key.lower() == "e":
            self.move_pulse(0.03, -0.03)
        elif key.lower() == "a":
            self.move_pulse(-0.03, 0.0)
        elif key.lower() == "d":
            self.move_pulse(0.03, 0.0)
        elif key.lower() == "z":
            self.move_pulse(-0.03, 0.03)
        elif key.lower() == "x":
            self.move_pulse(0.0, 0.03)
        elif key.lower() == "c":
            self.move_pulse(0.03, 0.03)
        elif key == "bracketleft":
            self.pulse.radius = max(0.05, self.pulse.radius - 0.02)
        elif key == "bracketright":
            self.pulse.radius = min(0.8, self.pulse.radius + 0.02)
        elif key == "minus":
            self.pulse.pulse_hz = max(0.1, self.pulse.pulse_hz - 0.1)
        elif key == "equal":
            self.pulse.pulse_hz = min(8.0, self.pulse.pulse_hz + 0.1)

    def update(self) -> None:
        now_ms = self.now_ms()
        self.render_canvas(now_ms)
        self.render_leds(now_ms)
        self.update_status(now_ms)
        self.root.after(33, self.update)

    def run(self) -> None:
        self.root.mainloop()


if __name__ == "__main__":
    LightSimApp().run()
