import datetime
import pickle
import os
from keras import Sequential, optimizers
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.engine.saving import load_model
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization, \
    LeakyReLU
from keras.regularizers import l2

from learn.classes import thumouseDataGen
from utils.path_utils import generate_train_val_ids


if __name__ == '__main__':
    is_use_pre_train = True
    epochs = 50000
    pre_trained_path = 'D:/PycharmProjects/mmWave_gesture_iwr6843/models/thm_model.h5'
    dataGenParams = {'dim': (1, 25, 25, 25),
                     'batch_size': 8,
                     'shuffle': True}

    dataset_path = 'D:/alldataset/thm_dataset'
    label_dict_path = 'D:/alldataset/thm_label_dict.p'

    data_ids = os.listdir(dataset_path)

    partition = generate_train_val_ids(0.2, dataset_path=data_ids)
    labels = pickle.load(open(label_dict_path, 'rb'))

    ## Generators
    training_gen = thumouseDataGen(partition['train'], labels, **dataGenParams, dataset_path=dataset_path)
    validation_gen = thumouseDataGen(partition['validation'], labels, **dataGenParams, dataset_path=dataset_path)

    if not is_use_pre_train:
        # Build the RNN ###############################################
        model = Sequential()
        model.add(Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
                         # kernel_regularizer=l2(0.0005)
                         ))
        # model.add(LeakyReLU(alpha=0.1))
        model.add(Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first'))
        # model.add(LeakyReLU(alpha=0.1))

        model.add(BatchNormalization())
        model.add(MaxPooling3D(pool_size=(2, 2, 2)))

        # model.add(
        #     TimeDistributed(Conv3D(filters=32, kernel_size=(3, 3, 3), data_format='channels_first',
        #                            activation='relu', kernel_regularizer=l2(0.0005))))
        # model.add(TimeDistributed(BatchNormalization()))
        # model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))

        model.add(Flatten())
        model.add(Dense(units=256))
        model.add(LeakyReLU(alpha=0.1))
        model.add(Dropout(0.5))

        model.add(Dense(units=3))
        # sgd = optimizers.SGD(lr=5e-5, momentum=0.9, decay=1e-6, nesterov=True)
        adam = optimizers.adam(lr=5e-6, decay=1e-6)
        model.compile(optimizer=adam, loss='mean_squared_error')
    else:
        print('Using Pre-trained Model: ' + pre_trained_path)
        model = load_model(pre_trained_path)

    # add early stopping
    es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=100)
    mc = ModelCheckpoint(
        'D:/trained_models/bestSoFar_thm_CRNN' + str(datetime.datetime.now()).replace(':', '-').replace(
            ' ', '_') + '.h5',
        monitor='val_loss', mode='min', verbose=1, save_best_only=True)

    history = model.fit_generator(generator=training_gen, validation_data=validation_gen, use_multiprocessing=True,
                                  workers=6, shuffle=True, epochs=epochs, callbacks=[es, mc])

    import matplotlib.pyplot as plt

    # summarize history for loss
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()
