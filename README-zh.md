# simpletun演示程序使用说明

# 1. 编译步骤
（稍后补充）

# 2. 服务器端运行步骤
为节省时间，以下命令均以root账户登陆执行。

## 2.1 服务器主机创建TUN虚拟网口
下面的命令将创建名为tun0的虚拟网口，并设置静态IP地址10.9.8.7、子网掩码255.255.255.0：
```
ip tuntap add tun0 mode tun user root group root
ip link set tun0 up
ip addr add 10.9.8.7/24 dev tun0
```

## 2.2 启动simpletun服务器守护进程
选项说明：
- "-p 55555" 指定TCP端口号；
- "-s" s代表server，即服务器模式；
- "-u -i tun0" 指定虚拟网卡类型为TUN，名称为tun0;
- "-d" 打印调试信息；
```
./simpletun -d -u -i tun0 -s -p 55555
```

# 3. 客户端运行步骤

## 3.1 客户端主机创建TUN虚拟网口
客户端主机上也创建一个虚拟网口，并设置静态IP地址10.9.8.8、子网掩码255.255.255.0。
虚拟网口的名称可以随意指定，一般可以用tun0：
```
ip tuntap add tun0 mode tun user root group root
ip link set tun0 up
ip addr add 10.9.8.8/24 dev tun0
```

## 3.2 运行simpletun客户端
选项说明：
- "-p 55555" 对应服务器主机所开启TCP端口号；
- "-c 192.168.1.16" c代表connect或client，后面的IP必须是服务器的真实IP；
- "-u -i tun0" 指定虚拟网卡类型为TUN，名称为tun0;
- "-d" 打印调试信息；
```
./simpletun -d -u -i tun0 -c 192.168.1.16 -p 55555
```
