# npou
软件包管理器

# 安装
``` bash
$ git clone https://github.com/naucye/npou.git

$ cd npou

$ sudo make build && make install
```

# 介绍
npou是一款面向自由的软件包管理工具，任何人都有机会将自己的软件包发布上来，让npou帮你管理。npou软件包格式为npu。

# 声明
目前的版本还未完善，还未正式投入使用！

# 使用

## npou
软件包管理工具
 ``` bash
$ npou -Ud # 更新数据库
$ npou -S [软件包] # 安装软件包 
```


## npu
软件包打包工具
 ``` bash
$ npu -f [软件包] -g # 查看软件包信息

$ npu build [软件包源码目录] # 将源码打包为npu文件
```

## npudb
软件包数据库工具
``` bash
$ npudb -f [npu配置文件] -d [操作的数据库文件] -p [npu文件] # 将npu文件信息写入数据库
```
# 卸载
``` bash
$ sudo make uninstall # 卸载安装的文件

$ sudo make remove # 删除当前目录所有文件（拉取的源代码）

$ sudo make uninstall && make remove # 删除当前目录所有文件和安装所有文件 
```