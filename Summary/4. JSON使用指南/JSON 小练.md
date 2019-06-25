# JSON 小练

在日常的开发环境中，尤其是进行服务器端和客户端开发中，我们一定会对数据进行序列化和反序列化，在之前，我总是用空格作为两个数据之间的分隔，直到我遇到 JSON， 我爱了。

那么我就用一个简单的例子来总结一下目前我用到的 JSON 操作。

## 网络计算器

> 我用的是 `jsoncpp` :kissing_smiling_eyes:

### 客户端：

### 利用 JSON 进行序列化 

在计算器的客户端，我需要用户进行输入操作，规定，先输入两个数字，然后再输入符号。

```std::cin >> num1 >> num2 >> op```

那么现在对这三个数据进行序列化

```cpp
// 序列化
Json::Value req_value;

// 下面这两种 writer 会生成两种不同的字符串
// 下面贴图
Json::StyledWriter writer; // Json::FastWriter writer

// 用 Value 对象进行序列化， 生成的是 Key: Value 形式
req_value["num1"] = num1;
req_value["num2"] = num2;
req_value["op"] = op;

// 将 Json 格式的序列化转成字符串形式返回
std::string req = writer.write(req_value);
```

### 利用 JSON 进行反序列化

当客户端接收到服务器返回的数据时，也要进行反序列化，因为服务器也会返回一个序列化好的 JSON 字符串。

```cpp
// 接收
std::string resp;
client.Recv(&resp);

// 反序列化
Json::Reader reader;
Json::Value resp_value;

// 将 resp JSON 字符串变成 JSON 格式的数据放到 resp_value 里面
reader.parse(resp, resp_value);
// 将 Key 为 result 的 value 当做整型给 result
int result = resp_value["result"].asInt();

std::cout << "Result: " << result << std::endl;
```

### 服务器：

### 利用 JSON 对接收到的数据进行反序列化

```cpp
Json::Reader reader;
Json::Value req_value;

// 反序列化
// 先把收到的字符串变成 JSON 格式
reader.parse(req, req_value);

// 然后把键值对中的值拿出来
int num1 = req_value["num1"].asInt();
int num2 = req_value["num2"].asInt();
std::string op = req_value["op"].asString();

```

### 利用 JSON 对处理完的数据进行序列化

```cpp
// 序列化
Json::FastWriter writer;
Json::Value resp_value;

resp_value["result"] = ret;
*resp = writer.write(resp_value);
```

## 对比 Json::FastWriter 和 Json::StyledWriter 的区别

先看看如果是 Json::StyledWriter 将 Json 变成字符串后的效果

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190623142357123.png)

可以看到他是有格式的字符串。

而 FastWriter 穿过来的是在一行上的字符串。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190623142402300.png)

## 怎么把 JSON 里的 Key 变成数组

```cpp
// 序列化
Json::Value req_value;
Json::FastWriter writer;

// 这里用 append 接口就是把 Key 为 nums 的元素作为数组在后面插入元素
req_value["nums"].append(num1);
req_value["nums"].append(num2);
req_value["op"] = op;
```



在解析的时候循环拿数据就行

```cpp
std::vector<int> nums;
for (int i = 0; i < req_value["nums"].size(); i++)
{
    nums.push_back(req_value["nums"][i].asInt());
}
std::string op = req_value["op"].asString()
```



****

**To be continue...**

叮~:bell: