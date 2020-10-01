#! /usr/bin/env python
# coding=utf-8

import os

import cv2

from loss import *


def img_proc(img: np.array, new_size, gt_boxes_with_id=None):
    (tmp_w, tmp_h) = new_size
    (in_h, in_w, _) = img.shape
    scale_k: float = min(tmp_w / in_w, tmp_h / in_h)
    (new_w, new_h) = int(scale_k * in_w), int(scale_k * in_h)
    img_resized: np.array = cv2.resize(img, (new_w, new_h))
    (border_w, border_h) = (tmp_w - new_w) // 2, (tmp_h - new_h) // 2
    img_padded: np.array = cv2.copyMakeBorder(
        img_resized, tmp_h - new_h - border_h, border_h, tmp_w - new_w - border_w, border_w,
        cv2.BORDER_CONSTANT, value=(255, 255, 255)) / 255.
    img_padded = np.reshape(img_padded, [tmp_w, tmp_h, 3])
    # cv2.imshow('',img_padded)
    # cv2.waitKey(0)

    if gt_boxes_with_id is None:
        return img_padded
    else:
        gt_boxes_with_id[:, [1, 3]] = gt_boxes_with_id[:, [1, 3]] * scale_k + border_w
        gt_boxes_with_id[:, [2, 4]] = gt_boxes_with_id[:, [2, 4]] * scale_k + border_h
        return img_padded, gt_boxes_with_id


