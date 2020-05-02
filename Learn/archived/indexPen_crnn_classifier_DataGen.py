import datetime
import pickle

from keras import Sequential, optimizers
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization, \
    LeakyReLU
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization, \
    LeakyReLU
from keras.regularizers import l2
from keras.engine.saving import load_model

from learn.classes import indexPenDataGen
from utils.path_utils import generate_train_val_ids

pre_trained_path = 'D:/trained_models/11_12_19.h5'
epochs = 50000
is_use_pre_train = True

classifying_labels = [0, 1, 2, 3, 4]
num_classes = len(classifying_labels)

interval_duration = 4
sample_per_sec = 20

timesteps = interval_duration * sample_per_sec

if __name__ == '__main__':
    dataGenParams = {'dim': (timesteps, 1, 25, 25, 25),
                     'batch_size': 8,
                     'n_classes': num_classes,
                     'shuffle': True}

    dataset_path = 'E:/alldataset/idp_dataset'
    label_dict_path = 'E:/alldataset/idp_label_dict.p'
    partition = generate_train_val_ids(0.2, dataset_path=dataset_path)
    labels = pickle.load(open(label_dict_path, 'rb'))

    ## Generators
    training_gen = indexPenDataGen(partition['train'], labels, dataset_path=dataset_path, **dataGenParams)
    validation_gen = indexPenDataGen(partition['validation'], labels, dataset_path=dataset_path, **dataGenParams)

    # Build the RNN ###############################################
    if not is_use_pre_train:
        classifier = Sequential()
        classifier.add(
            TimeDistributed(
                Conv3D(filters=32, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
                       kernel_regularizer=l2(0.0005), kernel_initializer='random_uniform'),
                input_shape=(timesteps, 1, 25, 25, 25)))
        # classifier.add(TimeDistributed(LeakyReLU(alpha=0.1)))
        classifier.add(TimeDistributed(BatchNormalization()))

        classifier.add(TimeDistributed(
            Conv3D(filters=32, kernel_size=(3, 3, 3), data_format='channels_first', kernel_regularizer=l2(0.0005))))
        # classifier.add(TimeDistributed(LeakyReLU(alpha=0.1)))
        classifier.add(TimeDistributed(BatchNormalization()))

        classifier.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))

        classifier.add(TimeDistributed(Flatten()))

        classifier.add(LSTM(units=128, return_sequences=True, kernel_initializer='random_uniform'))
        classifier.add(Dropout(rate=0.5))

        classifier.add(LSTM(units=128, return_sequences=True, kernel_initializer='random_uniform'))
        classifier.add(Dropout(rate=0.5))

        classifier.add(LSTM(units=128, return_sequences=False, kernel_initializer='random_uniform'))
        classifier.add(Dropout(rate=0.5))

        classifier.add(Dense(units=128))

        classifier.add(Dense(num_classes, activation='softmax', kernel_initializer='random_uniform'))

        # adam = optimizers.adam(lr=1e-6, decay=1e-2 / epochs)
        sgd = optimizers.SGD(lr=1e-4, momentum=0.9, decay=1e-2 / epochs, nesterov=True)

        classifier.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])
    else:
        print('Using Pre-trained Model: ' + pre_trained_path)
        classifier = load_model(pre_trained_path)

    # add early stopping
    es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=1000)
    mc = ModelCheckpoint(
        'D:/trained_models/bestSoFar_indexPen_CRNN' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
                                                                                                             '_') + '.h5',
        monitor='val_acc', mode='max', verbose=1, save_best_only=True)

    history = classifier.fit_generator(generator=training_gen, validation_data=validation_gen, use_multiprocessing=True,
                                       workers=6, shuffle=True, epochs=epochs, callbacks=[es, mc])

    import matplotlib.pyplot as plt

    plt.plot(history.history['acc'])
    plt.plot(history.history['val_acc'])
    plt.title('model accuracy')
    plt.ylabel('accuracy')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()

    # summarize history for loss
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()
