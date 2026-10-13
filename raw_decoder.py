import argparse
import itertools
from raw_signal import RAW

ONE_SPACE_MIN_MS = 1450
T_MS = 500


def smooth_raw_code(raw_code):
    return round(raw_code / T_MS) * T_MS


def to_bit(raw_code):
    if raw_code < -ONE_SPACE_MIN_MS:
        return '1'
    else:
        return '0'


def decode(raw_code, is_binary):
    data = [to_bit(i) for i in raw_code[2:] if i < 0]

    # group every 8 bits and apply LSB First
    data = [int(''.join(batch)[::-1], 2) for batch in itertools.batched(data, 8)]

    if is_binary:
        out = [f"{byte_:08b}" for byte_ in data]

    else:
        out = [f"{byte_:02X}" for byte_ in data]
    return out


parser = argparse.ArgumentParser("ir_pronto_converter.py")
parser.add_argument("-b", help="display in binary", action='store_true')
args = parser.parse_args()


for key, raw_code in RAW.items():
    result = ' '.join(decode(raw_code, args.b))
    print(f"{result} - {key}")
