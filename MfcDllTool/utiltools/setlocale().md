## setlocale函数

函数原型：char* setlocale (int category, const char* locale);

setlocale位于头文件![](https://www.linuxidc.com/upload/2019_08/190802204767222.jpg)，setlocale() 函数既可以用来对当前程序进行**地域设置**（本地设置、区域设置），也可以用来获取当前程序的**地域设置信息**，使用setlocale需要两个参数

 

**第一个参数category：**

用来设置地域设置的影响范围。地域设置包含日期格式、数字格式、货币格式、字符处理、字符比较等多个方面的内容，当前的地域设置可以只影响某一方面的内容，也可以影响所有的内容，

category 的值不能随便设置，必须使用 <locale.h> 中定义的宏

![](https://www.linuxidc.com/upload/2019_08/190802204767223.jpg)

 

![](https://www.linuxidc.com/upload/2019_08/190802204767224.jpg)

 

**第二个参数locale：**

用来设置地域设置的名称（字符串），也就是设置为哪种地域，对于不同的平台和不同的编译器，地域设置的名称可能会不同，[C语言](https://www.linuxidc.com/topicnews.aspx?tid=18)标准没有干预太多。C语言标准只是规定，各个组织在实现编译器时至少要支持以下**三个名称**：

![](https://www.linuxidc.com/upload/2019_08/190802204767225.jpg)

 

**setlocale函数的返回值：**

如果 setlocale() 执行**成功**，那么返回一个指向字符串的指针，该字符串包含了当前地域设置的名称。也就是说，setlocale() 会将当前地域设置的名称返回

如果 setlocale() 执行**失败**（例如为 locale 指定的名称不存在，就会导致地域设置失败），那么返回空指针NULL

 

如果程序中没有用setlocale函数设置地域等其他参数，那么程序运行时locale 被初始化为默认的 C locale，其采用的字符编码是所有本地 ANSI 字符集编码的公共部分，是用来书写C语言源程序的最小字符集，

下面用setlocale来得到当前使用的地域设置

```
char *p;

p = setlocale(LC_ALL, NULL);
printf("%s\n", p);
```

运行结果：

![](https://www.linuxidc.com/upload/2019_08/190802204767226.jpg)

从上面可以看出当前编译器使用的地域设置是C，如果想用宽字符来输出中文，却仍然使用默认的地域设置（C）就会出现问题，下面尝试用setlocale来解决宽字符输出中文的问题

```
wchar_t ss[20];
char *p;

p = setlocale(LC_ALL, NULL);
ss[0] = _T('中');
ss[1] = _T('文');
ss[2] = _T('\0');
wprintf(L"%ls\n", ss);

printf("地域设置：%s\n", p);
```

运行结果：

![](https://www.linuxidc.com/upload/2019_08/190802204767227.jpg)

从上面的运行结果可以看出'中' '文'两个字并没有被正确的输出，因为locale被初始化为默认的C，其采用的字符编码是所有本地ANSI字符集编码的公共部分，是用来书写C语言源程序的**最小字符集**，下面用setlocale来把地域设置为简体中文，修改后的代码如下：

```
wchar_t ss[20];
char *p;

p = setlocale(LC_ALL, "");
ss[0] = _T('中');
ss[1] = _T('文');
ss[2] = _T('\0');
wprintf(L"%ls\n", ss);

printf("地域设置：%s\n", p);
```

运行结果：

![](https://www.linuxidc.com/upload/2019_08/190802204767228.jpg)

 

因为操作系统是中文环境，所以上面的代码中![](https://www.linuxidc.com/upload/2019_08/190802204767229.jpg)可以改为![](https://www.linuxidc.com/upload/2019_08/1908022047672210.jpg)效果是一样的

