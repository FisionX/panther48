idf_component_register(
    SRCS "panther.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES mygpio mytimer led_driver
)
