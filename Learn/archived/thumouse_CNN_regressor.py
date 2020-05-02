import datetime
import pickle
import os
import numpy as np
from keras import Sequential, optimizers
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.engine.saving import load_model
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization, \
    LeakyReLU
from keras.regularizers import l2
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
from keras import backend

# from learn.classes import thumouseDataGen
from utils.path_utils import generate_train_val_ids


if __name__ == '__main__':

    # end of train the CNN, start of training the RNN ##################################################################

    is_use_pre_train = False
    epochs = 50000
    timesteps = 3

    pre_trained_path = 'D:/PycharmProjects/mmWave_gesture_iwr6843/models/thm_model.h5'

    date = 121319
    label_path = 'D:/dataset_thm_leap_' + str(date) + '/label.p'
    dataset_path = 'D:/dataset_thm_leap_' + str(date)
    scaler_path = 'D:/train_result/thm_scaler' + str(date) + '.p'
    labels = pickle.load(open(label_path, 'rb'))

    ## Generators
    X = []
    Y = []

    for i, data in enumerate(os.listdir(dataset_path)):
        print('Loading ' + str(i) + ' of ' + str(len(os.listdir(dataset_path))))
        if data.split('.')[-1] == 'npy':  # only load .npy files
            X.append(np.load(os.path.join(dataset_path, data)))
            Y.append(labels[data.strip('.npy')])
            print('loaded: ' + data)
    print('Load Finished!')

    X = np.asarray(X)
    Y = np.asarray(Y)

    # minmax normallize y
    scaler = MinMaxScaler()
    Y = scaler.fit_transform(Y)
    pickle.dump(scaler, open('D:/PycharmProjects/mmWave_gesture_iwr6843/models/data_scaler_' + str(date) + '.p', 'wb'))

    print('Splitting test-train...')
    X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.20, random_state=3, shuffle=True)

    # if not is_use_pre_train:
    #     print('Building Model...')
    #
    #     # CNN version ###############################################
    #     model = Sequential()
    #     model.add(Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
    #                      # kernel_regularizer=l2(0.0005)
    #                      ))
    #     # model.add(LeakyReLU(alpha=0.1))
    #     model.add(Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first'))
    #     # model.add(LeakyReLU(alpha=0.1))
    #
    #     model.add(BatchNormalization())
    #     model.add(MaxPooling3D(pool_size=(2, 2, 2)))
    #
    #     model.add(Flatten())
    #
    #     # CRNN version ################################################
    #     model = Sequential()
    #     model.add(
    #         TimeDistributed(
    #             Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
    #                    kernel_regularizer=l2(0.0005), kernel_initializer='random_uniform'),
    #             input_shape=(timesteps, 1, 25, 25, 25)))
    #     # classifier.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    #     model.add(TimeDistributed(BatchNormalization()))
    #
    #     model.add(TimeDistributed(
    #         Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', kernel_regularizer=l2(0.0005))))
    #     # classifier.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    #     model.add(TimeDistributed(BatchNormalization()))
    #     model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))
    #     model.add(TimeDistributed(Flatten()))
    #
    #     model.add(LSTM(units=64, return_sequences=True, kernel_initializer='random_uniform'))
    #     model.add(Dropout(rate=0.4))
    #
    #     model.add(LSTM(units=64, return_sequences=True, kernel_initializer='random_uniform'))
    #     model.add(Dropout(rate=0.4))
    #
    #     model.add(LSTM(units=64, return_sequences=False, kernel_initializer='random_uniform'))
    #     model.add(Dropout(rate=0.4))
    #
    #     model.add(Dense(units=256))
    #     model.add(LeakyReLU(alpha=0.1))
    #     model.add(Dropout(0.5))
    #
    #     model.add(Dense(units=2))
    #     # sgd = optimizers.SGD(lr=5e-5, momentum=0.9, decay=1e-6, nesterov=True)
    #     adam = optimizers.adam(lr=5e-6, decay=1e-6)
    #     model.compile(optimizer=adam, loss='mean_squared_error')
    # else:
    #     print('Using Pre-trained Model: ' + pre_trained_path)
    #     model = load_model(pre_trained_path)
    #
    # # add early stopping
    # es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=50)
    # mc = ModelCheckpoint(
    #     'D:/trained_models/bestSoFar_thm_CRNN' + str(datetime.datetime.now()).replace(':', '-').replace(
    #         ' ', '_') + '.h5',
    #     monitor='val_loss', mode='min', verbose=1, save_best_only=True)
    #
    # history = model.fit(X_train, Y_train, validation_data=(X_test, Y_test),
    #                     batch_size=16, epochs=epochs, callbacks=[es, mc])
    #
    # import matplotlib.pyplot as plt
    #
    # # summarize history for loss
    # plt.plot(history.history['loss'])
    # plt.plot(history.history['val_loss'])
    # plt.title('model loss')
    # plt.ylabel('loss')
    # plt.xlabel('epoch')
    # plt.legend(['train', 'test'], loc='upper left')
    # plt.show()
    #
    #
    # # end of train the CNN, start of training the RNN ##################################################################
    # del model
    # del X, Y, X_train, X_test, Y_train, Y_test
    # backend.clear_session()
    #
    # is_use_pre_train = False
    # epochs = 50000
    # # timesteps = 5
    #
    # pre_trained_path = 'D:/PycharmProjects/mmWave_gesture_iwr6843/models/thm_model.h5'
    #
    # dataset_path = 'D:/alldataset/thm_dataset/'
    # label_path = 'D:/alldataset/thm_label_dict.p'
    # data_without_label = pickle.load(open('D:/alldataset/thm_data_without_label.p', 'rb'))
    # labels = pickle.load(open(label_path, 'rb'))
    #
    # ## Generators
    # X = []
    # Y = []
    #
    # for i, data in enumerate(os.listdir(dataset_path)):
    #     print('Loading ' + str(i) + ' of ' + str(len(os.listdir(dataset_path))))
    #     if data.strip('.npy') not in data_without_label:
    #         X.append(np.load(os.path.join(dataset_path, data)))
    #         Y.append(labels[data.strip('.npy')])
    # print('Load Finished!')
    #
    # X = np.asarray(X)
    # Y = np.asarray(Y)
    #
    # # minmax normallize y
    # scaler = MinMaxScaler()
    # Y = scaler.fit_transform(Y)
    # pickle.dump(scaler, open('D:/PycharmProjects/mmWave_gesture_iwr6843/models/120519_data_scaler.p', 'wb'))
    #
    # print('Splitting test-train...')
    # X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.20, random_state=3, shuffle=True)

    if not is_use_pre_train:
        print('Building Model...')
        # CRNN version ###############################################
        model = Sequential()
        model.add(
            TimeDistributed(
                Conv3D(filters=8, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
                       kernel_regularizer=l2(0.0005), kernel_initializer='random_uniform'),
                input_shape=(timesteps, 1, 25, 25, 25)))
        # classifier.add(TimeDistributed(LeakyReLU(alpha=0.1)))
        model.add(TimeDistributed(BatchNormalization()))

        model.add(TimeDistributed(
            Conv3D(filters=8, kernel_size=(3, 3, 3), data_format='channels_first', kernel_regularizer=l2(0.0005))))
        # classifier.add(TimeDistributed(LeakyReLU(alpha=0.1)))
        model.add(TimeDistributed(BatchNormalization()))
        model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))
        model.add(TimeDistributed(Flatten()))

        model.add(LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform'))
        model.add(Dropout(rate=0.4))

        model.add(LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform'))
        model.add(Dropout(rate=0.4))

        model.add(LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform'))
        model.add(Dropout(rate=0.4))

        model.add(Dense(units=256))
        model.add(LeakyReLU(alpha=0.1))
        model.add(Dropout(0.5))

        model.add(Dense(units=3))
        # sgd = optimizers.SGD(lr=5e-5, momentum=0.9, decay=1e-6, nesterov=True)
        adam = optimizers.adam(lr=1e-6, decay=1e-6)
        model.compile(optimizer=adam, loss='mean_squared_error')
    else:
        print('Using Pre-trained Model: ' + pre_trained_path)
        model = load_model(pre_trained_path)

    # add early stopping
    es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=50)
    mc = ModelCheckpoint(
        'D:/train_result/bestSoFar_thm_CNN' + str(datetime.datetime.now()).replace(':', '-').replace(
            ' ', '_') + '.h5',
        monitor='val_loss', mode='min', verbose=1, save_best_only=True)

    history = model.fit(X_train, Y_train, validation_data=(X_test, Y_test),
                        batch_size=16, epochs=epochs, callbacks=[es, mc])

    import matplotlib.pyplot as plt

    # summarize history for loss
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()
