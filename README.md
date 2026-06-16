# fusion-kbd

RGB keyboard backlight controller for Gigabyte AERO / AORUS laptops on Linux,
targeting the `1044:7a3f` Chu Yuen Enterprise / Gigabyte Fusion controller.

Reverse-engineered from [martin31821/fusion-kbd-controller](https://github.com/martin31821/fusion-kbd-controller),
extended with a user-friendly shell command, persistent state, and systemd restore on boot.

## Tested on

- Gigabyte AERO 15 Studio XB — Ubuntu 24.04

## Known compatible device IDs

- `1044:7a3f` — Gigabyte AERO 15 SA, AERO 15 Studio XB, AERO 17 XD

## Requirements

- Ubuntu / Debian (or any distro with `apt`)
- `libusb-1.0`
- `gcc`
- Root access (libusb requires detaching the kernel HID driver)

## Install

```bash
git clone https://github.com/YOUR_USERNAME/fusion-kbd.git
cd fusion-kbd
chmod +x install.sh
sudo ./install.sh
```

## Usage

```bash
kb red                       # static red
kb white                     # static white
kb breathing blue            # breathing blue
kb neon                      # rainbow neon cycle
kb rainbow                   # rainbow marquee
kb rotate                    # rotating colors
kb hedge                     # hedge effect
kb raindrop                  # raindrop effect
kb circle                    # circle marquee
kb marquee                   # marquee snake
kb ripple red                # ripple on keypress (red)
kb flash green               # flash on keypress (green)
kb fade blue                 # fade on keypress (blue)
kb breathing red 30 3        # breathing red, brightness 30, speed 3
kb solid RRGGBB              # all keys one hex color: F4BB22
kb alternate '#RGB1' '#RGB2' # alternate two colors: '#4400BB' '#AA55BB'
kb gradient RRGGBB '#RGB'    # gradient across keys: 2299FF '#DD9944'
kb custom-file <file.bin>    # raw 512-byte payload
kb --last                    # reapply last saved setting
kb --last                    # reapply last saved setting
kb --help                    # full usage
```
* NOTE: If you want to use a hash character you need to wrap the value in quotes

## Brightness and speed

- Brightness: 0–50 (default 50)
- Speed: 1–10 (default 5)

```bash
kb breathing blue 40 8       # brightness 40, speed 8
```

## Persistence

Your last `kb` command is saved to `/var/lib/fusion-kbd/state` and
automatically reapplied at boot via the `fusion-kbd` systemd service.

## Modes not supported

- `wave` (mode 3) — produces no output on the `7a3f` controller

## Uninstall

```bash
chmod +x uninstall.sh
sudo ./uninstall.sh
```

## Credits

- Protocol reverse-engineering: [martin31821/fusion-kbd-controller](https://github.com/martin31821/fusion-kbd-controller)
- Interface identification and `7a3f` adaptation: discovered via HID descriptor analysis
