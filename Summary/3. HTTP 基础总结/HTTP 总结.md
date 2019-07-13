# HTTP 简单总结

HTTP 是我们每个人日常生活中几乎不可能不用的一个协议。我们浏览网页就是在使用 HTTP 协议，那么我就来简单的总结一下 HTTP 协议都有什么组成。

tips: 只是一个简单的总结，没有涉及到深层次的东西。

## HTTP 协议格式

这里我先用一个工具 fiddler 抓包看看。抓个百度。

**请求部分**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190622192048462.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

**响应部分**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190622192129268.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

### 请求格式

#### 首行：使用空格分隔

1. 方法（GET/POST/PUT/DELETE/HEAD）
2. url
3. HTTP 版本号 (HTTP/1.1 当前最常见的版本号)

#### 头部信息(header)

包含了若干行，每行是一个键值对，键和值之间用 : (冒号空格)分隔。

#### 空行

表示header的结束。

#### 请求体(body)

空行后面的内容都是 body。

body 是可选的，可以有也可以没有。

一般来说 GET 请求不带 body；POST/PUT 请求带 body。



### 响应格式

#### 首行

1. HTTP 版本号
2. 状态码
3. 状态码的描述信息

#### 头部信息(header)

有若干行，每行是一个 header (键值对)。

键和值之间用 : (冒号空格)分隔。

#### 空行

表示 header 部分结束。

#### body

空行之后的内容。



## HTTP 方法

### HTTP 的常见方法有哪些？

| 方法    | 说明                   | 支持的 HTTP 协议版本 |
| ------- | ---------------------- | -------------------- |
| GET     | 获取资源               | 1.0 1.1              |
| POST    | 传输实体主体           | 1.0 1.1              |
| PUT     | 传输文件               | 1.0 1.1              |
| HEAD    | 获得报文头部           | 1.0 1.1              |
| DELETE  | 删除文件               | 1.0 1.1              |
| OPTIONS | 询问支持的方法         | 1.1                  |
| TRACE   | 追踪路径               | 1.1                  |
| CONNECT | 要求用隧道协议连接代理 | 1.1                  |
| LINK    | 建立和资源之间的联系   | 1.0                  |
| UNLINE  | 断开连接关系           | 1.0                  |



### GET 和 POST 的区别

> 原本创造者的初衷是 GET 获取资源，POST 去传输资源。
>
> 但是到 2019 年了，这两个方法基本上互相都可以获取资源和传输资源。
>
> GET往往把参数放到 query_string 中；
> POST 往往把参数放到 body 中。

## HTTP 状态码

### 常见的状态码

- 200 OK	访问成功
- 302 Found	重定向    登录页面一般是这种
  - 303 See Other   重定向    
  - 重定向依仗 Location 字段表示重定向到那个 url 网址上去
- 403 Forbidden    用户没有权限访问
- 404 Not Found    表示没有找到指定的资源
- 502 Bad Gateway    服务器挂了
- 504 Gateway Timeout    服务器响应超时
- 隐藏状态码： 418 彩蛋 内容 -> "I am a teapot"

###  分类

- 2xx 请求成功
- 3xx 重定向
- 4xx 客户端出现错误
- 5xx 服务器出现错误

## HTTP header

- Content-Type: 描述响应 body 部分数据格式是什么类型(text/html等)。
- Content-Length: 描述了 body 部分的长度，单位是字节。
- Host: 服务器域名或服务器 ip。
- User-Agent: 描述了浏览器的信息和操作系统的信息。但是手机端看到的页面根据 UA 来吐的一个单独的比较窄的页面，而是完全靠前端的技术来实现的（响应式编程）。
- referer: 当前页面是从哪个页面跳转过来的。可以通过这个来查看用户从哪里搜索到某个网页。就是商家去搜索引擎投广告，然后有人点进来通过 referer 查看别人是从哪里进来的，然后通过这个赚钱。但是这个可以改叫做流量劫持，所以就有了 https 加密，然后就不容易改了。
- Cookie: 是一个保存在浏览器端的字符串。身份标识(session id)。
  - 字符串的含义，是由程序员内部约定的；所以外部的用户一般是看不懂的。
  - Cookie 中最常见的内容是用户的身份信息。
  - 当我们登录账号的时候，服务器就会返回一个身份信息字符串，浏览器保存在 Cookie 中，后续继续访问这个页面的时候，浏览器就会带着这个 Cookie 就不用一直登录了。但是 Cookie 有时间限制，过了一定的时间就会被删除，就需要重新登录。
- Location: 搭配 3xx 状态码使用，告诉客户端接下来要去哪访问。 

### HTTP 不同版本的区别

### HTTP 0.9 ：

1. 只接受GET一种请求方法，没有在通信中指定版本号，且不支持请求头；
2. 此外该版本也不支持POST方法，因此客户端无法向服务器传递太多信息；

### HTTP 1.0：

1. 支持POST、GET、HEAD三种方法；
2. 规定浏览器与服务器之间只保持短暂的连接，浏览器的每次请求都需要与服务器建立一个TCP连接，服务器完成请求处理后立即断开TCP连接，服务器不跟踪每个客户也不记录过去的请求；

### HTTP 1.1：

1. 新增了五种请求方法：PUT、DELETE、CONNECT、TRACE、OPTIONS；

2. HTTP 1.1 在Request消息头里头多了一个Host域，便于一台WEB服务器上可以在同一个IP地址和端口号上使用不同的主机名来创建多个虚拟WEB站点；

3. 在HTTP/1.1中新增了24个状态响应码：
   100-199 用于指定客户端应相应的某些动作。

   200-299 用于表示请求成功。

   300-399 用于已经移动的文件并且常被包含在定位头信息中指定新的地址信息。

   400-499 用于指出客户端的错误。

   500-599 用于支持服务器错误。

叮~:bell: