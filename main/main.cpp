#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
MatrixPanel_I2S_DMA *dma_display = nullptr;
void serial_output_task(void* pvParameters)
{
    uint32_t seconds = 0;  // 添加秒数计数器
    printf("Serial output task started!\n");
    while (1) {
        printf("Seconds elapsed: %lu\n", seconds++);  // 输出并递增秒数
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时1秒
    }
}
extern "C" void app_main() {
    // 定义引脚映射
    HUB75_I2S_CFG::i2s_pins _pins = {
        42,  // R1
        41,  // G1
        40,  // B1
        38,  // R2
        39,  // G2
        37,  // B2
        45,  // A
        36,  // B
        48,  // C
        35,  // D
        21,  // E
        47,  // LAT
        14,  // OE
        2    // CLK
    };

    // 创建显示屏配置
    HUB75_I2S_CFG mxconfig(
        64,     // 宽度
        64,     // 高度
        1,      // 链长度
        _pins   // 引脚映射
    );

    // 创建显示屏对象
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    if (dma_display == nullptr) {
        printf("Failed to create display!\n");
        return;
    }

    // 初始化显示屏
    if (!dma_display->begin()) {
        printf("Failed to initialize display!\n");
        return;
    }

    // 设置亮度 (0-255)
    dma_display->setBrightness8(80);

    // 清空屏幕
    dma_display->clearScreen();

    // 绘制一个测试图案
    dma_display->fillScreen(0x0000);  // 黑色背景
    
    // 绘制边框
    for(int i = 0; i < 64; i++) {
        dma_display->drawPixel(i, 0, 0xFFFF);      // 上边框
        dma_display->drawPixel(i, 63, 0xFFFF);     // 下边框
        dma_display->drawPixel(0, i, 0xFFFF);      // 左边框
        dma_display->drawPixel(63, i, 0xFFFF);     // 右边框
    }

    // 绘制对角线
    for(int i = 0; i < 64; i++) {
        dma_display->drawPixel(i, i, 0xF800);      // 红色对角线
        dma_display->drawPixel(i, 63-i, 0x07E0);   // 绿色对角线
    }

        // ... (显示屏初始化和绘图代码) ...

    // 创建并启动串口输出任务
  // 创建并启动串口输出任务
    TaskHandle_t serial_task_handle = NULL;
    BaseType_t task_created = xTaskCreate(
        serial_output_task,   // 任务函数
        "serial_out",         // 任务名称
        4096,                 // 增加堆栈大小到4096
        NULL,                 // 任务参数
        5,                    // 提高优先级到5
        &serial_task_handle   // 保存任务句柄
    );

    if (task_created != pdPASS) {
        printf("Failed to create serial output task!\n");
        return;
    }

    printf("Serial output task created successfully!\n");
    //     while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }


}
