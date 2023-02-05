## TrackPoint 78
xxxx
### 工程目录
xxxx
### 关于固件升级
Q: 如何进入BOOTLOADER？
A: 按住底板中BOOT键通电，或按下Fn+B进入BOOTLOADER。注意：Fn+B后会擦除部分CodeFlash部分片段，上电后必须先下载程序。
Q: 如何升级固件？
A: 使用WCH官方ISP工具进行固件升级，工具目录：tools/WCHISPTool/WCHISPTool.exe。
   1. 选择CH58x系列；
   2. 在USB设备列表里找到自己的设备，若找不到尝试重新进入BOOT；
   3. 根据需求勾选相关下载配置，若不确定可进行咨询，一般使用默认配置即可；
   4. 选择用户程序文件；
   5. 点击下载。
Q: 多次点击下载后，进度条跑完显示失败。
A: 建议更换核心板。
