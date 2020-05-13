import datetime
import pickle

from keras import Sequential, optimizers
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization
from keras.regularizers import l2

from learn.classes import thumouseDataGen
from utils.path_utils import generate_train_val_ids

if __name__ == '__main__':
    dataGenParams = {'dim': (10, 1, 25, 25, 25),
                     'batch_size': 8,
                     'shuffle': True}

    dataset_path = 'D:/thumouse/dataset'
    label_dict_path = 'D:/thumouse/labels/label_dict.p'

    partition = generate_train_val_ids(0.1, dataset_path=dataset_path)
    labels = pickle.load(open(label_dict_path, 'rb'))

    ## Generators
    training_gen = thumouseDataGen(partition['train'], labels, **dataGenParams, dataset_path=dataset_path)
    validation_gen = thumouseDataGen(partition['validation'], labels, **dataGenParams, dataset_path=dataset_path)

    # Build the RNN ###############################################
    model = Sequential()
    model.add(
        TimeDistributed(Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
                               activation='relu', kernel_regularizer=l2(0.0005)), input_shape=(10, 1, 25, 25, 25)))
    model.add(TimeDistributed(BatchNormalization()))
    model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))

    # model.add(
    #     TimeDistributed(Conv3D(filters=32, kernel_size=(3, 3, 3), data_format='channels_first',
    #                            activation='relu', kernel_regularizer=l2(0.0005))))
    # model.add(TimeDistributed(BatchNormalization()))
    # model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))

    model.add(TimeDistributed(Flatten()))

    model.add(LSTM(units=16, return_sequences=False))
    model.add(Dropout(rate=0.5))

    model.add(Dense(units=2))
    epochs = 5000

    adam = optimizers.adam(lr=1e-3, decay=1e-2 / epochs)
    model.compile(optimizer=adam, loss='mean_squared_error')

    # add early stopping
    es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=100)
    mc = ModelCheckpoint(
        'D:/thumouse/trained_models/bestSoFar_thuMouse_CRNN' + str(datetime.datetime.now()).replace(':', '-').replace(' ', '_') + '.h5',
        monitor='val_loss', mode='min', verbose=1, save_best_only=True)

    history = model.fit_generator(generator=training_gen, validation_data=validation_gen, use_multiprocessing=True, workers=6, shuffle=True, epochs=epochs, callbacks=[es, mc])

    import matplotlib.pyplot as plt

    # summarize history for loss
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()
