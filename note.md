## perror 作用

在C++中，`perror`函数用于将与全局变量`errno`中存储的错误代码相关联的错误消息打印到标准错误流（stderr）。它的原型定义在`<cstdio>`（或`<stdio.h>`）头文件中，函数声明如下：

```cpp
void perror(const char* str);
```

`perror`函数接受一个字符串参数`str`，该参数通常用于描述与错误消息相关的上下文信息。它会将`str`的内容以及与`errno`相关的错误消息打印到标准错误流。

`perror`函数的作用在于提供详细的错误信息，帮助程序员诊断和调试程序。它将错误消息打印到标准错误流而不是标准输出流，这样可以将错误消息与正常的程序输出区分开来。此外，`perror`函数会自动添加一个换行符，使错误消息更易读。

与普通的`printf`或`cout`相比，`perror`函数具有以下优点：

1. 错误信息的格式是标准化的，易于阅读和解析。
2. 错误消息中包含与`errno`相关的具体错误代码，使得诊断和调试更加方便。
3. 错误消息被打印到标准错误流，与正常的程序输出分离，方便错误定位。

总之，`perror`函数是一个方便的错误处理工具，可以将错误消息打印到标准错误流并提供有用的上下文信息，以帮助开发人员调试和修复程序中的错误。


### 奇怪的问题
```cpp
void ClientGame::handleMessage(const string &rawMessage)
{
    string message = trim(rawMessage);

    // 打印16进制字符做检测
    for (unsigned char c : message)
    {
        cout << hex << static_cast<int>(c) << ' ';
    }
    cout << endl;

    for (unsigned char c : "Your turn")
    {
        cout << hex << static_cast<int>(c) << ' ';
    }
    cout << endl;
    if (message == "Your turn")
    {
        cout << "Your turn.\n";
        playerMove();
    }
    else if (message == "Opponent's turn")
    {
        cout << "Opponent's turn.\n";
    }
    else
    {
        cout << "else branch" << endl;
    }
}
```

这段函数的输出结果是:
59 6f 75 72 20 74 75 72 6e 0 
59 6f 75 72 20 74 75 72 6e 0 
else branch

尽管十六进制一样, 但是还是进入了else branch


# 需要好好管理socket资源

问题描述:
在服务端以及客户端都没有显示的关闭socket时, 客户端 socket 调用 recv 函数返回 0, 显示对端关闭了连接. 

问题排查:
1. 服务端中的客户端 socket 关闭连接的分支并没有被执行, 说明客户端并没有显示的关闭连接
2. 使用 ```pgrep <program_name>``` 查看程序的进程号, 然后使用 ```cat \proc\<process_id>\fd``` 查看程序打开的文件, 发现服务端程序在第二个客户端 socket 接入并发送匹配码成功匹配时, 就会关闭第一个以及第二个客户端的 socket.
3. 排查问题, 既然server中没有显示的关闭socket, 那么就是在其他代码中关闭了, 排查到 SessionManager类时发现其在构造器内创建了 ServerGame 类, 而 ServerGame 类在析构器中关闭了 socket. 由于 ServerGame被创建在栈上, 所以在SessionManager 的 create_session 函数结束时, ServerGame对象会被析构, 从而关闭了socket.