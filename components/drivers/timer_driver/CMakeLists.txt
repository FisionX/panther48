idf_component_register(
    SRCS "timer_driver.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_common esp_hw_support timer_hal
)
