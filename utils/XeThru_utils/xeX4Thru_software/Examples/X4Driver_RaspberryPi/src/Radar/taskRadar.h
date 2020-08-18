#ifndef TASK_RADAR_H
#define TASK_RADAR_H

#ifdef __cplusplus
#include "x4driver.h"

/**
 * @brief Radar task.
 *
 * @return nothing
 */
int taskRadar(void);

/**
 * @brief Initialize Radar task.
 *
 * @return Status of execution
 */
uint32_t task_radar_init(X4Driver_t** x4driver);

/*
class appDebug_xep : public AbstractDebugOutput
{
public:
    appDebug_xep(XepDispatch_t *dispatch);

    virtual uint32_t send_byte_stream(uint32_t contentID, uint32_t info, uint8_t *data, uint32_t length);
    virtual void stop();

    // cpu
    virtual uint64_t getTick_hw() ;
    virtual uint64_t getTick_os() ;

    void setDebugLevel(uint8_t debug_level){debug_level_ = debug_level;}

private:
    uint8_t debug_level_;

    XepDispatch_t* dispatch_ = nullptr;
};
*/

#endif

#ifdef __cplusplus
#define EXPORT_C extern "C"
#else
#define EXPORT_C
#endif

//EXPORT_C void* AppDebug_xep_new(XepDispatch_t *dispatch);
#endif // TASK_RADAR_H
