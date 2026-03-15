# 电池数据分析系统

一款专业的电池电压数据可视化分析工具。

## 功能特点

- 📊 **热力图显示** - 8×52 电池电压彩色矩阵，一目了然
- 📈 **电压曲线** - 多组电压曲线实时显示，支持缩放
- 📋 **统计分析** - 最大/最小/平均/标准差
- 🖥️ **专业界面** - 深色主题，工业风格

## 数据格式

CSV 文件，格式如下：
```
time,time_zone,V1,V2,V3,...,V416
2026-3-15 0:0:4,UTC+08:00,3064,3066,3068,...,3066
```

- 8 组 × 52 电池 = 416 个电池单元
- 电压单位：mV

## 编译

### 方式一：GitHub Actions（推荐）

1. 将代码推送到 GitHub 仓库
2. 自动触发编译
3. 下载 `BatteryAnalyzer-Windows` artifact

### 方式二：本地编译

```bash
# 安装 Qt 6
# Windows: 下载 Qt Open Source
# Linux: sudo apt install qt6-base-dev qt6-charts-dev

# 编译
cd BatteryAnalyzer
qmake BatteryAnalyzer.pro
make

# 运行
./BatteryAnalyzer
```

## 使用方法

1. 运行程序
2. 点击工具栏「打开文件」
3. 选择 CSV 数据文件
4. 查看热力图、曲线和统计数据
5. 点击左侧电池组名称切换显示

## 系统要求

- Windows 10/11 (64位)
- 或 Linux (带 Qt6)
