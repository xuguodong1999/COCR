import tensorflow as tf

from config import *


def box_iou(box_1, box_2):
    """
    @param box_1: (a, b, ..., 4)
    @param box_2: (A, B, ..., 4)
        x:X is 1:n or n:n or n:1
    @return (max(a,A), max(b,B), ...)
    ex) (4,):(3,4) -> (3,)
        (2,1,4):(2,3,4) -> (2,3)
    """
    boxes_1_area = box_1[..., 2] * box_1[..., 3]
    boxes_2_area = box_2[..., 2] * box_2[..., 3]

    boxes_1_pts = tf.concat([
        box_1[..., :2] - box_1[..., 2:] * 0.5,
        box_1[..., :2] + box_1[..., 2:] * 0.5,
    ], axis=-1, )
    boxes_2_pts = tf.concat([
        box_2[..., :2] - box_2[..., 2:] * 0.5,
        box_2[..., :2] + box_2[..., 2:] * 0.5,
    ], axis=-1, )

    left_up = tf.maximum(boxes_1_pts[..., :2], boxes_2_pts[..., :2])
    right_down = tf.minimum(boxes_1_pts[..., 2:], boxes_2_pts[..., 2:])

    inter_section = tf.maximum(right_down - left_up, 0.0)
    inter_area = inter_section[..., 0] * inter_section[..., 1]

    union_area = boxes_1_area + boxes_2_area - inter_area

    iou = tf.math.divide_no_nan(inter_area, union_area)

    return iou


def box_giou(boxes_1, boxes_2):
    """
    Generalized IoU
    @param boxes_1: (a, b, ..., 4)
    @param boxes_2: (A, B, ..., 4)
        x:X is 1:n or n:n or n:1
    @return (max(a,A), max(b,B), ...)
    ex) (4,):(3,4) -> (3,)
        (2,1,4):(2,3,4) -> (2,3)
    """
    boxes_1_area = boxes_1[..., 2] * boxes_1[..., 3]
    boxes_2_area = boxes_2[..., 2] * boxes_2[..., 3]

    boxes_1_pts = tf.concat([
        boxes_1[..., :2] - boxes_1[..., 2:] * 0.5,
        boxes_1[..., :2] + boxes_1[..., 2:] * 0.5,
    ], axis=-1, )
    boxes_2_pts = tf.concat([
        boxes_2[..., :2] - boxes_2[..., 2:] * 0.5,
        boxes_2[..., :2] + boxes_2[..., 2:] * 0.5,
    ], axis=-1, )
    # print(boxes_1_pts.shape,boxes_2_pts.shape)
    left_up = tf.maximum(boxes_1_pts[..., :2], boxes_2_pts[..., :2])
    right_down = tf.minimum(boxes_1_pts[..., 2:], boxes_2_pts[..., 2:])

    inter_section = tf.maximum(right_down - left_up, 0.0)
    inter_area = inter_section[..., 0] * inter_section[..., 1]

    union_area = boxes_1_area + boxes_2_area - inter_area

    iou = tf.math.divide_no_nan(inter_area, union_area)

    enclose_left_up = tf.minimum(boxes_1_pts[..., :2], boxes_2_pts[..., :2])
    enclose_right_down = tf.maximum(
        boxes_1_pts[..., 2:], boxes_2_pts[..., 2:])

    enclose_section = enclose_right_down - enclose_left_up
    enclose_area = enclose_section[..., 0] * enclose_section[..., 1]

    giou = iou - tf.math.divide_no_nan(enclose_area - union_area, enclose_area)

    return giou


