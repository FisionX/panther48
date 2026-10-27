idf_component_register(
    SRCS "panther.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_hw_support led_driver
)
