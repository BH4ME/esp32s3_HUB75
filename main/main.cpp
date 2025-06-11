#include <time.h>
#include <esp_timer.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include <math.h>
#include "../fall_animation.h"

MatrixPanel_I2S_DMA *dma_display = nullptr;

// 定义颜色结构体
struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// 表情尺寸和位置定义
#define EMOJI_WIDTH  32
#define EMOJI_HEIGHT 32
#define EMOJI_X_OFFSET (64 - EMOJI_WIDTH) / 2  // 面板宽度 (64) - 表情宽度 / 2
#define EMOJI_Y_OFFSET (32 - EMOJI_HEIGHT) / 2 // 面板高度 (32) - 表情高度 / 2

// 预定义颜色 (RGB565 格式)
#define COLOR_YELLOW 0xFFE0 // RGB(255, 255, 0)
#define COLOR_BLACK  0x0000 // RGB(0, 0, 0)

// 调整颜色亮度
uint32_t adjustColor(uint32_t color) {
    // 提取RGB和Alpha值
    uint8_t alpha = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    // 检查是否是白色（RGB都接近255）
    if (r > 240 && g > 240 && b > 240) {
        return color;  // 保持白色不变
    }
    
    // 对于非白色，增加亮度但保持颜色关系
    if (r > 0 || g > 0 || b > 0) {  // 只要有一个通道不为0
        // 增加每个通道的亮度，但保持相对关系
        if (r > 0) r = (r * 2.0 > 255) ? 255 : r * 2.0;
        if (g > 0) g = (g * 2.0 > 255) ? 255 : g * 2.0;
        if (b > 0) b = (b * 2.0 > 255) ? 255 : b * 2.0;
    }
    
    // 重新组合颜色值
    return (alpha << 24) | (r << 16) | (g << 8) | b;
}

// 生成波浪颜色
RGB getWaveColor(int x, int y, float time) {
    // 使用正弦函数创建波浪效果
    float wave1 = sin(x * 0.1 + time) * 0.5 + 0.5;
    float wave2 = sin(y * 0.1 + time * 0.7) * 0.5 + 0.5;
    float wave3 = sin((x + y) * 0.1 + time * 1.3) * 0.5 + 0.5;
    
    RGB color;
    color.r = (uint8_t)(wave1 * 255);
    color.g = (uint8_t)(wave2 * 255);
    color.b = (uint8_t)(wave3 * 255);
    
    return color;
}

// 动画效果函数 - 波浪动画
void draw_wave_animation_effect() {
    static float time = 0.0f; // 保持时间状态
    
    // 更新整个屏幕
    for(int y = 0; y < dma_display->height(); y++) {
        for(int x = 0; x < dma_display->width(); x++) {
            // 使用正弦函数创建波浪效果
            float wave1 = sin(x * 0.1 + time) * 0.5 + 0.5;
            float wave2 = sin(y * 0.1 + time * 0.7) * 0.5 + 0.5;
            float wave3 = sin((x + y) * 0.1 + time * 1.3) * 0.5 + 0.5;
            
            // 使用库的color565函数生成颜色
            uint16_t color = dma_display->color565(
                (uint8_t)(wave1 * 255),
                (uint8_t)(wave2 * 255),
                (uint8_t)(wave3 * 255)
            );
            
            dma_display->drawPixel(x, y, color);
        }
    }
    
    time += 0.1f;  // 控制动画速度
}

// 动画任务
void animation_task(void *pvParameters) {
    uint8_t current_frame = 0;  // 当前帧索引
    
    while(1) {  // 无限循环
        // 显示当前帧
        dma_display->clearScreen(); // 清屏
        for (int y = 0; y < EMOJI_HEIGHT; y++) {
            for (int x = 0; x < EMOJI_WIDTH; x++) {
                uint32_t pixel = fall_data[current_frame][y * EMOJI_WIDTH + x];
                
                // 调整颜色亮度
                pixel = adjustColor(pixel);
                
                // 直接使用原始颜色值
                dma_display->drawPixel(EMOJI_X_OFFSET + x, EMOJI_Y_OFFSET + y, pixel);
            }
        }
        
        // 更新帧索引
        current_frame = (current_frame + 1) % FALL_FRAME_COUNT;
        
        // 每帧约62.5ms
        vTaskDelay(pdMS_TO_TICKS(62));
    }
}

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

    // 定义HUB75引脚映射结构体，包含E引脚
    HUB75_I2S_CFG mxconfig(
        64, // width
        32, // height
        1,  // chain length
        {
            R1_PIN_DEFAULT, G1_PIN_DEFAULT, B1_PIN_DEFAULT,
            R2_PIN_DEFAULT, G2_PIN_DEFAULT, B2_PIN_DEFAULT,
            A_PIN_DEFAULT, B_PIN_DEFAULT, C_PIN_DEFAULT, D_PIN_DEFAULT,
            17, // E引脚设置为GPIO17
            LAT_PIN_DEFAULT, OE_PIN_DEFAULT, CLK_PIN_DEFAULT
        },
        HUB75_I2S_CFG::FM6124, // 使用 FM6126A 驱动
        false,    // double buffer
        HUB75_I2S_CFG::HZ_20M,    // 提高时钟频率到20MHz
        1,        // 减小 latch_blanking 到1
        true,     // clock phase
        100,      // 降低刷新率到100Hz以提高亮度
        32        // 将颜色深度改为32位
    );

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(255);  // 已经是最大亮度了
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

    // 创建动画任务
    xTaskCreatePinnedToCore(animation_task, "animation_task", 4096, NULL, 5, NULL, 1);  // 在核心1上运行

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));  // 添加100ms延时，让其他任务有机会运行
    }
}
