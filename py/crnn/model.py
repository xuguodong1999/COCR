from tensorflow import keras
from tensorflow.keras import layers
from tensorflow.keras.applications import MobileNetV2,EfficientNetB0

def vgg_in_paper(input_tensor):
    x = input_tensor
    for i in range(1, 3):
        x = layers.Conv2D(64 * i, 3, padding='same', activation='relu')(x)
        x = layers.MaxPool2D(pool_size=2, padding='same')(x)
    for i in range(1, 3):
        x = layers.Conv2D(256 * i, 3, padding='same', use_bias=False)(x)
        x = layers.BatchNormalization()(x)
        x = layers.Activation('relu')(x)
        x = layers.Conv2D(256 * i, 3, padding='same', activation='relu')(x)
        x = layers.MaxPool2D(pool_size=2, strides=(2, 1), padding='same')(x)
    x = layers.Conv2D(512, 2, use_bias=False)(x)
    x = layers.BatchNormalization()(x)
    x = layers.Activation('relu')(x)
    print(type(x))
    return x


def backbone(input_tensor, model_name: str = 'mobilenetv2'):
    if model_name == 'vgg':
        return vgg_in_paper(input_tensor)
    elif model_name == 'mobilenetv2':
        model= MobileNetV2(
            alpha=0.5,
            include_top=False,
            weights="imagenet",
            input_shape=(None, None, 3)
        )
        print(type(model))
        return model(input_tensor)
    elif model_name == 'efficientnet':
        return EfficientNetB0(
            include_top=False,
            weights="imagenet",
            input_shape=(None, None, 3)
        )(input_tensor)


def build(class_num: int = 26 * 2 + 10 + 2 + 2,
          input_height: int = 32, backbone_name: str = 'mobilenetv2'):
    img_input = keras.Input(shape=(input_height, None, 3))
    print(type(img_input))
    x = backbone(img_input, backbone_name)
    print(type(layers.Reshape((-1, 512))))
    x = layers.Reshape((-1, 512))(x)
    print(type(x))
    x = layers.Bidirectional(layers.LSTM(units=256, return_sequences=True))(x)
    x = layers.Bidirectional(layers.LSTM(units=256, return_sequences=True))(x)
    x = layers.Dense(units=class_num)(x)
    return keras.Model(inputs=img_input, outputs=x, name='CRNN')


if __name__ == '__main__':
    model = build(66, 32, 'vgg')
    # model.save('D:/tmp1.h5')
    model = build(66, 32, 'mobilenetv2')
    # model.save('D:/tmp2.h5')
    model = build(66, 32, 'efficientnet')
    print(type(model))
    # model.save('D:/tmp3.h5')