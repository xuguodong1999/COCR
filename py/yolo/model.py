import os

import tensorflow as tf
from tensorflow.keras import Input
from tensorflow.keras.layers import Conv2D, BatchNormalization, \
    SeparableConv2D, UpSampling2D, LeakyReLU, Concatenate, ZeroPadding2D
from tensorflow.keras.utils import plot_model

from config import *
from efficientnet import EfficientNetB0


def yolo_backbone(img_tensor_yb):
    model = EfficientNetB0(
        include_top=False, weights='./efficientnetb0_notop.h5',
        input_tensor=img_tensor_yb, input_shape=(None, None, 3)
    )
    feature_32x = model.get_layer('top_activation').output  # 1280
    feature_16x = model.get_layer('block6a_expand_activation').output  # 672
    feature_8x = model.get_layer('block4a_expand_activation').output  # 240

    # model = MobileNetV2(alpha=1.0, include_top=False,
    #                     weights='imagenet',
    #                     input_tensor=img_tensor_yb)
    # feature_32x = model.get_layer('out_relu').output  # 13
    # feature_16x = model.get_layer('block_13_expand_relu').output  # 26
    # feature_8x = model.get_layer('block_6_expand_relu').output  # 52

    # model.summary()
    # model.save('/tmp/enetb0.h5')
    return feature_32x, feature_16x, feature_8x, model


# # 在人为的层冻结和推理状态下，都预期不更新 BN 层参数，在应用层打上这个补丁
# class HackBatchNormalization(BatchNormalization):
#     def call(self, x, training=False):
#         if not training:
#             training = tf.constant(False)
#         training = tf.logical_and(training, self.trainable)
#         return super().call(x, training)


def yolo_head_separable_conv(input_layer, kernel_size, filters, down_sample=False):
    if down_sample:
        input_layer = ZeroPadding2D(((1, 0), (1, 0)))(input_layer)
        padding = 'valid'
        strides = 2
    else:
        padding = 'same'
        strides = 1
    out_layer = SeparableConv2D(
        filters=filters, kernel_size=kernel_size,
        strides=strides, padding=padding, use_bias=False,
        kernel_regularizer=tf.keras.regularizers.l2(0.0005),
        kernel_initializer=tf.random_normal_initializer(stddev=0.01),
        bias_initializer=tf.constant_initializer(0.))(input_layer)
    # out_layer = HackBatchNormalization()(out_layer)
    out_layer = BatchNormalization()(out_layer)
    out_layer = LeakyReLU(alpha=0.1)(out_layer)
    # out_layer = tf.nn.leaky_relu(out_layer, alpha=0.1)
    return out_layer


def yolo_up_sample(input_layer):
    return UpSampling2D(interpolation='bilinear')(input_layer)
    # return tf.image.resize(
    #     input_layer, method='bilinear',
    #     size=(input_layer.shape[1] * 2, input_layer.shape[2] * 2))


def yolo_head_conv(input_layer, kernel_size, filters, activate_type='leaky'):
    strides = 1
    padding = 'same'
    out_layer = Conv2D(
        filters=filters, kernel_size=kernel_size,
        strides=strides, padding=padding, use_bias=False,
        kernel_regularizer=tf.keras.regularizers.l2(0.0005),
        kernel_initializer=tf.random_normal_initializer(stddev=0.01),
        bias_initializer=tf.constant_initializer(0.))(input_layer)
    # out_layer = HackBatchNormalization()(out_layer)
    out_layer = BatchNormalization()(out_layer)

    out_layer = LeakyReLU(alpha=0.1)(out_layer)
    # out_layer = tf.nn.leaky_relu(out_layer, alpha=0.1)

    return out_layer


def yolo_head_regression(input_layer, kernel_size, filters):
    strides = 1
    padding = 'same'
    out_layer = Conv2D(
        filters=filters, kernel_size=kernel_size,
        strides=strides, padding=padding, use_bias=True,
        kernel_regularizer=tf.keras.regularizers.l2(0.0005),
        kernel_initializer=tf.random_normal_initializer(stddev=0.01),
        bias_initializer=tf.constant_initializer(0.))(input_layer)
    return out_layer


