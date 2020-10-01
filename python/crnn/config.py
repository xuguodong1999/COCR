import argparse

parser = argparse.ArgumentParser()
parser.add_argument(
    '-cn', '--crnn_name', type=str, required=False, default='crnn',
    help='The path of training data annnotation file.')
args = parser.parse_args()
