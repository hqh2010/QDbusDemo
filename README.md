# 说明

This is a demo for QDBus ＆ parse json file.

cmake version:3.13.4

qtcreator 4.8.2验证ok

cli侧的　--version暂时没有使用

# 使用方式

mkdir build

cd build

cmake ..

开Debug模式命令

cmake .. -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_DEBUG_ASAN=ON

make -j16

# 运行

先运行服务端service

然后运行客户端

cli query com.xunlei.download --version=1.2.3

id                      name            version         arch        description
com.xunlei.download     迅雷          1.0.0.2         x86_64      十八年技术沉淀，专注下载传输，成就用户信赖的必备工具”，迅雷团队首次推出 linux 界面版本，为国...

# 清理

make clean

# 参考文档

QDbus 数据类型

[https://blog.csdn.net/weixin_33909059/article/details/89630036]()

QDbus 传递复杂数据类型

[https://www.cnblogs.com/skogkatt/archive/2015/01/17/4231225.html](https://www.cnblogs.com/skogkatt/archive/2015/01/17/4231225.html)

关于获取、解析、分析dbus返回值 —— QDBus类

[https://blog.csdn.net/weixin_38627652/article/details/109638619](https://blog.csdn.net/weixin_38627652/article/details/109638619)

[https://www.cnblogs.com/chendeqiang/p/12861670.html](https://www.cnblogs.com/chendeqiang/p/12861670.html)

https://www.cnblogs.com/coderfenghc/archive/2012/10/20/2712806.html
