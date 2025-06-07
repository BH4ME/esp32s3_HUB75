idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES ESP32-HUB75-MatrixPanel-I2S-DMA esp_timer driver freertos esp_common)

target_compile_options(${COMPONENT_LIB} PRIVATE 
    -fpermissive
    -Wno-attributes
    -Wno-pragmas
    -Wno-warning-option
    -fno-strict-volatile-bitfields
    -fno-tree-switch-conversion
    -mlongcalls
    -fno-shrink-wrap
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-unused-but-set-parameter
    -Wno-unused-value
    -Wno-unused-label
    -Wno-unused-dummy-argument
    -Wno-unused-macros
    -Wno-unused-local-typedefs
    -Wno-unused-result
)
