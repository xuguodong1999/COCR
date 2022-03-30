#!/usr/bin/env python3
# -*- coding:utf-8 -*-
# Copyright (c) Megvii, Inc. and its affiliates.
import os

import torch.nn as nn

from ...exp import Exp as MyExp


class Exp(MyExp):
    def __init__(self):
        super(Exp, self).__init__()
        self.depth = 0.33
        self.width = 0.25
        self.input_size = (640, 640)
        self.random_size = (10, 20)
        self.test_size = (640, 640)
        self.exp_name = os.path.split(os.path.realpath(__file__))[1].split(".")[0]
        # soso 8
        self.num_classes = 8
        # scale
        self.multiscale_range = 2
        # dir
        self.data_dir = os.path.join(os.path.expanduser('~'), 'datasets/soso-obj')
        # image dir
        self.img_dir_name = 'JPEGImages'
        # name of annotation file for training
        self.train_ann = "soso8-coco-train.json"  # annotations/soso8-coco-train.json
        # name of annotation file for evaluation
        self.val_ann = "soso8-coco-test.json"
        # name of annotation file for testing
        self.test_ann = "soso8-coco-test.json"
        # disable mixup
        self.mixup_prob = -1
        self.flip_prob = -1
        self.hsv_prob = -1
        self.shear = 0
        self.translate = 0
        self.degrees = 0
        self.mosaic_prob = -1
        self.enable_mixup = False

    def get_model(self, sublinear=False):

        def init_yolo(M):
            for m in M.modules():
                if isinstance(m, nn.BatchNorm2d):
                    m.eps = 1e-3
                    m.momentum = 0.03

        if "model" not in self.__dict__:
            from ...models import YOLOX, YOLOPAFPN, YOLOXHead
            in_channels = [256, 512, 1024]
            # NANO model use depthwise = True, which is main difference.
            backbone = YOLOPAFPN(
                self.depth, self.width, in_channels=in_channels,
                act=self.act, depthwise=True, image_channel=1,
            )
            head = YOLOXHead(
                self.num_classes, self.width, in_channels=in_channels,
                act=self.act, depthwise=True
            )
            self.model = YOLOX(backbone, head)

        self.model.apply(init_yolo)
        self.model.head.initialize_biases(1e-2)
        return self.model