def box_ciou(boxes_1, boxes_2):
    """
    Complete IoU
    @param boxes_1: (a, b, ..., 4)
    @param boxes_2: (A, B, ..., 4)
        x:X is 1:n or n:n or n:1
    @return (max(a,A), max(b,B), ...)
    ex) (4,):(3,4) -> (3,)
        (2,1,4):(2,3,4) -> (2,3)
    """
    boxes_1_area = boxes_1[..., 2] * boxes_1[..., 3]
    boxes_2_area = boxes_2[..., 2] * boxes_2[..., 3]

    boxes_1_pts = tf.concat([
        boxes_1[..., :2] - boxes_1[..., 2:] * 0.5,
        boxes_1[..., :2] + boxes_1[..., 2:] * 0.5,
    ], axis=-1, )
    boxes_2_pts = tf.concat([
        boxes_2[..., :2] - boxes_2[..., 2:] * 0.5,
        boxes_2[..., :2] + boxes_2[..., 2:] * 0.5,
    ], axis=-1, )

    left_up = tf.maximum(boxes_1_pts[..., :2], boxes_2_pts[..., :2])
    right_down = tf.minimum(boxes_1_pts[..., 2:], boxes_2_pts[..., 2:])

    inter_section = tf.maximum(right_down - left_up, 0.0)
    inter_area = inter_section[..., 0] * inter_section[..., 1]

    union_area = boxes_1_area + boxes_2_area - inter_area

    iou = tf.math.divide_no_nan(inter_area, union_area)

    enclose_left_up = tf.minimum(boxes_1_pts[..., :2], boxes_2_pts[..., :2])
    enclose_right_down = tf.maximum(
        boxes_1_pts[..., 2:], boxes_2_pts[..., 2:])

    enclose_section = enclose_right_down - enclose_left_up
    c_2 = enclose_section[..., 0] ** 2 + enclose_section[..., 1] ** 2
    center_diagonal = boxes_2[..., :2] - boxes_1[..., :2]
    rho_2 = center_diagonal[..., 0] ** 2 + center_diagonal[..., 1] ** 2
    diou = iou - tf.math.divide_no_nan(rho_2, c_2)
    v = ((tf.math.atan(
        tf.math.divide_no_nan(boxes_1[..., 2], boxes_1[..., 3])) - tf.math.atan(
        tf.math.divide_no_nan(boxes_2[..., 2], boxes_2[..., 3]))) * 2 / np.pi) ** 2
    alpha = tf.math.divide_no_nan(v, 1 - iou + v)
    ciou = diou - alpha * v
    return ciou


def nms(boxes, iou_threshold, sigma=0.3, method='nms'):
    """
    :param boxes: (xmin, ymin, xmax, ymax, score, class)

    Note: soft-nms, https://arxiv.org/pdf/1704.04503.pdf
          https://github.com/bharatsingh430/soft-nms
    """
    classes_in_img = list(set(boxes[:, 5]))
    best_boxes = []

    for cls in classes_in_img:
        cls_mask = (boxes[:, 5] == cls)
        cls_boxes = boxes[cls_mask]

        while len(cls_boxes) > 0:
            max_ind = np.argmax(cls_boxes[:, 4])
            best_box = cls_boxes[max_ind]
            best_boxes.append(best_box)
            cls_boxes = np.concatenate([cls_boxes[: max_ind], cls_boxes[max_ind + 1:]])
            iou = box_iou(best_box[np.newaxis, :4], cls_boxes[:, :4])
            weight = np.ones((len(iou),), dtype=np.float32)

            assert method in ['nms', 'soft-nms']

            if method == 'nms':
                iou_mask = iou > iou_threshold
                weight[iou_mask] = 0.0

            if method == 'soft-nms':
                weight = np.exp(-(1.0 * iou ** 2 / sigma))

            cls_boxes[:, 4] = cls_boxes[:, 4] * weight
            score_mask = cls_boxes[:, 4] > 0.
            cls_boxes = cls_boxes[score_mask]

    return best_boxes


