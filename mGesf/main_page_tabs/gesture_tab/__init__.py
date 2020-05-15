import webbrowser
import config


def help_btn_action():
    webbrowser.open(config.help_link)
    return


def calc_set(classes=['A', 'B', 'C', 'D', 'E'], repeat_times=10):
    """returns a full list of characters for the user to input"""
    set_str = ""

    for character in classes:
        for time in range(repeat_times):
            set_str += character + " "
        set_str += "\n"

    return set_str
