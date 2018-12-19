# Automatic Puzzle Solver
## About Hardware ##
![CyberDIP](/qtCyberDIP-master/pic/CyberDIP.png)

CyberDIP driver for windows in C++ 11.

CyberDIP is a USB device clicking touchscreens of smartphones or pads under PC's control. 
With the help of [grbl](https://github.com/grbl/grbl) running on an Arduino Nano, CyberDIP translates serial port signals from your PC into 2D-motion and clicks.
Combined with computer vision, CyberDIP can play many games autonomously on almost any devices with a touchscreen.

本工程是依赖C++ 11标准编写的CyberDIP在Windows环境下的配套软件。

CyberDIP是通过计算机USB控制的触摸屏点击设备(中国实用新型专利2016201772460)，通过搭载[grbl 0.8c/0.9j](https://github.com/grbl/grbl)的Arduino Nano(ATmega328)控制器，CyberDIP可以将USB串口发来的指令翻译成相应的二维运动与点击操作，模拟单指对屏幕的操作。
结合图像处理算法，CyberDIP可以实现触屏手机上游戏的自动攻略功能。

## About Software  ##
### Dependence
* [Qt](https://www.qt.io/) ≥5.7
* [FFmpeg](https://ffmpeg.org/) 2.2.2 (included in [/3rdparty](/3rdparty))
* [QTFFmpegWrapper](https://inqlude.org/libraries/qtffmpegwrapper.html) (included in [/qtCyberDip/QTFFmpegWrapper](/qtCyberDip/QTFFmpegWrapper))
* [Android Debug Bridge](http://developer.android.com/tools/help/adb.html) (included in [/qtCyberDip/prebuilts](/qtCyberDip/prebuilts))
* (Optional)[OpenCV](http://www.opencv.org/) ≥2.4.9

### Framework
qtCyberDIP contains 4 + 1 parts:
* __Serial Port(COM)__: Serial port and g-code module to work with Arduino.
* __[BBQScreen Client](https://github.com/xplodwild/bbqscreen_client)__: BBQScreen client module to decode frames from Android devices.
* __Screen Capture__: Win32 API module to grab screen or windows, working with [AirPlayer](http://pro.itools.cn/airplayer) for iOS devices and [Total Control](http://tc.sigma-rt.com.cn/index.php) for Android devices.
* __Video Input__: Read image sequences from files.
* __Game Controller__: User defined game controller, change this to play different games.

### Usage
* Install Visual Studio (≥2013)
* Install Qt and Qt VS Add-In
* Install git for windows
* Clone this project using script below at git bash:
```
git clone https://github.com/leofansq/Automatic-Puzzle-Solver.git
```
* (If necessary)Install Driver for CH340 Chipset after CyberDIP connected. ([Driver](/CH340) is included)
* Add '3rdparty/x64' (Default, if you are using 32-bit OS, add '3rdparty/x86') into Environment Variable 'Path' (__ATTENTION! NEVER CLEAN 'Path'__, unless you are exactly conscious of the consequence)
* Open [qtCyberDip.sln](/qtCyberDip.sln) in Visual Studio and follow the instructions in [stdafx.h](qtCyberDip/stdafx.h)

### 使用方法
* 安装 Visual Studio 2013或更高版本
* 安装 Qt 与相应版本的 Qt-VS插件
* 在一个恰当的目录中打开git bash并输入以下命令：
```
git clone https://github.com/leofansq/Automatic-Puzzle-Solver.git
```
* 连接CyberDIP，如有必要则安装CH340 芯片组驱动，驱动文件包含在[CH340](/qtCyberDIP-master/CH340)文件夹中。
* 将 '3rdparty/x64' (默认x64，32位操作系统添加 '3rdprty/x86')加入系统环境变量 'Path' 中(注意不要清空原有内容)
* 使用VS打开工程文件 [qtCyberDip.sln](/qtCyberDIP-master/qtCyberDip.sln), 并按照 [stdafx.h](/qtCyberDIP-master/qtCyberDip/stdafx.h) 中的说明继续配置

## About Project ##
- All the code for the algorithm is in the [usrGameController.h](/qtCyberDIP-master/qtCyberDip/usrGameController.h) 和 [usrGameController.cpp](/qtCyberDIP-master/qtCyberDip/usrGameController.cpp) two files
-  In the process, we have tried several methods.Some of the operable versions  are stored in folder [previous-version](/previous-version)

## Acknowledgement ##
The source of this project is SJTU digital image processing. The main hardware and software are done by course teachers and teaching assistants. I and the team members only completed the algorithm part of the puzzle implementation. Original project address: https://github.com/LostXine/qtCyberDIP.git
