import os
import shutil

import tensorflow as tf
physical_devices = tf.config.experimental.list_physical_devices('GPU')
if len(physical_devices) > 0:
    tf.config.experimental.set_memory_growth(physical_devices[0], True)
from absl import app

from config import *
from dataset import Dataset
from loss import compute_loss
from model import yolo_model


def main(_argv):    

    train_set = Dataset('./train.list')
    # test_set = Dataset('./test.list')
    log_dir = "/tmp/yolo-log"
    steps_per_epoch: int = len(train_set)
    [warmup_epochs, first_stage_epochs, second_stage_epochs] = TRAIN_EPOCHES
    global_steps = tf.Variable(1, trainable=False, dtype=tf.int64)
    warmup_steps = warmup_epochs * steps_per_epoch
    total_steps = (warmup_epochs + first_stage_epochs + second_stage_epochs) * steps_per_epoch
    input_layer = tf.keras.layers.Input([TRAIN_INPUT_WIDTH, TRAIN_INPUT_HEIGHT, 3])

    model, pred_all = yolo_model(input_layer)
    # model.summary()
    model.load_weights('./yolo_epoch_104.h5',True,True)

    optimizer = tf.keras.optimizers.Adam()
    if os.path.exists(log_dir):
        shutil.rmtree(log_dir)
    writer = tf.summary.create_file_writer(log_dir)

    def train_step(x, y):
        with tf.GradientTape() as tape:
            pred_result = model(x, training=True)
            giou_loss = conf_loss = prob_loss = 0
            # optimizing process
            for i in range(3):
                conv, pred = pred_result[i * 2], pred_result[i * 2 + 1]
                loss_items = compute_loss(pred, conv, y[i][0], y[i][1], i=i)
                giou_loss += loss_items[0]
                conf_loss += loss_items[1]
                prob_loss += loss_items[2]

            total_loss: float = giou_loss + conf_loss + prob_loss

            gradients = tape.gradient(total_loss, model.trainable_variables)
            optimizer.apply_gradients(zip(gradients, model.trainable_variables))
            tf.print("=> STEP %04d/%04d   lr: %.6f   giou_loss: %4.2f   conf_loss: %4.2f   "
                     "prob_loss: %4.2f   total_loss: %4.2f" % (
                         global_steps, total_steps, optimizer.lr.numpy(),
                         giou_loss, conf_loss, prob_loss, total_loss))
            # update learning rate
            global_steps.assign_add(1)
            if global_steps < warmup_steps:
                lr = global_steps / warmup_steps * TRAIN_LR_BEGIN
            else:
                lr = TRAIN_LR_END + 0.5 * (TRAIN_LR_BEGIN - TRAIN_LR_END) * (
                    (1 + tf.cos((global_steps - warmup_steps) / (total_steps - warmup_steps) * np.pi)))
            optimizer.lr.assign(lr.numpy())

            # writing summary data
            with writer.as_default():
                tf.summary.scalar("lr", optimizer.lr, step=global_steps)
                tf.summary.scalar("loss/total_loss", total_loss, step=global_steps)
                tf.summary.scalar("loss/giou_loss", giou_loss, step=global_steps)
                tf.summary.scalar("loss/conf_loss", conf_loss, step=global_steps)
                tf.summary.scalar("loss/prob_loss", prob_loss, step=global_steps)
            writer.flush()

    for epoch in range(warmup_steps + first_stage_epochs + second_stage_epochs):
        if epoch == warmup_epochs:
            for i in range(len(model.layers)):
                model.layers[i].trainable = True
        for image_data, target in train_set:
            train_step(image_data, target)
        model.save_weights("/tmp/yolo_epoch_" + str(epoch) + ".h5")


if __name__ == '__main__':
    os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
    os.environ['TF_FORCE_GPU_ALLOW_GROWTH'] = 'true'

    try:
        app.run(main)
    except SystemExit:
        pass
