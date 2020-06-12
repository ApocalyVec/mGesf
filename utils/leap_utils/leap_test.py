from utils.leap_utils.LeapInterface import LeapInterface
import time

start_time = time.time()
timing_list = []

def run_test():
    leap_interface = LeapInterface()

    while 1:
        # info = getLeapInfo()
        frame = leap_interface.getLeapFrame()
        # print(info)
        print(frame)
        timing_list.append(time.time())
        time.sleep(1e-3)


# normally, this file is used as a library that you import
# however, you can also just test by running "python leap.py" to see if things are working
if __name__ == "__main__":
    try:
        run_test()
    except KeyboardInterrupt:
        duration = time.time() - start_time
        fps = len(timing_list) / duration