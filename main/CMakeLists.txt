idf_component_register(SRCS         "main.c"
                                    "helpers.c"
                                    "settings.c"
                                    "libs/src/i2cdev.c"
                                    "libs/src/ads111x.c"
                                    "libs/src/gpio.c"
                    INCLUDE_DIRS    "."
                                    "libs/inc" 
                                    "../components/espressif__esp_modem" 
                                    "../components/ssd1306" 
                                    "../components/button"
                    EMBED_FILES     webpage/app.css 
                                    webpage/app.js 
                                    webpage/index.html
                                    webpage/favicon.ico 
                                    webpage/jquery-3.3.1.min.js)