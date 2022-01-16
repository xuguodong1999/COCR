import numpy as np

ANCHORS = [10, 10, 10, 40, 40, 10, 30, 30, 30, 120, 120, 30, 60, 60, 60, 80, 80, 60]
N_ANCHORS = np.array(ANCHORS).reshape(3, 3, 2)
STRIDES = [8, 16, 32]
N_STRIDES = np.array(STRIDES)
CLASSES = [  # "Dash","Single","Double","Triple","Solid","C","H","O","N","P","S"
    "@", "-", "=", "#", "@@", "C", "H", "O", "N", "P", "S"]
NUM_CLASSES = 11
CELL_ANCHOR_NUM = 3
LOSS_IOU_THRESH = 0.5
BOX_ASSIGN_IOU_THRESH = 0.3

TRAIN_BATCH_SIZE = 4
TRAIN_INPUT_WIDTH = 416
TRAIN_INPUT_HEIGHT = 416
TRAIN_USE_DATA_AUG = True
TRAIN_LR_BEGIN = 1e-3
TRAIN_LR_END = 1e-6
TRAIN_EPOCHES = [5, 50, 50]

TRAIN_MAX_BOX_NUM = 1024

TEST_IOU_THRESH = 0.5
TEST_CONF_THRESH = 0.5
