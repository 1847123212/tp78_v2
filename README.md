<img title="" src="resources/logo.png" alt="sd" width="152" data-align="center">

<h1 align="center">TrackPoint78 </h1>

TrackPoint78是基于CH582M的三模机械键盘方案。

V2.0新特性：

1. 支持USB U盘改键和配置

2. 支持触摸板触摸条操作

3. 集成USB hub接口

4. 增加I2C磁吸扩展口

5. 集成RF模式和接收器方案

6. 增加I2C小红点方案

7. 增加触摸条震动反馈

8. 优化OELD UI

9. 增加Fn+B进BootLoader功能

10. 增加待机自动进lowpower模式

11. ...

### 仓库目录

```mermaid
flowchart LR

A(repository) --> B(documents - 手册和文档)
A(repository) --> C(mounriver_project - wch软件工程)
A(repository) --> D(pcb_project - PCB工程)
A(repository) --> E(solidworks_project - 模型相关文件)
A(repository) --> F(tools - 相关辅助工具)
C(mounriver_project - wch软件工程) --> G(BLE_CH582M - 三模无线键盘工程)
C(mounriver_project - wch软件工程) --> H(RF_CH582M - 配套接收器工程)
C(mounriver_project - wch软件工程) --> I(CH582M - 单模USB旧工程, 用来测试)
```

### 软件工程目录

```mermaid
flowchart LR

A(BLE_582M) --> B(APP - 应用层)
A(BLE_582M) --> C(HAL - HAL层, 包含各种驱动接口)
A(BLE_582M) --> D(Ld - 链接文件)
A(BLE_582M) --> E(LIB - 存放WCH蓝牙闭源库)
A(BLE_582M) --> F(Packages - 第三方开源包)
A(BLE_582M) --> G(Profile - 蓝牙服务相关)
A(BLE_582M) --> H(RVMSIS - RISC V相关指令)
A(BLE_582M) --> I(Startup - CH582M启动文件)
A(BLE_582M) --> J(StdPeriphDriver - CH582M库函数)
```

### 关于固件升级Q&A

- **Q**: 如何进入BOOTLOADER？

- **A:** 按住底板中BOOT键通电，或按下Fn+B进入BOOTLOADER。注意：Fn+B后会擦除部分CodeFlash部分片段，上电后必须先下载程序。

- **Q:** 如何升级固件？

- **A:** 使用WCH官方ISP工具进行固件升级，工具目录：tools/WCHISPTool/WCHISPTool.exe。
  
  > - 选择CH58x系列；
  > 
  > - 在USB设备列表里找到自己的设备，若找不到尝试重新进入BOOT；
  > 
  > - 根据需求勾选相关下载配置，若不确定可进行咨询，一般使用默认配置即可；
  > 
  > - 选择用户程序文件；
  > 
  > - 最后点击下载。

- **Q:** 多次点击下载后，进度条跑完显示失败。

- **A:** 建议更换核心板。
