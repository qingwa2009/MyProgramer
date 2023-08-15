# MyProgramer
arduino uno(atmega328) programer 
    
    CC2541蓝牙模块 编程器
        * .hex 文本格式烧写flash
        * .bin 二进制格式烧写flash
        
    ATmega328 编程器
        * .hex 文本格式烧写flash
        * .bin 二进制格式烧写flash
        * .eep 文本格式烧写eeprom
        * .eepb 二进制格式烧写eeprom

目录说明：

    buildTool: 
        是从proteus那里牛过来的，mksketch.exe用来将ino转cpp，顺带编译arduino一堆库；
        MAKE文件夹也是从proteus那里牛过来的(●ˇ∀ˇ●)，make路径要加到环境变量里面去。

    cc2541: 
        cc2541芯片相关的乱七八糟都丢里面了，doc里面是芯片的一些数据手册，
        exampleHex里面blinkuTest是呼吸灯的程序，
        还有一个是从https://github.com/RedBearLab/CCLoader 牛过来的HM10蓝牙模块的bin文件，
        我找来找去都找不到哪些可以用的蓝牙模块文件。

    atmega328:
        atmega328芯片相关的乱七八糟都丢里面了。

    mylib: 
        自己写的一些库函数

    preBuild:
        已经编译好了的atmega328跟cc2541的编程器hex文件，直接用就可以了不用自己再编译一遍了：
        命令行直接输入：
        make upload UPLOAD_FILE=prebuild/cc2541.hex
        或者
        make upload UPLOAD_FILE=prebuild/atmega328.hex
        就可以将UNO烧录成对应的编程器了。

编译环境搭建：

    我用的vscode编写代码，然后用proteus里面的make工具编译代码（好像是i386-pc-mingw32版本的）

    Makefile里面ARDUINO_PATH要替换成自己的路径（如：D:\Program Files\Arduino）

    Makefile里面UPLOAD_PORT要替换成自己的串口名

    make.exe的路径要加入环境变量Path（如：D:\Program Files\Proteus 8 Professional\Tools\MAKE）

    avr-gcc.exe的路径也要加入环境变量Path (如：D:\Program Files\Arduino\hardware\tools\avr\bin)

    ar.exe的路径也要加入环境变量Path（如：D:\Program Files\Arduino\hardware\tools\avr\avr\bin）

    上位机我用的nodejs+serialport，没有的需要安装下：
    装了nodejs后，在当前目录下cmd运行 npm install 就会自动安装serialport

    Makefile里面下面两句分别对应编译cc2541跟atmega328编程器，注释掉其中一句就可以了。
    # TARGET_MCU_FILE = mcu_cc2541.cpp
    # TARGET_MCU_FILE = mcu_atmega328.cpp
    也可以不用改，在命令行输入“make TARGET_MCU_FILE=xxxxx.cpp”这样直接指定编译哪个芯片也可以。
    需要注意的是因为编译后输出的文件是一样的，所以切换到另一个时编译之前要先make clean



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

    2、cmd cd到当前目录 输入“make clean”清空之前的编译（可选）

    3、输入“make TARGET_MCU_FILE=mcu_cc2541.cpp”编译

    4、输入“make upload”上传

    5、输入“node main .\cc2541\exampleHex\blinkTest.hex” 给cc2541烧录blinkTest.hex(支持hex跟bin文件)

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

关于蓝牙模块烧录TI官方的示例程序：

    由于蓝牙模块的硬件跟示例程序的配置可能不一致，导致烧录进去无法正常运行。比较典型的情况就是：有些蓝牙模块《没焊外部32k晶振》-_-!!!，而示例程序应该全是默认存在在外部32k晶振编译的程序，所以导致程序运行不下去。
    下面提供一些我用到的预编译宏，直接加到IAR->工程设置->c/c++ Compiler->Preprocessor->Defined symbols的框里面，注释不要写进去，跟已有的有冲突的话就删掉已有的：
    HAL_KEY=FALSE             
    HAL_LED=TRUE               
    HAL_LCD=FALSE              
    HAL_AES_DMA=TRUE            
    HAL_DMA=TRUE                			
    HAL_UART_ISR=0              
    HAL_UART=TRUE               
    HAL_UART_DMA=2			//使用port1的串口
    NPI_UART_FC=FALSE		//关闭串口流控制
    XOSC32K_INSTALLED=FALSE	//使用内部32k晶振

关于串口波特率：

    我设的是250000，用115200时运行备份程序老是备份不了，少好多个字节，
    大概是115200的波特率有错误率，所以设了250000，设成9600也可以，就是速度太慢了。

可能会遇到一些问题：

    偶尔死活烧录不进去，具体未知，可以用串口调试助手:
    /d进入调试模式/gi获取id/r退出调试，来回折腾几下，或者拔掉地线再接回去，就又可以了。

演示：
    https://www.bilibili.com/video/BV1K94y1Q7iS

atmega328接线:

    参见atmega328文件夹里面的接线图。

    其中晶振我是没按数据手册那样接电容电阻的，接上反而有问题，有点谜。UNO的2号接芯片的1脚；3号是编程模式会输出8M的pwm信号，退出编程模式则会关闭pwm信号输出，用来给芯片提供外置时钟，防止时钟熔丝设置错误导致芯片无法编程；uno的11，12，13就是spi的接线接芯片对应的引脚就行；芯片的电源跟地就跟uno的电源地接一起就可以了。

烧录atmega328：

    1、接好线

    2、cmd cd到当前目录 输入“make clean”清空之前的编译（可选）

    3、输入“make TARGET_MCU_FILE=mcu_atmega328.cpp”编译

    4、输入“make upload”上传到uno

    5、输入“node main x\xxxx\xxxx.hex” 给atmega328烧录程序(支持hex跟bin文件，也可以烧录.eep（跟hex一样）到eeprom或者.eepb（跟bin一样）到eeprom)，烧录eeprom不会清空flash，烧录flash根据熔丝位芯片会自动清空或者保留eeprom（详细见数据手册Fuse High Byte for ATmega328P）

设置atmega328熔丝位：

    用串口调试助手，波特率设置250000，输入“/h”以回车换行(\r\n)结尾
    可以看到详细的使用说明，大概详细吧-_-!!! 如：

    /d 进入调试模式，mcu暂停，pc计数器指到0

    /r 退出调试模式，mcu正常运行

    /q 是开启或者关闭arduino串口输出，这样上位机可以直接跟atmega328串口通信

    /sfl xx 设置低熔丝位，时钟源相关的就是在这里
    
    ....

演示：
    https://www.bilibili.com/video/BV1me4y1C7f6


