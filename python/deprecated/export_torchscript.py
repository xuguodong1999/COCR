#!/usr/bin/env python3
# -*- coding:utf-8 -*-
# Copyright (c) Megvii, Inc. and its affiliates.

import argparse
import os

import torch
from loguru import logger


def make_parser():
    parser = argparse.ArgumentParser("YOLOX torchscript deploy")
    parser.add_argument(
        "--output-name", type=str, default="torchscript.pt", help="output name of models"
    )
    parser.add_argument("--batch-size", type=int, default=1, help="batch size")
    parser.add_argument(
        "-f",
        "--exp_file",
        default=None,
        type=str,
        help="expriment description file",
    )
    parser.add_argument("-i", "--image_channel", default=3, type=int, help="image channel")
    parser.add_argument("-expn", "--experiment-name", type=str, default=None)
    parser.add_argument("-n", "--name", type=str, default=None, help="model name")
    parser.add_argument("-c", "--ckpt", default=None, type=str, help="ckpt path")
    parser.add_argument(
        "opts",
        help="Modify config options using the command-line",
        default=None,
        nargs=argparse.REMAINDER,
    )

    return parser


@logger.catch
def main():
    args = make_parser().parse_args()
    logger.info("args value: {}".format(args))
    from yolox.exp.yolox_nano_soso8 import Exp
    exp = Exp()
    exp.merge(args.opts)

    if not args.experiment_name:
        args.experiment_name = exp.exp_name

    model = exp.get_model()
    if args.ckpt is None:
        file_name = os.path.join(exp.output_dir, args.experiment_name)
        ckpt_file = os.path.join(file_name, "best_ckpt.pth")
    else:
        ckpt_file = args.ckpt

    # load the model state dict
    ckpt = torch.load(ckpt_file, map_location="cpu")

    model.eval()
    if "model" in ckpt:
        ckpt = ckpt["model"]
    model.load_state_dict(ckpt)
    model.head.decode_in_inference = False

    logger.info("loading checkpoint done.")
    dummy_input = torch.randn(args.batch_size, args.image_channel, exp.test_size[0], exp.test_size[1])

    mod = torch.jit.trace(model, dummy_input)
    mod.save(args.output_name)
    logger.info("generated torchscript model named {}".format(args.output_name))


if __name__ == "__main__":
    main()
