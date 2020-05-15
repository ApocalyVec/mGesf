import cv2
import numpy as np
from PIL import Image
import datetime
import os
import time

def nothing(x):
    # any operation
    pass

cap = cv2.VideoCapture(0)

# cv2.namedWindow("Trackbars")
# cv2.namedWindow("Trackbars2")

# cv2.createTrackbar("L-H", "Trackbars", 0, 180, nothing)
# cv2.createTrackbar("L-S", "Trackbars", 0, 255, nothing)
# cv2.createTrackbar("L-V", "Trackbars", 0, 255, nothing)
# cv2.createTrackbar("U-H", "Trackbars", 180, 180, nothing)
# cv2.createTrackbar("U-S", "Trackbars", 255, 255, nothing)
# cv2.createTrackbar("U-V", "Trackbars", 255, 255, nothing)

today = datetime.datetime.now()

root_dn = 'v_data-' + str(today).replace(':', '-').replace(' ', '_')
cam1_dn = os.path.join(root_dn, 'cam2')

cam1_list = []

while True:
    _, frame = cap.read()


    # hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    #
    # lower_h = cv2.getTrackbarPos("L-H", "Trackbars")
    # lower_s = cv2.getTrackbarPos("L-S", "Trackbars")
    # lower_v = cv2.getTrackbarPos("L-V", "Trackbars")
    #
    # upper_h = cv2.getTrackbarPos("U-H", "Trackbars")
    # upper_s = cv2.getTrackbarPos("U-S", "Trackbars")
    # upper_v = cv2.getTrackbarPos("U-V", "Trackbars")


    # lower_red = np.a([lower_h, lower_s, lower_v])
    # upper_red = np.a([upper_h, upper_s, upper_v])
    #
    # mask = cv2.inRange(hsv, lower_red, upper_red)

    cv2.imshow("Frame1", frame)

    # cv2.imshow("Mask", mask)

    im1 = Image.fromarray(frame)

    cur_time = str(time.time())

    # put the newly captured frames in a list
    cam1_list.append([im1, cur_time])

    key = cv2.waitKey(1)
    if key == 27:
        break

is_save = input('Do you wish to save the frames? [y/n]')

if is_save == 'y':
    os.mkdir(root_dn)
    os.mkdir(cam1_dn)

    for i, frame in enumerate(cam1_list):
        print('Processing ' + str(i + 1) + ' of ' + str(len(cam1_list)) + ' of cam2')

        img = frame[0].split()
        img = Image.merge("RGB", (img[2], img[1], img[0]))

        img.save(open(os.path.join(cam1_dn, frame[1]) + '.jpg', 'wb'))

cap.release()
cv2.destroyAllWindows()

print('Done!')