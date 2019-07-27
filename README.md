# TCPTestTool 一款C++写的tcp性能测试（压力测试）工具
## [一款C++写的tcp性能测试（压力测试）工具-原博客园地址](https://www.cnblogs.com/xuhuajie/p/8213534.html)
## 相关说明：
### 1、本版本是在[RibbonServyou](https://github.com/RibbonServyou/TCPTestTool)的TCPTestTool的原有版本基础上修改的，使用VS2017开发工具。
### 2、添加依赖库- 加密库 Crypto++
* (1).需要下载[Crypto++](https://cryptopp.com)加密和解密库的源代码，编译对应的库文件和添加对应的头文件到TCPTestTool项目中去。生成Crypto++库的lib、dll文件很简单，下载对应版本的源代码后，打开根目录下的cryptest.sln工程，重定向项目到当前的系统SDK版本，再编译对应的X86或者X64 lib和dll库，会在根目录下的Win32或者X64中生成我们需要的文件。根据自己的需求配置好Crypto++库，比如我配置的是使用VS2017编译的X86库，debug和release版本的。
编译好的cryptopp564的头文件和lib文件我已经上传到cryptopp564子目录下了，分别包含include头文件目录、lib库文件目录和bin目录，注意：由于Github上面单个文件最大上传大小为25M，我把几个lib库文件压缩成了.7z格式，需要使用7z软件解压缩。
* (2).属性配置文件 cryptopp564_x86_debug.props和cryptopp564_x86_release.props
为了配置方便，可以使用属性配置文件.props，我在自己的电脑上把第三方库一般放在D:\env_build目录下，可以把Github上的cryptopp564文件夹拷贝到自己电脑上的任意目录，如果是编译的是X86 Debug,则修改对应的属性文件cryptopp564_x86_debug.props，分别修改include和lib的对应路径；如果是x86 release的，则修改对应的属性文件cryptopp564_x86_release.props、
* (3).关于VS2017下编译Crypto++，可以参考[hddong97的CSDN博客-VS2017编译Crypto++](https://blog.csdn.net/weixin_42314534/article/details/81840131)
和[Crypto++编译使用](https://www.cnblogs.com/findumars/p/6009249.html)
### 3、打开原版本后，资源文件TcpTestTool.rc2、TcpTestTool.ico会打不开，可以在TCPTestTool工程目录下新建一个res文件夹，将前面的TcpTestTool.rc2、TcpTestTool.ico这2个文件移到res资源文件夹中，先将这2个文件从TCPTestTool工程中移除，然后再重新导入。
