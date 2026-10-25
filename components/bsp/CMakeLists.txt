idf_component_register(
    SRCS "panther48.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common esp_hw_support led_driver timer_driver
)