class Dataset(object):
    def __init__(self, img_list_path: str):
        self.strides = np.array(STRIDES)
        self.classes = {}
        for i in range(len(CLASSES)):
            self.classes[str(i)] = CLASSES[i]
        self.img_list_path = img_list_path
        self.data_lines = self.load_data_lines()
        self.batch_sum = int(np.ceil(len(self.data_lines) / TRAIN_BATCH_SIZE))
        self.batch_index = 0
        self.add_noise = False

    def load_data_lines(self, img_dir: str = '/JPEGImages/', label_dir: str = '/labels/'):
        with open(self.img_list_path, "r") as f:
            xy = []
            img_list = f.readlines()
            for img_path in img_list:
                img_path = img_path.strip()
                img_prefix, _ = os.path.splitext(img_path)
                label_prefix = img_prefix.replace(img_dir, label_dir)
                with open(label_prefix + ".txt", 'r') as f2:
                    lines = f2.readlines()
                    payload = ""
                    for line in lines:
                        line = line.strip().split()
                        class_num = int(line[0])
                        box_cent_x_01, box_cent_y_01 = float(line[1]), float(line[2])
                        box_w_01, box_h_01 = float(line[3]) / 2, float(line[4]) / 2
                        payload += " {},{},{},{},{}".format(
                            class_num, box_cent_x_01, box_cent_y_01, box_w_01, box_h_01)
                    xy.append(img_path + payload)
        np.random.shuffle(xy)
        return xy

    def __iter__(self):
        return self

    def __next__(self):
        with tf.device("/cpu:0"):
            w_out, h_out = TRAIN_INPUT_WIDTH // self.strides, TRAIN_INPUT_HEIGHT // self.strides
            # print(w_out,h_out)
            x = np.zeros((TRAIN_BATCH_SIZE, TRAIN_INPUT_WIDTH, TRAIN_INPUT_HEIGHT, 3), dtype=np.float32)
            # direct gt output with all
            out_per_batch = [np.zeros(
                (TRAIN_BATCH_SIZE, w_out[i], h_out[i], CELL_ANCHOR_NUM, 5 + NUM_CLASSES),
                dtype=np.float32) for i in range(3)]
            # gt output with just boxes
            out_boxes_per_batch = [np.zeros(
                (TRAIN_BATCH_SIZE, TRAIN_MAX_BOX_NUM, 4), dtype=np.float32) for _ in range(3)]

            if self.batch_index < self.batch_sum:
                for i in range(TRAIN_BATCH_SIZE):
                    index = (self.batch_index * TRAIN_BATCH_SIZE + i) % len(self.data_lines)
                    annotation = self.data_lines[index]
                    img, gt_boxes_with_id = self.parse_data_line(annotation)

                    x[i, :, :, :] = img

                    out_list, out_box_list = self.parse_gt_boxes(gt_boxes_with_id)
                    for j in range(3):
                        out_per_batch[j][i, :, :, :, :] = out_list[j]
                        out_boxes_per_batch[j][i, :, :] = out_box_list[j]

                self.batch_index += 1
                y_s = out_per_batch[0], out_boxes_per_batch[0]
                y_m = out_per_batch[1], out_boxes_per_batch[1]
                y_l = out_per_batch[2], out_boxes_per_batch[2]
                return x, (y_s, y_m, y_l)
            else:
                self.batch_index = 0
                np.random.shuffle(self.data_lines)
                raise StopIteration

    def paint_noise(self, img):
        return img

    def parse_data_line(self, data_line):
        data_line = data_line.split()
        img_path = data_line[0]
        img = cv2.imread(img_path)
        if img is None:
            raise KeyError("fail to load %s... " % img_path)
        h, w, _ = img.shape
        gt_boxes_with_id = np.array([list(map(float, box.split(","))) for box in data_line[1:]])
        gt_boxes_with_id = gt_boxes_with_id * np.array([1., w, h, w, h])
        if self.add_noise:
            self.paint_noise(img)
        # img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        img, gt_boxes_with_id = img_proc(img, (TRAIN_INPUT_WIDTH, TRAIN_INPUT_HEIGHT), gt_boxes_with_id)
        return img, gt_boxes_with_id

    # assign gt boxes to grids in blob
    def parse_gt_boxes(self, gt_boxes_with_id):
        w_out, h_out = TRAIN_INPUT_WIDTH // self.strides, TRAIN_INPUT_HEIGHT // self.strides
        # out list has 2-d struct
        out_list = [np.zeros((w_out[i], h_out[i], CELL_ANCHOR_NUM, 5 + NUM_CLASSES,))
                    for i in range(3)]
        # out_box_list is just a 1-d box list
        out_box_list = [np.zeros((TRAIN_MAX_BOX_NUM, 4)) for _ in range(3)]
        out_box_num = np.zeros((3,))

        for box in gt_boxes_with_id:
            class_id = int(box[0])
            one_hot = np.zeros(NUM_CLASSES, dtype=np.float)
            one_hot[class_id] = 1.
            uniform_distribution = np.full(NUM_CLASSES, 1. / NUM_CLASSES)
            delta = 0.01
            one_hot = one_hot * (1. - delta) + delta * uniform_distribution

            box_xy_wh = np.array(box[1:5], dtype=np.float)
            box_xy_wh_scaled = 1.0 * box_xy_wh[np.newaxis, :] / N_STRIDES[:, np.newaxis]

            iou_list = []
            exist_positive = False
            # if iou(gt box,grid) > xxx, assign it to the grid
            for i in range(3):
                anchors_xy_wh = np.zeros((CELL_ANCHOR_NUM, 4))
                anchors_xy_wh[:, 0:2] = (np.floor(box_xy_wh_scaled[i, 0:2]).astype(np.int32) + 0.5)
                anchors_xy_wh[:, 2:4] = N_ANCHORS[i]

                iou_scale = box_iou(box_xy_wh_scaled[i][np.newaxis, :], anchors_xy_wh)
                iou_list.append(iou_scale)
                iou_mask = iou_scale > BOX_ASSIGN_IOU_THRESH

                if np.any(iou_mask):
                    x_index, y_index = np.floor(box_xy_wh_scaled[i, 0:2]).astype(np.int32)

                    out_list[i][x_index, y_index, iou_mask, :] = 0
                    out_list[i][x_index, y_index, iou_mask, 0:4] = box_xy_wh
                    out_list[i][x_index, y_index, iou_mask, 4:5] = 1.0
                    out_list[i][x_index, y_index, iou_mask, 5:] = one_hot

                    box_index = int(out_box_num[i] % TRAIN_MAX_BOX_NUM)
                    out_box_list[i][box_index, :4] = box_xy_wh
                    out_box_num[i] += 1

                    exist_positive = True
            # gt box must be assigned to at least one grid
            if not exist_positive:
                best_anchor_index = np.argmax(np.array(iou_list).reshape(-1), axis=-1)
                best_detect = int(best_anchor_index / CELL_ANCHOR_NUM)
                best_anchor = int(best_anchor_index % CELL_ANCHOR_NUM)
                x_index, y_index = np.floor(box_xy_wh_scaled[best_detect, 0:2]).astype(np.int32)

                out_list[best_detect][x_index, y_index, best_anchor, :] = 0
                # print(box_xy_wh.shape)
                out_list[best_detect][x_index, y_index, best_anchor, 0:4] = box_xy_wh
                out_list[best_detect][x_index, y_index, best_anchor, 4:5] = 1.0
                out_list[best_detect][x_index, y_index, best_anchor, 5:] = one_hot

                box_index = int(out_box_num[best_detect] % TRAIN_MAX_BOX_NUM)
                out_box_list[best_detect][box_index, :4] = box_xy_wh
                out_box_num[best_detect] += 1
        return out_list, out_box_list

    def __len__(self):
        return self.batch_sum
