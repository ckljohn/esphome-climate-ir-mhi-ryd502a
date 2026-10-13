# ESPHome Mitsubishi Heavy Industries A/C RYD502A

This project adds ESPHome support for Mitsubishi Heavy Industries (MHI) air-conditioners that use the RYD502A IR remote.

It has been tested with an MHI SRK**MWKH1 indoor unit and the matching RYD502A remote control.

## Features

- Fan speed control from level 1 to level 4
  - The ESPHome climate entity exposes three fan levels; `Off` is used as level 1.
- Temperature control from 18°C to 30°C
- Swing modes: off, horizontal, vertical, both
- Operation modes: auto, cool, dry, fan, heat
- Custom default airflow positions for vertical and horizontal directions

## Installation

Add the component as an external ESPHome component:

```yaml
external_components:
  - source: github://ckljohn/esphome-climate-ir-mhi-ryd502a@main
    components: [mhi_ryd502a]
```

Then configure your IR transmitter and the climate entity.

## Example configuration

See also [example_mhi_ryd502a.yaml](example_mhi_ryd502a.yaml).

```yaml
external_components:
  - source: github://ckljohn/esphome-climate-ir-mhi-ryd502a@main
    components: [mhi_ryd502a]

remote_transmitter:
  pin: GPIO4
  carrier_duty_percent: 50%

climate:
  - name: "A/C"
    platform: mhi_ryd502a
    supports_heat: false
    vertical_default: "middle-down"
    horizontal_default: "split"
```

## Configuration options

- `horizontal_default` (optional, string): Default horizontal airflow direction when the AC is not in swing mode.
  - Supported values: `left`, `middle-left`, `middle`, `middle-right`, `right`, `split`, `focus`
  - Default: `middle`
- `vertical_default` (optional, string): Default vertical airflow direction when the AC is not in swing mode.
  - Supported values: `down`, `middle-down`, `middle`, `middle-up`, `up`
  - Default: `middle`
- All other supported options from the [ESPHome IR climate component](https://esphome.io/components/climate/climate_ir/#configuration-variables).

## Protocol details

The implementation uses the following IR format:

- Carrier frequency: 38 kHz
- Header mark: 5000 ms
- Header space: 5000 ms
- Bit mark: 500 ms
- Logic 0 space: 500 ms
- Logic 1 space: 1500 ms
- State length: 16 bits
- Bit order: LSB first

### State layout

| Position | Function | Values | Notes |
|----------|----------|--------|-------|
| 0 | Unknown | `0x55` | |
| 1 | Unknown | `0x00` | |
| 2 | Fan / mode / power | `0FFFMMMS` | Fan: `2` = level 1, `3` = level 2, `4` = level 3, `5` = level 4, `6` = auto; mode: `1` = heat, `2` = cool, `3` = dry, `4` = fan, `5` = auto; power: `0` = off, `1` = on |
| 3 | Temperature | `00TTTT00` | `2` = 18°C, ... `14` = 30°C |
| 4-6 | Unknown | `0x00` | |
| 7 | *Eco mode | `00E00000` | `0` = off, `1` = on |
| 8 | Vertical airflow | `01VVV001` | `0` = up, ... `4` = down, `5` = auto |
| 9 | Horizontal airflow | `0001HHH0` | `0` = left, ... `4` = right, `5` = auto |
| 10 | *UV mode / *power mode / *LED display | `0U10P1D0` | `0` = off, `1` = on |
| 11 | Unknown | `0x00` | |
| 12 | *Area airflow | `0000AAAA` | `3` = center, `4` = split, `5` = far left, `6` = near left, `7` = far right, `8` = near right, `9` = auto, `10` = horizontal split, `11` = horizontal focus |
| 13 | Checksum | - | Adds the previous 13 states |
| 14 | Unknown | `0x00` | |
| 15 | Unknown | `0xAA` | |

* Not implemented yet.

## Decode raw IR signals

If you need to inspect or debug the raw protocol:

1. Capture a raw signal and place it in `raw_signal.py`
2. Run the decoder:

```bash
python raw_decoder.py [-b]
```

This displays the decoded signal data for analysis.

## License

MIT
