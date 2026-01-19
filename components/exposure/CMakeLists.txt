idf_component_register(
    SRCS "exposure.cpp"
    INCLUDE_DIRS "include"
    REQUIRES board_config store driver esp_timer
)