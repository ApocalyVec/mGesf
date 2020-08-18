#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdio.h>
#include <string.h>

#include <wiringPi.h>
#include "taskRadar.h"

#define DEBUG

using namespace std;

int main(int argc, char *argv[])
{
    printf("raspbian_x4driver start to work!\n""""""\n\n");

    std::thread taskRadarThread(taskRadar);
    taskRadarThread.join();

    printf("raspbian_x4driver done.\n");
    return 0;
}
