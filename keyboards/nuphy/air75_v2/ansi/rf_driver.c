#include "host_driver.h"
#include "rf_driver.h"
#include "host.h"

/* Host driver */
static uint8_t rf_keyboard_leds(void);
static void    rf_send_keyboard(report_keyboard_t *report);
static void    rf_send_nkro(report_nkro_t *report);
static void    rf_send_mouse(report_mouse_t *report);
static void    rf_send_extra(report_extra_t *report);
host_driver_t  rf_host_driver = {rf_keyboard_leds, rf_send_keyboard, rf_send_nkro, rf_send_mouse, rf_send_extra};

/* defined in rf.c */
extern void uart_send_report_func(void);
extern void uart_send_mouse_report(void);
extern void uart_send_consumer_report(void);
extern void uart_send_system_report(void);

static uint8_t rf_keyboard_leds(void) {
    // TODO: this returns the LED state for caps locks and whatnot?
    return 0;
}
static void rf_send_keyboard(report_keyboard_t *report) {
    uart_send_report_func();
}

static void rf_send_nkro(report_nkro_t *report) {
    uart_send_report_func();
}

static void rf_send_mouse(report_mouse_t *report) {
    uart_send_mouse_report();
}

static void rf_send_extra(report_extra_t *report) {
    if (report->report_id == REPORT_ID_CONSUMER) {
        uart_send_consumer_report();
    } else if (report->report_id == REPORT_ID_SYSTEM) {
        uart_send_system_report();
    }
}