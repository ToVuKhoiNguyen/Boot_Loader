/***************************************************************************//**
 * @file app_run.h
 * @brief Common entry point declaration for application firmware.
 * @details Both Build Configurations (Slot_A and Slot_B) share the same
 *          main.c which calls APP_Run(). The actual implementation is
 *          provided by either app_a.c (Slot_A) or
 *          app_b.c (Slot_B), selected via Exclude from Build.
 ******************************************************************************/
#ifndef APP_RUN_H_
#define APP_RUN_H_

/***************************************************************************//**
 * @brief Application main loop entry point.
 * @note This function does not return — App1 or App2 depending on
 *       the active Build Configuration.
 ******************************************************************************/
void APP_Run(void);

#endif /* APP_RUN_H_ */
