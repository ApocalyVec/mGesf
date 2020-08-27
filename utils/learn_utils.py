import tensorflow as tf
from tensorflow.python.keras import Sequential, Model
from tensorflow.python.keras.layers import TimeDistributed, Conv2D, BatchNormalization, MaxPooling2D, Flatten, \
    concatenate, LSTM, Dropout, Dense

from config import ra_shape, rd_shape


def make_model(classes, points_per_sample):
    # creates the Time Distributed CNN for range Doppler heatmap ##########################
    mmw_rdpl_input = (points_per_sample,) + rd_shape + (1,) # range doppler shape here
    mmw_rdpl_TDCNN = Sequential()
    mmw_rdpl_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_last',
                   # kernel_regularizer=l2(0.0005),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_rdpl_input))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_rdpl_TDCNN.add(TimeDistributed(
        Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_last')))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_rdpl_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
    mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    # creates the Time Distributed CNN for range Azimuth heatmap ###########################
    mmw_razi_input = (points_per_sample,) + ra_shape + (1,)  # range azimuth shape here
    mmw_razi_TDCNN = Sequential()
    mmw_razi_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_last',
                   # kernel_regularizer=l2(0.0005),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_razi_input))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_razi_TDCNN.add(TimeDistributed(
        Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_last')))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_razi_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
    mmw_razi_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    merged = concatenate([mmw_rdpl_TDCNN.output, mmw_razi_TDCNN.output])  # concatenate two feature extractors
    regressive_tensor = LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform')(merged)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
    regressive_tensor = LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform')(regressive_tensor)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)

    regressive_tensor = Dense(units=128)(regressive_tensor)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
    regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)

    model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)
    adam = tf.keras.optimizers.Adam(lr=1e-4, decay=1e-7)
    model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
    return model


def make_model_simple(classes, points_per_sample, channel_mode='channels_last'):
    # creates the Time Distributed CNN for range Doppler heatmap ##########################
    mmw_rdpl_input = (points_per_sample,) + rd_shape + (1,) if channel_mode == 'channels_last' else (points_per_sample, 1) + rd_shape
    # range doppler shape here
    mmw_rdpl_TDCNN = Sequential()
    mmw_rdpl_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=4, kernel_size=(3, 3), data_format=channel_mode,
                   # kernel_regularizer=l2(0.0005),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_rdpl_input))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    # creates the Time Distributed CNN for range Azimuth heatmap ###########################
    mmw_razi_input = (points_per_sample,) + ra_shape + (1,)  if channel_mode == 'channels_last' else (points_per_sample, 1) + ra_shape
    mmw_razi_TDCNN = Sequential()
    mmw_razi_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=4, kernel_size=(3, 3), data_format=channel_mode,
                   # kernel_regularizer=l2(0.0005),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_razi_input))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_razi_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    merged = concatenate([mmw_rdpl_TDCNN.output, mmw_razi_TDCNN.output])  # concatenate two feature extractors
    regressive_tensor = LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform')(merged)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)

    regressive_tensor = Dense(units=32)(regressive_tensor)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
    regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)

    model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)
    adam = tf.keras.optimizers.Adam(lr=1e-5, decay=1e-7)
    model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
    return model


# def make_model_simple(classes, points_per_sample, channel_mode='channels_last'):
#     # creates the Time Distributed CNN for range Doppler heatmap ##########################
#     mmw_rdpl_input = (points_per_sample,) + rd_shape + (1,) if channel_mode == 'channels_last' else (points_per_sample, 1) + rd_shape
#     # range doppler shape here
#     mmw_rdpl_TDCNN = Sequential()
#     mmw_rdpl_TDCNN.add(
#         TimeDistributed(
#             Conv2D(filters=4, kernel_size=(3, 3), data_format=channel_mode,
#                    # kernel_regularizer=l2(0.0005),
#                    kernel_initializer='random_uniform'),
#             input_shape=mmw_rdpl_input))
#     # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
#     mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
#     mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets
#
#     # creates the Time Distributed CNN for range Azimuth heatmap ###########################
#     mmw_razi_input = (points_per_sample,) + ra_shape + (1,)  if channel_mode == 'channels_last' else (points_per_sample, 1) + ra_shape
#     mmw_razi_TDCNN = Sequential()
#     mmw_razi_TDCNN.add(
#         TimeDistributed(
#             Conv2D(filters=4, kernel_size=(3, 3), data_format=channel_mode,
#                    # kernel_regularizer=l2(0.0005),
#                    kernel_initializer='random_uniform'),
#             input_shape=mmw_razi_input))
#     # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
#     mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
#     mmw_razi_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets
#
#     merged = concatenate([mmw_rdpl_TDCNN.output, mmw_razi_TDCNN.output])  # concatenate two feature extractors
#     regressive_tensor = LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform')(merged)
#     regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
#
#     regressive_tensor = Dense(units=32)(regressive_tensor)
#     regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
#     regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)
#
#     model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)
#     adam = tf.keras.optimizers.Adam(lr=1e-5, decay=1e-7)
#     model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
#     return model