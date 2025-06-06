# ESP32-S3 LED Matrix Display Project

这是一个基于 ESP32-S3 的 LED 矩阵显示项目，使用 HUB75 接口的 LED 矩阵面板。

## 硬件要求

- ESP32-S3 开发板
- HUB75 接口的 LED 矩阵面板 (64x64)
- USB 数据线
- 5V 电源适配器（用于 LED 矩阵供电）

## 引脚连接

| LED Matrix Pin | ESP32-S3 Pin |
|----------------|--------------|
| R1             | GPIO42       |
| G1             | GPIO41       |
| B1             | GPIO40       |
| R2             | GPIO38       |
| G2             | GPIO39       |
| B2             | GPIO37       |
| A              | GPIO45       |
| B              | GPIO36       |
| C              | GPIO48       |
| D              | GPIO35       |
| E              | GPIO21       |
| LAT            | GPIO47       |
| OE             | GPIO14       |
| CLK            | GPIO2        |

## 开发环境设置

1. 安装 ESP-IDF
   ```bash
   # 克隆 ESP-IDF
   git clone --recursive https://github.com/espressif/esp-idf.git
   
   # 运行安装脚本
   cd esp-idf
   ./install.sh
   
   # 设置环境变量
   . ./export.sh
   ```

2. 克隆本项目
   ```bash
   git clone https://github.com/BH4ME/esp32s3.git
   cd esp32s3
   ```

3. 配置项目
   ```bash
   idf.py menuconfig
   ```

4. 编译项目
   ```bash
   idf.py build
   ```

5. 烧录项目
   ```bash
   idf.py -p (PORT) flash
   ```

6. 监视串口输出
   ```bash
   idf.py -p (PORT) monitor
   ```

## 功能特性

- 支持 64x64 LED 矩阵显示
- 使用 DMA 进行数据传输
- 实时串口输出状态信息
- 可调节显示亮度

## 项目结构

```
esp32s3/
├── main/               # 主程序目录
│   ├── main.cpp       # 主程序文件
│   └── CMakeLists.txt # 主程序构建配置
├── components/        # 组件目录
├── CMakeLists.txt     # 项目构建配置
└── sdkconfig         # 项目配置文件
```

## 更新日志

### 2024-03-06
- 修复 RTOS 任务调度问题
  - 提高串口输出任务优先级（从1到5）
  - 增加任务堆栈大小（从2048到4096）
  - 添加任务创建错误检查
  - 添加任务状态调试输出
- 优化串口输出
  - 添加运行时间计数器
  - 改进输出格式
  - 增加任务启动提示

## RTOS 任务说明

项目使用 FreeRTOS 的抢占式调度机制，主要任务包括：

1. 串口输出任务（serial_output_task）
   - 优先级：5
   - 堆栈大小：4096 字节
   - 功能：每秒输出运行时间
   - 状态：持续运行

2. 主任务（app_main）
   - 功能：初始化硬件和创建其他任务
   - 状态：执行完成后返回

## 许可证

MIT License

## 作者

BH4ME (gaozu@chinaham.com)
