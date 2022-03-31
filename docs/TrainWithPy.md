# with yolox

## train

`python train.py -d 1 -b 48 --fp16`

## resume

`python train.py -d 1 -b 48 --fp16 --resume -c ./out/yolox_nano_soso8/latest_ckpt.pth`

## export

`python export_torchscript.py -c ./out/yolox_nano_soso8/latest_ckpt.pth --output-name ./out/yolox_nano_soso8/latest_ts.pth`

# with crnn