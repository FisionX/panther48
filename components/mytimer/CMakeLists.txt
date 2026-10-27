idf_component_register( SRCS "src/mytimer.c" "hal/timer_hal.c"
                INCLUDE_DIRS "include"
                PRIV_INCLUDE_DIRS "hal"
                PRIV_REQUIRES esp_hw_support xtensa)
