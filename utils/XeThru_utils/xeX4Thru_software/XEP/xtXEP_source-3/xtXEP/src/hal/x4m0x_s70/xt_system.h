#ifndef XT_SYSTEM_H_
#define XT_SYSTEM_H_

/**
 * @brief Sets the processor in an idle state waiting for interrupt
 *
 * @return Status of execution
 */
int xt_idle_sleep(void);

/**
 * @brief Sets the processor in a deep sleep state. A power toggle or reset is required to wake it
 *
 * @return Status of execution
 */
int xt_deep_sleep(void);

/**
 * @brief Register callback for crash dump
 *
 * @return Status of execution as defined in @ref xt_error_codes_t
 */
int xt_register_crashdump_callback(int (*callback)(xt_swreset_reason_t,const char*,bool));

/**
 * @brief Enables or disables certification mode
 *
 * @return Status of execution as defined in @ref xt_error_codes_t
 */
int xt_enable_certification_mode(bool cert_mode);

/**
 * @brief Checks whether or not certification mode is enabled
 *
 * @return True for certification mode, false otherwise. 
 */
bool xt_is_in_certification_mode(void);

#endif