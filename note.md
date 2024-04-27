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