#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "radar_hal.h"
#include "taskRadar.h"
#include "x4driver.h"
#include "xep_hal.h"

volatile xtx4driver_errors_t x4_initialize_status = XEP_ERROR_X4DRIVER_UNINITIALIZED;
X4Driver_t *x4driver = NULL;

#define DEBUG 0

using namespace std;
std::recursive_mutex x4driver_mutex;

typedef struct
{
    //TaskHandle_t radar_task_handle;
    radar_handle_t *radar_handle; // Some info separating different radar chips on the same module.
} XepRadarX4DriverUserReference_t;

typedef struct
{
    //XepDispatch_t* dispatch;
    X4Driver_t *x4driver;
} RadarTaskParameters_t;

void x4driver_data_ready(void)
{
    uint32_t status = XEP_ERROR_X4DRIVER_OK;
    uint32_t bin_count = 0;
    x4driver_get_frame_bin_count(x4driver, &bin_count);
    uint8_t down_conversion_enabled = 0;
    x4driver_get_downconversion(x4driver, &down_conversion_enabled);

    uint32_t fdata_count = bin_count;
    if (down_conversion_enabled == 1)
    {
        fdata_count = bin_count * 2;
    }

    uint32_t frame_counter = 0;
    float32_t data_frame_normolized[fdata_count];

    status = x4driver_read_frame_normalized(x4driver, &frame_counter, data_frame_normolized, fdata_count);

    if (XEP_ERROR_X4DRIVER_OK == status)
    {
        printf("x4 frame data ready! \n");
    }
    else
    {
        printf("fail to get x4 frame data errorcode:%d! \n", status);
    }

    printf("Size:%d,New Frame Data Normolized(%d){\n", fdata_count, frame_counter);
    for (uint32_t i = 0; i < fdata_count; i++)
    {
        printf(" %f, ", data_frame_normolized[i]);
    }
    printf("}\n");
}

static uint32_t x4driver_callback_take_sem(void *sem, uint32_t timeout)
{
    x4driver_mutex.lock();
    return 1;
}

static void x4driver_callback_give_sem(void *sem)
{
    x4driver_mutex.unlock();
}

static uint32_t x4driver_callback_pin_set_enable(void *user_reference, uint8_t value)
{
    XepRadarX4DriverUserReference_t *x4driver_user_reference = (XepRadarX4DriverUserReference_t *)user_reference;
    int status = radar_hal_pin_set_enable(x4driver_user_reference->radar_handle, value);
    return status;
}

static uint32_t x4driver_callback_spi_write(void *user_reference, uint8_t *data, uint32_t length)
{
    XepRadarX4DriverUserReference_t *x4driver_user_reference = (XepRadarX4DriverUserReference_t *)user_reference;
    return radar_hal_spi_write(x4driver_user_reference->radar_handle, data, length);
}
static uint32_t x4driver_callback_spi_read(void *user_reference, uint8_t *data, uint32_t length)
{
    XepRadarX4DriverUserReference_t *x4driver_user_reference = (XepRadarX4DriverUserReference_t *)user_reference;
    return radar_hal_spi_read(x4driver_user_reference->radar_handle, data, length);
}

static uint32_t x4driver_callback_spi_write_read(void *user_reference, uint8_t *wdata, uint32_t wlength, uint8_t *rdata, uint32_t rlength)
{
    XepRadarX4DriverUserReference_t *x4driver_user_reference = (XepRadarX4DriverUserReference_t *)user_reference;
    return radar_hal_spi_write_read(x4driver_user_reference->radar_handle, wdata, wlength, rdata, rlength);
}

static void x4driver_callback_wait_us(uint32_t us)
{
    delayMicroseconds(us);
}

void x4driver_enable_ISR(void *user_reference, uint32_t enable)
{
    if (enable == 1)
    {
        pinMode(X4_GPIO_INT, INPUT);
        pullUpDnControl(X4_GPIO_INT, PUD_DOWN);
        if (wiringPiISR(X4_GPIO_INT, INT_EDGE_RISING, &x4driver_data_ready) < 0)
        {
            printf("unable to setup ISR");
        }
    }
    else
        pinMode(X4_GPIO_INT, OUTPUT); //disable Interrupt
}

