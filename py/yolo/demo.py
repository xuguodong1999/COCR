import cv2
import numpy as np
import sys
import tensorflow as tf
from absl import flags

from model import yolo_model

flags.DEFINE_list(
    'anchors', default=[
        [10., 10.], [10., 40.], [40., 10.],
        [30., 30.], [30., 120.], [120., 30.],
        [60., 60.], [60., 80.], [80., 60.]
    ], help=''
)
flags.DEFINE_list(
    'classes', default=[
        'Solid', 'Single', 'Double', 'Triple', 'Dash',
        'Carbon', 'Hydrogen', 'Oxygen', 'N', 'P', 'S'
    ], help=''
)
flags.DEFINE_float('conf_thresh', default=0.5, help='')
global FLAGS
FLAGS = flags.FLAGS.flag_values_dict()


def set_env():
    import tensorflow as tf
    physical_devices = tf.config.list_physical_devices('GPU')
    try:
        tf.config.experimental.set_memory_growth(physical_devices[0], True)
    except:
        print(physical_devices)
    ## for old version
    # for gpu in gpus:
    #     tf.config.experimental.set_memory_growth(gpu, True)
    # from tensorflow.compat.v1 import ConfigProto
    # from tensorflow.compat.v1 import InteractiveSession
    # config = ConfigProto()
    # config.gpu_options.allow_growth = True
    # session = InteractiveSession(config=config)

    import os
    os.environ['CUDA_VISIBLE_DEVICES'] = '0'
    os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'


def add_dimension(imread_obj):
    try:
        return np.expand_dims(imread_obj[:, :, :: -1] / 255.0, axis=0)
    except:
        print('fail to convert. 3 channel image required.')
        sys.exit(-1)


def get_color_list(color_num):
    import colorsys
    hsv_tuples = [(1.0 - x / color_num, 1., 1.) for x in range(color_num)]
    colors = list(map(lambda x: colorsys.hsv_to_rgb(*x), hsv_tuples))
    colors = list(map(lambda x: (int(x[0] * 255), int(x[1] * 255), int(x[2] * 255)), colors))
    return colors


