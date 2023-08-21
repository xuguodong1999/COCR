"""
Copyright (c) 2019-present NAVER Corp.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import torch.nn as nn

from yolox.layers.bilstm import BidirectionalLSTM
from yolox.layers.vgg import VGG
from yolox.models.darknet import CSPDarknet


class Model(nn.Module):

    def __init__(self, opt):
        super(Model, self).__init__()

        """ FeatureExtraction """
        if opt.FeatureExtraction == 'VGG':
            self.FeatureExtraction = VGG(opt.input_channel, opt.output_channel, depthwise=True)
        elif opt.FeatureExtraction == 'CSP':
            self.FeatureExtraction = CSPDarknet(
                0.33,
                0.25,
                out_features=("dark5"),
                depthwise=True,
                act="silu",
                image_channel=1,
                use_single_out=True,
                use_focus=False,
                crnn_last=True
            )
            output_channel = self.FeatureExtraction.get_out_channel()
            print('use CSPDarknet and revert output_channel from {} to {}'.format(opt.output_channel, output_channel))
            opt.output_channel = output_channel
        else:
            raise Exception('No FeatureExtraction module specified')
        self.FeatureExtraction_output = opt.output_channel  # int(imgH/16-1) * 512
        self.AdaptiveAvgPool = nn.AdaptiveAvgPool2d((opt.output_channel, 1))  # Transform final (imgH/16-1) -> 1

        """ Sequence modeling"""
        self.SequenceModeling = nn.Sequential(
            BidirectionalLSTM(self.FeatureExtraction_output, opt.hidden_size, opt.hidden_size),
            BidirectionalLSTM(opt.hidden_size, opt.hidden_size, opt.hidden_size))
        self.SequenceModeling_output = opt.hidden_size

        """ Prediction """
        self.Prediction = nn.Linear(self.SequenceModeling_output, opt.num_class)

    def forward(self, input, text='', is_train=True):
        """ Feature extraction stage """
        visual_feature = self.FeatureExtraction(input)  # 0 1 2 3
        # print('self.FeatureExtraction: ', sum(m.numel() for m in self.FeatureExtraction.parameters()))
        visual_feature = self.AdaptiveAvgPool(visual_feature.permute(0, 3, 1, 2))  # [b, c, h, w] -> [b, w, c, h]
        visual_feature = visual_feature.squeeze(3)

        """ Sequence modeling stage """
        contextual_feature = self.SequenceModeling(visual_feature)
        """ Prediction stage """
        prediction = self.Prediction(contextual_feature.contiguous())
        prediction = prediction.permute(1, 0, 2)
        return prediction