uint32_t task_radar_init(X4Driver_t **x4driver)
{
    XepRadarX4DriverUserReference_t *x4driver_user_reference = (XepRadarX4DriverUserReference_t *)malloc(sizeof(XepRadarX4DriverUserReference_t));
    memset(x4driver_user_reference, 0, sizeof(XepRadarX4DriverUserReference_t));

    void *radar_hal_memory = malloc(radar_hal_get_instance_size());
    int status = radar_hal_init(&(x4driver_user_reference->radar_handle), radar_hal_memory);

#ifdef DEBUG
    if (status == XT_SUCCESS)
    {
        printf("radar_hal_init success\n");
    }
    else
    {
        printf("radar_hal_init unknow situcation\n");
    }
#endif // DEBUG

    //! [X4Driver Platform Dependencies]

    // X4Driver lock mechanism, including methods for locking and unlocking.
    X4DriverLock_t lock;
    lock.object = (void *)&x4driver_mutex;
    lock.lock = x4driver_callback_take_sem;
    lock.unlock = x4driver_callback_give_sem;

    // X4Driver timer for generating sweep FPS on MCU. Not used when sweep FPS is generated on X4.
    //    uint32_t timer_id_sweep = 2;
    X4DriverTimer_t timer_sweep;
    //    timer_sweep.object = xTimerCreate("X4Driver_sweep_timer", 1000 / portTICK_PERIOD_MS, pdTRUE, (void*)timer_id_sweep, x4driver_timer_sweep_timeout);
    //    timer_sweep.configure = x4driver_timer_set_timer_timeout_frequency;

    // X4Driver timer used for driver action timeout.
    //    uint32_t timer_id_action = 3;
    X4DriverTimer_t timer_action;
    //    timer_action.object = xTimerCreate("X4Driver_action_timer", 1000 / portTICK_PERIOD_MS, pdTRUE, (void*)timer_id_action, x4driver_timer_action_timeout);
    //	timer_action.configure = x4driver_timer_set_timer_timeout_frequency;

    // X4Driver callback methods.
    X4DriverCallbacks_t x4driver_callbacks;

    x4driver_callbacks.pin_set_enable = x4driver_callback_pin_set_enable; // X4 ENABLE pin
    x4driver_callbacks.spi_read = x4driver_callback_spi_read;             // SPI read method
    x4driver_callbacks.spi_write = x4driver_callback_spi_write;           // SPI write method
    x4driver_callbacks.spi_write_read = x4driver_callback_spi_write_read; // SPI write and read method
    x4driver_callbacks.wait_us = x4driver_callback_wait_us;               // Delay method
                                                                          //  x4driver_callbacks.notify_data_ready = x4driver_notify_data_ready;      // Notification when radar data is ready to read
                                                                          //  x4driver_callbacks.trigger_sweep = x4driver_trigger_sweep_pin;          // Method to set X4 sweep trigger pin
    x4driver_callbacks.enable_data_ready_isr = x4driver_enable_ISR;       // Control data ready notification ISR

    void *x4driver_instance_memory = malloc(x4driver_get_instance_size()); //pvPortMalloc(x4driver_get_instance_size());
    //x4driver_create(x4driver, x4driver_instance_memory, &x4driver_callbacks,&lock,&timer_sweep,&timer_action, (void*)x4driver_user_reference);
    x4driver_create(x4driver, x4driver_instance_memory, &x4driver_callbacks, &lock, &timer_sweep, &timer_action, x4driver_user_reference);

#ifdef DEBUG
    if (status == XEP_ERROR_X4DRIVER_OK)
    {
        printf("x4driver_create success\n");
    }
    else
    {
        printf("x4driver_create unknow situcation\n");
    }
#endif // DEBUG

    RadarTaskParameters_t *task_parameters = (RadarTaskParameters_t *)malloc(sizeof(RadarTaskParameters_t));
    //task_parameters->dispatch = dispatch;
    task_parameters->x4driver = *x4driver;

    task_parameters->x4driver->spi_buffer_size = 192 * 32;
    task_parameters->x4driver->spi_buffer = (uint8_t *)malloc(task_parameters->x4driver->spi_buffer_size);
    if ((((uint32_t)task_parameters->x4driver->spi_buffer) % 32) != 0)
    {
        int alignment_diff = 32 - (((uint32_t)task_parameters->x4driver->spi_buffer) % 32);
        task_parameters->x4driver->spi_buffer += alignment_diff;
        task_parameters->x4driver->spi_buffer_size -= alignment_diff;
    }
    task_parameters->x4driver->spi_buffer_size -= task_parameters->x4driver->spi_buffer_size % 32;

    //    xTaskCreate(task_radar, (const char * const) "Radar", TASK_RADAR_STACK_SIZE, (void*)task_parameters, TASK_RADAR_PRIORITY, &h_task_radar);
    //    x4driver_user_reference->radar_task_handle = h_task_radar;

    // TODO: downconversion bug
    //task_parameters->x4driver->downconversion_enabled=1;

    return XT_SUCCESS;
}

