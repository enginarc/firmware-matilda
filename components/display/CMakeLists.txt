idf_component_register(
    SRCS "display.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd lvgl esp_lvgl_port driver           # Use the built-in LCD component
    PRIV_REQUIRES board_config   # Your local pin definitions
)