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

## 许可证

MIT License

## 作者

BH4ME (gaozu@chinaham.com)