int taskRadar(void)
{
    printf("task_radar start!\n");

    uint32_t status = 0;
    //uint8_t* data_frame;

    //initialize radar task

    status = task_radar_init(&x4driver);

#ifdef DEBUG
    if (status == XT_SUCCESS)
    {
        printf("task_radar_init success\n");
    }
    else if (status == XT_ERROR)
    {
        printf("task_radar_init failure\n");
    }
    else
    {
        printf("task_radar_init unknow situcation\n");
    }
#endif // DEBUG

    xtx4driver_errors_t tmp_status = (xtx4driver_errors_t)x4driver_init(x4driver);

#ifdef DEBUG
    if (tmp_status == XEP_ERROR_X4DRIVER_OK)
    {
        printf("x4driver_init success\n");
    }
    else
    {
        printf("x4driver_init unknow situcation\n");
    }
#endif // DEBUG

    status = x4driver_set_sweep_trigger_control(x4driver, SWEEP_TRIGGER_X4); // By default let sweep trigger control done by X4
#ifdef DEBUG
    if (status == XEP_ERROR_X4DRIVER_OK)
    {
        printf("x4driver_set_sweep_trigger_control success\n");
    }
    else
    {
        printf("x4driver_set_sweep_trigger_control unknow situcation\n");
    }
#endif // DEBUG

    //    x4_initialize_status = tmp_status;

    status = x4driver_set_dac_min(x4driver, 949);
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error setting dac minimum\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_dac_min success\n");
#endif
    status = x4driver_set_dac_max(x4driver, 1100);
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error setting dac maximum\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_dac_max success\n");
#endif
    status = x4driver_set_iterations(x4driver, 32);
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error in x4driver_set_iterations\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_iterations success\n");
#endif
    status = x4driver_set_pulses_per_step(x4driver, 140);
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error in x4driver_set_pulses_per_step\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_pulses_per_step success\n");
#endif
    status = x4driver_set_downconversion(x4driver, 1); // Radar data as downconverted baseband IQ, not RF.
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error in x4driver_set_downconversion\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_downconversion success\n");
#endif

    status = x4driver_set_frame_area_offset(x4driver, 0); // Given by module HW. Makes frame_area start = 0 at front of module.
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error in x4driver_set_frame_area_offseto\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_frame_area_offset success\n");
#endif

    status = x4driver_set_frame_area(x4driver, 0, 2.5); // Observe from 0.5m to 4.0m.
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error in x4driver_set_frame_area\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
    printf("x4driver_set_frame_area success\n");

    status = x4driver_check_configuration(x4driver);
#ifdef DEBUG
    if (status == XEP_ERROR_X4DRIVER_OK)
    {
        printf("x4driver_check_configuration success\n");
    }
    else
    {
        printf("x4driver_check_configuration unknow situcation\n");
    }
#endif // DEBUG

    /***************set fps, this will trigger data output***************/
    status = x4driver_set_fps(x4driver, 1); // Generate 5 frames per second
    if (status != XEP_ERROR_X4DRIVER_OK)
    {
#ifdef DEBUG
        printf("Error in x4driver_set_fps\n");
        printf("Error code=%d\n", status);
#endif
        return 1;
    }
#ifdef DEBUG
    printf("x4driver_set_fps success\n");
#endif

    for (;;)
    {
    }
}