def decode_train(pred, i=0):
    out_w = TRAIN_INPUT_WIDTH // pow(2, i + 3)
    out_h = TRAIN_INPUT_HEIGHT // pow(2, i + 3)
    out_batch = tf.shape(pred)[0]
    xy_grid = tf.meshgrid(tf.range(out_h), tf.range(out_w))
    xy_grid = tf.expand_dims(tf.stack(xy_grid, axis=-1), axis=2)  # [gx, gy, 1, 2]
    xy_grid = tf.tile(tf.expand_dims(xy_grid, axis=0), [out_batch, 1, 1, 3, 1])
    xy_grid = tf.cast(xy_grid, tf.float32)

    pred = tf.reshape(tensor=pred, shape=(out_batch, out_w, out_h, 3, 5 + NUM_CLASSES))

    pred_xy, pred_wh, pred_conf, pred_prob = tf.split(pred, (2, 2, 1, NUM_CLASSES), axis=-1)
    pred_xy = N_STRIDES[i] * (tf.sigmoid(pred_xy) + xy_grid)
    pred_wh = tf.exp(pred_wh) * N_ANCHORS[i]
    pred_xy_wh = tf.concat([pred_xy, pred_wh], axis=-1)
    pred_conf = tf.sigmoid(pred_conf)
    pred_prob = tf.sigmoid(pred_prob)
    return tf.concat([pred_xy_wh, pred_conf, pred_prob], axis=-1)


def yolo_model(img_tensor_ym, is_train=True):
    # 1280 672 240
    # 1280 672 240
    d32x, d16x, d8x, model = yolo_backbone(img_tensor_ym)
    for i in range(len(model.layers)):
        model.layers[i].trainable = False

    route_0 = d32x
    d32x = yolo_head_conv(d32x, 1, 192)  ### 1280
    route = yolo_up_sample(d32x)

    d16x = yolo_head_conv(d16x, 1, 192)  ### 672
    # route = tf.concat([d16x, route], axis=-1)  # [192 + 288]
    route = Concatenate()([d16x, route])

    route = yolo_head_conv(route, 1, 192)
    route = yolo_head_separable_conv(route, 3, 288)
    route = yolo_head_conv(route, 1, 192)

    route_1 = route
    route = yolo_head_conv(route, 1, 128)
    route = yolo_up_sample(route)
    d8x = yolo_head_conv(d8x, 1, 128)  ### 240
    # route = tf.concat([d8x, route], axis=-1)
    route = Concatenate()([d8x, route])

    route = yolo_head_conv(route, 1, 128)
    route = yolo_head_separable_conv(route, 3, 192)
    route = yolo_head_conv(route, 1, 128)

    route_2 = route
    route = yolo_head_separable_conv(route, 3, 192)
    pred_big_d8x = yolo_head_regression(route, 1, 3 * (NUM_CLASSES + 5))

    route = yolo_head_separable_conv(route_2, 3, 192, down_sample=True)
    # route = tf.concat([route, route_1], axis=-1)
    route = Concatenate()([route, route_1])

    route = yolo_head_conv(route, 1, 192)
    route = yolo_head_separable_conv(route, 3, 288)
    route = yolo_head_conv(route, 1, 192)

    route_3 = route
    route = yolo_head_separable_conv(route, 3, 288)
    pred_medium_d16x = yolo_head_regression(route, 1, 3 * (NUM_CLASSES + 5))

    route = yolo_head_separable_conv(route_3, 3, 288, down_sample=True)
    # route = tf.concat([route_0, route], axis=-1)
    route = Concatenate()([route_0, route])

    route = yolo_head_conv(route, 1, 288)
    route = yolo_head_separable_conv(route, 3, 432)
    route = yolo_head_conv(route, 1, 288)

    route = yolo_head_separable_conv(route, 3, 432)
    pred_small_d32x = yolo_head_regression(route, 1, 3 * (NUM_CLASSES + 5))

    pred_all = [pred_big_d8x, pred_medium_d16x, pred_small_d32x]

    model = tf.keras.Model(img_tensor_ym, pred_all, name='yolo_no_postprocess')
    # model.save('/tmp/yolo_no_postprocess.h5')  # 13 26 52
    out_tensor = []
    for i, pred_i in enumerate(pred_all):  # 8 16 32
        pred_i_split = decode_train(pred_i, i)
        out_tensor.append(pred_i)
        out_tensor.append(pred_i_split)
    model = tf.keras.Model(img_tensor_ym, out_tensor, name='yolo')
    plot_model(model, to_file='/tmp/yolo.png', show_shapes=True, expand_nested=True)
    # model.save('/tmp/yolo.h5')  # 13 26 52
    return model, pred_all


if __name__ == '__main__':
    os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
    img_tensor = Input(shape=(TRAIN_INPUT_WIDTH, TRAIN_INPUT_HEIGHT, 3))
    # img_tensor = Input(shape=(None, None, 3))
    yolo_model(img_tensor)
    pass
