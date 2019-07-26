# TCPTestTool
## a small tool to test  the performance and concurrency of tcp-server 
## [一款C++写的tcp性能测试（压力测试）工具-原博客园地址](https://www.cnblogs.com/xuhuajie/p/8213534.html)
## 相关说明：
* 1、本版本是在[RibbonServyou](https://github.com/RibbonServyou/TCPTestTool)的TCPTestTool的原有版本基础上修改的，使用VS2017开发工具，需要下载[Crypto++](https://cryptopp.com)加密和解密库的源代码，编译对应的库文件和添加对应的头文件到TCPTestTool项目中去。生成Crypto++库的lib、dll文件很简单，下载对应版本的源代码后，打开根目录下的cryptest.sln工程，重定向项目到当前的系统SDK版本，再编译对应的X86或者X64 lib和dll库，会在根目录下的Win32或者X64中生成我们需要的文件。根据自己的需求配置好Crypto++库，比如我配置的是使用VS2017编译的X86库，debug和release版本的。
* 2、打开原版本后，资源文件TcpTestTool.rc2、TcpTestTool.rc、TcpTestTool.ico会打不开，可以在TCPTestTool工程目录下新建一个res文件夹，将前面的TcpTestTool.rc2、TcpTestTool.rc、TcpTestTool.ico这三个文件移到res资源文件夹中，先将这3个文件从TCPTestTool工程中移除，然后再重新导入。
