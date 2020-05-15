import os

from keras.models import load_model
from keras.models import model_from_json
from keras import backend as K
import tensorflow as tf
import logging
import numpy as np

logger = logging.getLogger('root')


class NeuralNetwork:
    def __init__(self):
        self.session = tf.Session()
        self.graph = tf.get_default_graph()
        # the folder in which the model and weights are stored
        self.model_folder = os.path.join(os.path.abspath("src"), "static")
        self.model = None
        # for some reason in a flask app the graph/session needs to be used in the init else it hangs on other threads
        with self.graph.as_default():
            with self.session.as_default():
                logging.info("neural network initialised")

    def load(self, file_name):
        """
        :param file_name: [model_file_name, weights_file_name]
        :return:
        """
        with self.graph.as_default():
            with self.session.as_default():
                try:
                    self.model = load_model(file_name)
                    return True
                except Exception as e:
                    logging.exception(e)
                    return False

    def predict(self, x):
        with self.graph.as_default():
            with self.session.as_default():
                y = self.model.predict(x)
        return y

class onehot_decoder():

    def inverse_transform(datum):
        return np.argmax(datum)