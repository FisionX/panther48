/** @file application.c
 *  @details Application behavior: toggle the LED every second.
 */
#include "application.h"
#include "panther.h"

// Set up the board and start toggling the LED every second.
esp_err_t application_init(void)
{
    // Prepare the board before starting the timer.
    esp_err_t ret = panther_init();
    if (ret != ESP_OK) {
        return ret;
    }
    // Toggle every second: 1 second ON, then 1 second OFF.
    // The application chooses the interval; the BSP chooses the LED pin.
    return panther_led_blink_start(1000000UL);
}