def filter_boxes(box_xy_wh, scores, score_threshold=0.4, input_shape=tf.constant([416, 416])):
    scores_max = tf.math.reduce_max(scores, axis=-1)

    mask = scores_max >= score_threshold
    class_boxes = tf.boolean_mask(box_xy_wh, mask)
    pred_conf = tf.boolean_mask(scores, mask)
    class_boxes = tf.reshape(class_boxes, [tf.shape(scores)[0], -1, tf.shape(class_boxes)[-1]])
    pred_conf = tf.reshape(pred_conf, [tf.shape(scores)[0], -1, tf.shape(pred_conf)[-1]])

    box_xy, box_wh = tf.split(class_boxes, (2, 2), axis=-1)

    input_shape = tf.cast(input_shape, dtype=tf.float32)

    box_yx = box_xy[..., ::-1]
    box_hw = box_wh[..., ::-1]

    box_mins = (box_yx - (box_hw / 2.)) / input_shape
    box_maxes = (box_yx + (box_hw / 2.)) / input_shape
    boxes = tf.concat([
        box_mins[..., 0:1],  # y_min
        box_mins[..., 1:2],  # x_min
        box_maxes[..., 0:1],  # y_max
        box_maxes[..., 1:2]  # x_max
    ], axis=-1)
    # return tf.concat([boxes, pred_conf], axis=-1)
    return (boxes, pred_conf)


def compute_loss(pred, conv, label, boxes, i=0):
    # print(pred.shape,label.shape)
    conv_shape = tf.shape(conv)
    batch_size = conv_shape[0]
    out_w, out_h = conv_shape[1], conv_shape[2]
    in_w = tf.cast(STRIDES[i] * out_w, tf.float32)
    in_h = tf.cast(STRIDES[i] * out_h, tf.float32)
    conv = tf.reshape(conv, (batch_size, out_w, out_h, 3, 5 + NUM_CLASSES))

    conv_raw_conf = conv[:, :, :, :, 4:5]
    conv_raw_prob = conv[:, :, :, :, 5:]

    pred_xy_wh = pred[:, :, :, :, 0:4]
    pred_conf = pred[:, :, :, :, 4:5]

    label_xy_wh = label[:, :, :, :, 0:4]
    respond_box = label[:, :, :, :, 4:5]
    label_prob = label[:, :, :, :, 5:]

    giou = tf.expand_dims(box_giou(pred_xy_wh, label_xy_wh), axis=-1)

    box_loss_scale = 2.0 - 1.0 * label_xy_wh[:, :, :, :, 2:3] * label_xy_wh[:, :, :, :, 3:4] / (in_w * in_h)
    giou_loss = respond_box * box_loss_scale * (1.0 - giou)

    iou = box_iou(pred_xy_wh[:, :, :, :, np.newaxis, :], boxes[:, np.newaxis, np.newaxis, np.newaxis, :, :])
    max_iou = tf.expand_dims(tf.reduce_max(iou, axis=-1), axis=-1)

    respond_bgd = (1.0 - respond_box) * tf.cast(max_iou < LOSS_IOU_THRESH, tf.float32)

    conf_focal = tf.pow(respond_box - pred_conf, 2)

    conf_loss = conf_focal * (
            respond_box * tf.nn.sigmoid_cross_entropy_with_logits(
        labels=respond_box, logits=conv_raw_conf) + respond_bgd *
            tf.nn.sigmoid_cross_entropy_with_logits(
                labels=respond_box, logits=conv_raw_conf))

    prob_loss = respond_box * tf.nn.sigmoid_cross_entropy_with_logits(
        labels=label_prob, logits=conv_raw_prob)

    giou_loss = tf.reduce_mean(tf.reduce_sum(giou_loss, axis=[1, 2, 3, 4]))
    conf_loss = tf.reduce_mean(tf.reduce_sum(conf_loss, axis=[1, 2, 3, 4]))
    prob_loss = tf.reduce_mean(tf.reduce_sum(prob_loss, axis=[1, 2, 3, 4]))

    return giou_loss, conf_loss, prob_loss
