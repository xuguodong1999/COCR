import argparse

import torch
import torchvision.transforms as transforms
from PIL import Image

from model import Model
from utils import CTCLabelConverter, AttnLabelConverter


def onnx_export(opt):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model = Model(opt)
    model = torch.nn.DataParallel(model).to(device)
    model.load_state_dict(torch.load(opt.saved_model, map_location=device))
    model.eval()
    model = model.module
    img = Image.open('../workspace/demo.jpg').convert('L')
    # img = Image.open('/home/xgd/source/crnn.pytorch/data/demo2.png').convert('L')
    # img = img.resize([192,32])
    # Image._show(img)
    toTensor = transforms.ToTensor()
    img = toTensor(img)
    img.sub_(0.5).div_(0.5)
    print('img.shape=', img.shape)
    image = img.unsqueeze(0).to(device)
    image = torch.rand(torch.Size([1, 1, 32, 3200])).to(device)
    print('image.shape=', image.shape)
    preds = model(image).permute(1, 0, 2)
    # Select max probabilty (greedy decoding) then decode index to character
    preds_size = torch.IntTensor([preds.size(1)] * 1)
    _, preds_index = preds.max(2)
    # preds_index = preds_index.view(-1)
    preds_str = converter.decode(preds_index, preds_size)
    print('preds_str=', preds_str)
    onnxfile1 = "../workspace/crnn_3200_mix.onnx"
    onnxfile2 = "../workspace/crnn.onnx"
    torch.onnx.export(model, image, onnxfile1)
    # torch.onnx.export(model, image, onnxfile2,
    #                   input_names=['input'],
    #                   output_names=['output'],
    #                   dynamic_axes={'input': [3],
    #                                 'output': [0]})
    # Find total parameters and trainable parameters
    total_params = sum(p.numel() for p in model.parameters())
    print(f'{total_params:,} total parameters.')
    total_trainable_params = sum(
        p.numel() for p in model.parameters() if p.requires_grad)
    print(f'{total_trainable_params:,} training parameters.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--imgH', type=int, default=32, help='the height of the input image')
    parser.add_argument('--imgW', type=int, default=100, help='the width of the input image')
    parser.add_argument('--rgb', action='store_true', help='use rgb input')
    parser.add_argument('--saved_model', required=True, help="path to saved_model to evaluation")
    parser.add_argument('--FeatureExtraction', type=str, required=True, help='FeatureExtraction stage. VGG|RCNN|ResNet')
    parser.add_argument('--character', type=str, default='0123456789abcdefghijklmnopqrstuvwxyz', help='character label')
    parser.add_argument('--sensitive', action='store_true', help='for sensitive character mode')
    parser.add_argument('--Transformation', type=str, required=True, help='Transformation stage. None|TPS')
    parser.add_argument('--SequenceModeling', type=str, required=True, help='SequenceModeling stage. None|BiLSTM')
    parser.add_argument('--Prediction', type=str, required=True, help='Prediction stage. CTC|Attn')
    parser.add_argument('--input_channel', type=int, default=1, help='the number of input channel of Feature extractor')
    parser.add_argument('--output_channel', type=int, default=512,
                        help='the number of output channel of Feature extractor')
    parser.add_argument('--hidden_size', type=int, default=256, help='the size of the LSTM hidden state')
    opt = parser.parse_args()
    opt.num_gpu = torch.cuda.device_count()

    if 'CTC' in opt.Prediction:
        converter = CTCLabelConverter(opt.character)
    else:
        converter = AttnLabelConverter(opt.character)
    opt.num_class = len(converter.character)
    onnx_export(opt)
