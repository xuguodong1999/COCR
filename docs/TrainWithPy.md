# with yolox

## train

`python train.py -d 1 -b 48 --fp16`

## resume

`python train.py -d 1 -b 48 --fp16 --resume -c ./out/yolox_nano_soso8/latest_ckpt.pth`

## export

`python export_torchscript.py -c ./out/yolox_nano_soso8/latest_ckpt.pth --output-name ./out/yolox_nano_soso8/latest_ts.pth`

## demo

`python demo.py image --path ../../../assets/img/9.jpg -c out/yolox_nano_soso8/latest_ckpt.pth --save_result`

# with crnn

## lmdb-train

`python create_lmdb_dataset.py ~/datasets/crnn-lmdb/train/JPEGImages ~/datasets/crnn-lmdb/train/labels.txt ./data/train`

## lmdb-test

`python create_lmdb_dataset.py ~/datasets/crnn-lmdb/test/JPEGImages ~/datasets/crnn-lmdb/test/labels.txt ./data/test`

## train

`python crnn_train.py --train_data='/home/xgd/datasets/leafxy-text-57/train' --valid_data='/home/xgd/datasets/leafxy-text-57/test' --model_top_dir='./out' --FeatureExtraction='CSP' --sensitive --select_data / --batch_ratio 1 --character='-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty' --imgH=32 --imgW=192 --input_channel=1 --output_channel=-1 --hidden_size=128 --batch_size=512 --workers=4 --num_iter=100000 --valInterval=5000 --exp_name='crnn'`

## onnx export

`python onnx_export.py --FeatureExtraction='VGG' --character='-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty' --imgH=32 --imgW=3200 --input_channel=1 --output_channel=256 --hidden_size=256 --saved_model='../workspace/VGG-BiLSTM-baidu_ctc_mixFont2/best_accuracy.pth'`

## ncnn convert

`~/shared/ncnn/bin/onnx2ncnn crnn_3200_mix.onnx vgg_lstm_57.param vgg_lstm_57.bin`

`~/shared/ncnn/bin/onnx2ncnn crnn_3200_mix.onnx vgg_lstm_57.param vgg_lstm_57.bin`

`~/shared/ncnn/bin/ncnnoptimize vgg_lstm_57.param vgg_lstm_57.bin vgg_lstm_57_fp16.param vgg_lstm_57_fp16.bin 1`