# copy from src: https://github.com/DadianisBidza/NumpyYoloProcess
def handle_yolov3_output(
        yolo_outputs,
        anchors, num_classes,
        width, height,
        score_threshold=0.5,
        iou_threshold=0.5
):
    image_shape = np.array([height, width])
    anchors = np.asarray(anchors)

    def sigmoid(x):
        return 1 / (1 + np.exp(-x))

    def min(x, axis=None, keepdims=False):
        if isinstance(axis, list):
            axis = tuple(axis)
        return np.min(x, axis=axis, keepdims=keepdims)

    def head(feats, anchors, num_classes, input_shape, calc_loss=False):
        num_anchors = len(anchors)
        anchors_tensor = np.reshape(anchors, [1, 1, 1, num_anchors, 2])
        grid_shape = feats.shape[1:3]
        grid_y = np.tile(
            np.reshape(
                np.arange(0, stop=grid_shape[0]),
                [-1, 1, 1, 1]), [1, grid_shape[1], 1, 1]
        )
        grid_x = np.tile(
            np.reshape(
                np.arange(0, stop=grid_shape[1]),
                [1, -1, 1, 1]), [grid_shape[0], 1, 1, 1]
        )
        grid = np.concatenate([grid_x, grid_y], axis=-1)
        grid = grid.astype(feats.dtype)
        feats = np.reshape(
            feats, [-1, grid_shape[0], grid_shape[1], num_anchors, num_classes + 5]
        )
        box_xy = (sigmoid(
            feats[..., :2]
        ) + grid) / np.array(grid_shape[::-1]).astype(feats.dtype)
        box_wh = np.exp(
            feats[..., 2:4]
        ) * anchors_tensor / np.array(input_shape[::-1]).astype(feats.dtype)
        box_confidence = sigmoid(feats[..., 4:5])
        box_class_probs = sigmoid(feats[..., 5:])
        if calc_loss == True:
            return grid, feats, box_xy, box_wh
        return box_xy, box_wh, box_confidence, box_class_probs

    def correct_boxes(box_xy, box_wh, input_shape, image_shape):
        box_yx = box_xy[..., ::-1]
        box_hw = box_wh[..., ::-1]
        input_shape = input_shape.astype(box_yx.dtype)
        image_shape = image_shape.astype(box_yx.dtype)
        new_shape = np.round(image_shape * min(input_shape / image_shape))
        offset = (input_shape - new_shape) / 2. / input_shape
        scale = input_shape / new_shape
        box_yx = (box_yx - offset) * scale
        box_hw *= scale
        box_mins = box_yx - (box_hw / 2.)
        box_maxes = box_yx + (box_hw / 2.)
        boxes = np.concatenate([
            box_mins[..., 0:1],
            box_mins[..., 1:2],
            box_maxes[..., 0:1],
            box_maxes[..., 1:2]
        ], axis=-1)
        boxes = boxes * np.concatenate([image_shape, image_shape], axis=-1)
        return boxes

    def boxes_and_scores(feats, anchors, num_classes, input_shape, image_shape):
        box_xy, box_wh, box_confidence, box_class_probs = head(
            feats, anchors, num_classes, input_shape
        )
        boxes = correct_boxes(box_xy, box_wh, input_shape, image_shape)
        boxes = np.reshape(boxes, [-1, 4])
        box_scores = box_confidence * box_class_probs
        box_scores = np.reshape(box_scores, [-1, num_classes])
        return boxes, box_scores

    def nms(boxes, scores, overlapThresh):
        if len(boxes) == 0:
            return []
        if boxes.dtype.kind == "i":
            boxes = boxes.astype("float")
        pick = []
        x1 = boxes[:, 0]
        y1 = boxes[:, 1]
        x2 = boxes[:, 2]
        y2 = boxes[:, 3]
        area = (x2 - x1 + 1) * (y2 - y1 + 1)
        idxs = np.argsort(scores)
        while len(idxs) > 0:
            last = len(idxs) - 1
            i = idxs[last]
            pick.append(i)
            xx1 = np.maximum(x1[i], x1[idxs[:last]])
            yy1 = np.maximum(y1[i], y1[idxs[:last]])
            xx2 = np.minimum(x2[i], x2[idxs[:last]])
            yy2 = np.minimum(y2[i], y2[idxs[:last]])
            w = np.maximum(0, xx2 - xx1 + 1)
            h = np.maximum(0, yy2 - yy1 + 1)
            overlap = (w * h) / area[idxs[:last]]
            idxs = np.delete(
                idxs,
                np.concatenate(([last], np.where(overlap > overlapThresh)[0]))
            )
        return pick

    num_layers = len(yolo_outputs)
    anchor_mask = [[6, 7, 8], [3, 4, 5], [0, 1, 2]]
    # anchor_mask = [[0, 1, 2], [3, 4, 5], [6, 7, 8]]
    input_shape = np.array(yolo_outputs[0].shape[1:3]) * 32
    boxes = []
    box_scores = []
    for l in range(num_layers):
        _boxes, _box_scores = boxes_and_scores(
            yolo_outputs[l], anchors[anchor_mask[l]], num_classes, input_shape, image_shape
        )
        boxes.append(_boxes)
        box_scores.append(_box_scores)
    boxes = np.concatenate(boxes, axis=0)
    box_scores = np.concatenate(box_scores, axis=0)
    mask = box_scores >= score_threshold
    boxes_ = []
    scores_ = []
    classes_ = []
    for c in range(num_classes):
        class_boxes = boxes[mask[:, c]]
        class_box_scores = box_scores[:, c][mask[:, c]]
        nms_index = nms(class_boxes, class_box_scores, iou_threshold)
        class_boxes = np.take(class_boxes, nms_index, axis=0)
        class_box_scores = np.take(class_box_scores, nms_index, axis=0)
        classes = c * np.ones(class_box_scores.shape, dtype=np.int32)
        boxes_.append(class_boxes)
        scores_.append(class_box_scores)
        classes_.append(classes)
    boxes_ = np.concatenate(boxes_, axis=0)
    scores_ = np.concatenate(scores_, axis=0)
    classes_ = np.concatenate(classes_, axis=0)
    return boxes_, scores_, classes_


if __name__ == '__main__':
    set_env()
    np.set_printoptions(suppress=True, threshold=np.inf)
    # input_size = 320
    input_layer = tf.keras.layers.Input([None, None, 3])

    model, pred_all = yolo_model(input_layer, False)
    model.summary()
    model.load_weights('./yolo_epoch_104.h5', True, True)
    img = cv2.imread('/media/xgd/C4DC99C6DB75A30E/datasets/soso.11.big/JPEGImages/1dd0.jpg')
    output = model.predict(add_dimension(img))
    # output = [output[2], output[1], output[0]]
    for i in range(len(output)):
        print(output[i].shape)
    boxes, scores, classes = handle_yolov3_output(
        output, FLAGS['anchors'], len(FLAGS['classes']), img.shape[1], img.shape[0]
    )
    colors = get_color_list(len(FLAGS['classes']))
    for i in range(len(boxes)):
        p = boxes[i].astype(int)
        color = colors[int(classes[i])]
        cv2.rectangle(img, (p[1], p[0]), (p[3], p[2]), color, 2)
        cv2.putText(
            img, FLAGS['classes'][int(classes[i])], (p[1] - 15, p[2] + 15),
            cv2.FONT_HERSHEY_PLAIN, 1.2, color, 2
        )
    cv2.imshow('', img)
    cv2.waitKey(0)
