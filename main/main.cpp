#include <time.h>
#include <esp_timer.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

MatrixPanel_I2S_DMA *dma_display = nullptr;

extern "C" void app_main() {
  // 初始化串口
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT
  };
  
  ESP_ERROR_CHECK(uart_driver_install(static_cast<uart_port_t>(UART_NUM_0), 256, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(static_cast<uart_port_t>(UART_NUM_0), &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(static_cast<uart_port_t>(UART_NUM_0), 17, 18, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  
  ESP_LOGI("MAIN", "ESP32-S3 HUB75 LED Matrix Demo");

  HUB75_I2S_CFG mxconfig(/* width = */ 64, /* height = */ 64, /* chain = */ 1);

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(80);
  dma_display->clearScreen();
  
  ESP_LOGI("MAIN", "LED Matrix initialized successfully!");

  // 创建一个任务来每秒输出信息
  xTaskCreatePinnedToCore([](void *pvParameters) {
    int count = 0;
    while(1) {
      ESP_LOGI("TASK", "系统运行时间: %d 秒", count++);
      vTaskDelay(pdMS_TO_TICKS(1000));  // 使用 pdMS_TO_TICKS 宏来转换时间
    }
  }, "print_task", 4096, NULL, 5, NULL, 0);  // 固定在核心0上运行，增加栈大小到4096

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(100));  // 添加100ms延时，让其他任务有机会运行
  }
}
