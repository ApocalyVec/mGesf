import webbrowser
import config


def help_btn_action():
    webbrowser.open(config.help_link)
    return


def generate_char_set(classes, repeat_times):
    """returns a full list of characters for the user to input"""
    return [c for c in classes for i in range(repeat_times)]

