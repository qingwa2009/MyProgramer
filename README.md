# MyProgramer
arduino uno(atmega328) CC2541蓝牙模块 编程器（可以烧hex跟bin文件）

目录说明：

    buildTool: 
        是从proteus那里牛过来的，mksketch.exe用来将ino转cpp，顺带编译arduino一堆库；
        MAKE文件夹也是从proteus那里牛过来的(●ˇ∀ˇ●)，make路径要加到环境变量里面去。

    cc2541: 
        芯片相关的乱七八糟都丢里面了，doc里面是芯片的一些数据手册，
        exampleHex里面blinkuTest是呼吸灯的程序，
        还有一个是从https://github.com/RedBearLab/CCLoader 牛过来的HM10蓝牙模块的bin文件，
        我找来找去都找不到哪些可以用的蓝牙模块文件。

    mylib: 
        自己写的一些库函数

编译环境搭建：

    我用的vscode编写代码，然后用proteus里面的make工具编译代码（好像是i386-pc-mingw32版本的）

    Makefile里面ARDUINO_PATH要替换成自己的路径（如：D:\Program Files\Arduino）

    Makefile里面UPLOAD_PORT要替换成自己的串口名

    make.exe的路径要加入环境变量Path（如：D:\Program Files\Proteus 8 Professional\Tools\MAKE）

    avr-gcc.exe的路径也要加入环境变量Path (如：D:\Program Files\Arduino\hardware\tools\avr\bin)

    ar.exe的路径也要加入环境变量Path（如：D:\Program Files\Arduino\hardware\tools\avr\avr\bin）

    上位机我用的nodejs+serialport，没有的需要安装下：
    装了nodejs后，在当前目录下cmd运行 npm install 就会自动安装serialport


CC2541接线：

    DC<---> 5 (DEBUG_CLOCK cc2541的P2_2引脚)

    DD<---> 6 (DEBUG_DATA cc2541的P2_1引脚)

    RST<---> 4 (cc2541的reset引脚)

    可以参考cc2541文件夹下的接线图，我连引脚都没焊，从电阻剪下一些线弯一下顶住o(*￣▽￣*)o
    引脚可以在myCC2541.c代码里面修改成自己要的。


关于电压，我arduino输出直接接蓝牙模块上面，没烧，还行，
电源跟接地也是直接接arduino的。

烧录cc2541：

    1、接好线

    2、cmd cd到当前目录 输入“make”编译

    3、输入“make upload”上传

    4、输入“node main .\cc2541\exampleHex\blinkTest.hex” 给cc2541烧录blinkTest.hex(支持hex跟bin文件)

我稀里糊涂的的把cc2541的调试功能都给写了：

调试cc2541：

    用串口调试助手，波特率设置250000，输入“/h”以回车换行(\r\n)结尾
    可以看到详细的使用说明，大概详细吧-_-!!! 如：

    /d 进入调试模式，mcu暂停，pc计数器指到0

    /r 退出调试模式，mcu正常运行

    /q 是开启或者关闭arduino串口输出，这样上位机可以直接跟cc2541串口通信

    ...

备份cc2541:

    输入“node mainCC2541backup [filename]” 备份cc2541的程序到bin文件，filename可以给也可以不给，
    默认当前目录下cc2541FlashBackup.bin文件；

    烧录前，能备份就备份吧，我的MLT-BT05模块没备份啥都没了，网上找来找去就找了个HM10蓝牙模块的bin文件可以用，
    功能感觉比MLT-BT05强，好像-_-!!!

    需要注意的是cc2541有个debug lock bit，就是防止别人调试，要是被设置了就备份不了了，备份不了时会提示的！
    debug lock bit只能通过清空flash来清除。

关于串口波特率：

    我设的是250000，用115200时运行备份程序老是备份不了，少好多个字节，
    大概是115200的波特率有错误率，所以设了250000，设成9600也可以，就是速度太慢了。

可能会遇到一些问题：

    偶尔死活烧录不进去，具体未知，可以用串口调试助手:
    /d进入调试模式/gi获取id/r退出调试，来回折腾几下，或者拔掉地线再接回去，就又可以了。

演示：
    https://www.bilibili.com/video/BV1K94y1Q7iS

atmega328接线:

    ...

烧录atmega328：

    ...




