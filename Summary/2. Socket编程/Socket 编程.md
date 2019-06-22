# Socket 编程

## socket 常见 API

### socket

创建 socket 文件描述符。

适用于 TCP/UDP 协议。

这个函数是服务器和客户端都需要用。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615113319810.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

**参数：**

- `domain`: 这是需要填一个宏，表示要用什么协议。例如这里填 `AF_INET`表示创建的`socket`使用 `ipv4`协议。

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615113535364.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

- `type`: 这里也是填一个宏，表示需要用什么协议。例如填 `SOCK_DGRAM`表示使用`UDP`协议。

  ![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615113806266.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

- `protocol`: 这个一般填0。

`socket` 本质是一个文件描述符创建`socket` 本质上就是创建一个文件（叫做`socket`类型的文件）。

这个文件不适合磁盘上的文件对应，而是和网卡这样的设备对应读文件就是从网卡上读数据，写文件就是往网卡上写数据（管道）也是这么搞的。

`socket`用完之后也要及时`close`，否则也会发生文件描述符泄漏。

### bind

绑定端口号。把一个 socket 和一个端口号关联在一起，主要是因为，当客户端要访问服务器的时候，需要知道服务器进程的进程id，但是一个普通的进程每次运行的时候 进程 id都是不一样的。所以我们需要给服务器程序一个固定的id，也就是端口号。把一个固定的端口号和创建的文件描述符绑定在一起，就是bind函数的作用。

适用于 TCP/UDP 协议。

主要是服务器使用，客户端也能用，但是一般不这么用。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615115000725.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

**参数：**

- `sockfd`: 就是上个函数创建的文件描述符。
- `addr`: 这个参数非常坑。这是一个通用的结构体，然后我们如果用 TCP/UDP 的话需要用创建`sockaddr_in`这个结构体，给它赋值，然后再强转成`sockaddr`进行函数传参。
- `addrlen`: 上面那个结构体的大小。

**返回值：**

单独说一下这个函数的返回值，因为这个函数如果成功，就会返回一个新的socket文件描述符。这个新的文件描述符去和客户端进行连接。而原来的文件描述符继续去等待新的客户端发来连接请求。

### listen

这个函数是给服务器用的，一般来说被动的一方叫服务器，主动地一方是客户端。这个函数就是让服务器进入监听状态，如果有客户端进行连接，他就把一个连接放到一个队列中去。等待下一次操作。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615120056657.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

**参数：**

`sockfd`: 创建的文件描述符。

`backlog`: 这个就是允许连接的最大数量。换句话说就是哪个阻塞队列的最大长度。

### accept

这个函数也是给服务器用的，就是服务器从那个阻塞队列中获取一个连接。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615120842875.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

**参数：**

- `sockfd`: 创建的文件描述符，就是给这个文件描述符获取一个连接。
- 后两个参数跟bind一样。

### connect

这个函数是客户端来用的。主要是向服务器发送一个请求来建立连接。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190615121131728.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

这里参数也是一样的，所以就多说了。

## Socket 编程步骤

TCP服务器端：

1. 先调用 `socket` 函数创建一个文件描述符。
2. 然后调用 `bind` 函数绑定一个端口号。
3. 下来调用 `listen` 函数进入监听状态。等待客户端来建立连接。
4. 调用 `accept` 函数来获取一个连接。
5. 使用多线程处理客户端发来的请求，计算响应。
6. 主线程继续等待新的客户端发来连接请求。
7. 新创建的线程计算完响应，发回给客户端。

UDP服务器就是少了 3 4 5 6 步骤，还是计算完响应发回给客户端。

客户端：

1. 调用 `socket` 创建一个文件描述符。
2. 调用 `connect` 函数与服务器建立连接。
3. 给服务器发送请求。
4. 接收服务器返回的响应。



