idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES
                        driver
                        esp_driver_gpio
                        mygpio
                        timer_hal
                        timer_driver
                        led_driver
                        bsp
                        app)