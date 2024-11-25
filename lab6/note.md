## getopt函数
`getopt` 是 C 标准库中用于解析命令行选项的函数，定义在 `<unistd.h>` 头文件中。它帮助程序轻松处理命令行输入中的选项和参数。

### 函数声明
```c
int getopt(int argc, char * const argv[], const char *optstring);
```

### 参数说明
1. **`argc`**: 命令行参数的个数（来自 `main(int argc, char *argv[])`）。
2. **`argv`**: 命令行参数的数组。
3. **`optstring`**: 描述可接受的选项字符串。
   - 每个选项是一个字符，例如 `"a"` 表示 `-a` 是一个合法选项。
   - 如果某选项需要参数，在选项后加 `:`，例如 `"b:"` 表示 `-b` 需要一个参数。
   - 如果某选项的参数是可选的，使用 `::`，例如 `"c::"` 表示 `-c` 可带参数，但参数是可选的（非 POSIX 标准）。
   - 如果命令行中没有指定某些选项,getopt不会报错,只是不会对其进行处理。

### 返回值
- 成功解析选项时，返回该选项的字符。
- 如果解析到非选项参数或遇到错误，返回 `?` 并设置全局变量 `optopt`。
- 如果解析结束，返回 `-1`。

### 全局变量
- **`optarg`**: 指向当前选项的参数值（如果选项需要参数）。
- **`optind`**: 指向下一个要处理的命令行参数的索引。
- **`opterr`**: 控制错误信息输出（默认是 1，设置为 0 可以关闭错误信息）。

### 用法示例
#### 示例代码
```c
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    // 定义支持的选项
    const char *optstring = "ab:c::";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'a':
            printf("Option: -a\n");
            break;
        case 'b':
            printf("Option: -b with argument '%s'\n", optarg);
            break;
        case 'c':
            printf("Option: -c with optional argument '%s'\n", optarg ? optarg : "none");
            break;
        case '?': // 未知选项
            printf("Unknown option: -%c\n", optopt);
            break;
        default:
            printf("Unhandled option\n");
        }
    }

    // 处理非选项参数
    for (int i = optind; i < argc; i++) {
        printf("Non-option argument: %s\n", argv[i]);
    }

    return 0;
}
```

#### 运行结果
假设编译生成 `program`：

1. 输入 `./program -a -b value -c other`
   ```plaintext
   Option: -a
   Option: -b with argument 'value'
   Option: -c with optional argument 'none'
   Non-option argument: other
   ```

2. 输入 `./program -d`
   ```plaintext
   Unknown option: -d
   ```

### 注意事项
1. **参数顺序问题**：
   - 选项必须在非选项参数之前。例如，`./program -a value` 正常，但 `./program value -a` 会把 `value` 当作非选项参数。
   - 如果需要支持混合顺序，使用 `POSIXLY_CORRECT` 环境变量禁用默认排序。

2. **线程安全性**：
   - `getopt` 不是线程安全的。如果在多线程程序中解析选项，可以使用 `getopt_long` 或 `getopt_long_only`。

### 扩展功能
`getopt` 的基础功能有限，现代程序推荐使用 GNU 扩展的 `getopt_long` 函数来支持长选项（如 `--help`）：
```c
#include <getopt.h>
int getopt_long(int argc, char * const argv[], const char *optstring,
                const struct option *longopts, int *longindex);              
```


## atoi 函数
`atoi` 是 C 标准库中一个将字符串转换为整数的函数，定义在 `<stdlib.h>` 头文件中。其全称是 **ASCII to Integer**，常用于将命令行参数或其他输入字符串转换为整数。

---

### 函数原型
```c
int atoi(const char *str);
```

---

### 功能
将以字符串形式表示的数字（如 `"1234"`）转换为对应的整数值（如 `1234`）。

---

### 参数
- **`str`**: 指向一个以 `null` 结尾的字符串，该字符串应包含可解析的整数表示。

---

### 返回值
- 返回字符串 `str` 转换后的整数值。
- 如果字符串中不包含有效的数字，返回值未定义（通常返回 `0`）。

---

### 使用示例

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char *numStr = "12345";
    const char *invalidStr = "abc123";

    // 转换字符串为整数
    int number = atoi(numStr);
    printf("Converted number: %d\n", number);

    // 转换无效字符串
    int invalidNumber = atoi(invalidStr);
    printf("Converted invalid string: %d\n", invalidNumber);

    return 0;
}
```

#### 输出
```plaintext
Converted number: 12345
Converted invalid string: 0
```

---

### 注意事项
1. **字符串格式**：
   - `atoi` 会忽略字符串前的空白字符（如空格、`\t` 等）。
   - 如果字符串以非数字字符开头，`atoi` 将返回 `0`。
   - 一旦遇到无法解析的字符，`atoi` 将停止解析。

   示例：
   ```c
   const char *str = "   42abc";
   int num = atoi(str);  // 输出 42
   ```

2. **未定义行为**：
   - 输入的字符串如果超出了 `int` 类型的范围，可能会导致未定义行为（无溢出检查）。
   - 示例：
     ```c
     const char *str = "999999999999999999";
     int num = atoi(str);  // 可能导致未定义行为
     ```

3. **更安全的替代函数**：
   使用 `strtol` 或 `strtoll` 替代 `atoi`，它们提供更强的错误处理和范围检查。

---

### 常见问题
#### 1. **`atoi` 是否会返回错误代码？**
   - 不会，`atoi` 无法检测转换失败的情况。如果需要错误检测，请使用 `strtol`。

#### 2. **处理负数字符串**
   - `atoi` 支持处理负数字符串，如 `"-123"` 会返回 `-123`。

#### 3. **如果字符串为空怎么办？**
   - 如果传递空字符串或空指针，会导致未定义行为。例如：
     ```c
     const char *emptyStr = "";
     int num = atoi(emptyStr);  // 未定义行为
     ```

---

### `atoi` 的替代函数 `strtol`
`strtol` 提供更灵活和安全的字符串到整数的转换。示例：
```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char *str = "123abc";
    char *endptr;

    long number = strtol(str, &endptr, 10);
    printf("Converted number: %ld\n", number);
    printf("Unprocessed part: %s\n", endptr);

    return 0;
}
```

#### 输出
```plaintext
Converted number: 123
Unprocessed part: abc
```

### 总结
`atoi` 简单快捷，但缺乏错误处理能力，建议在关键场景下使用更安全的 `strtol` 或 `strtoll`。


## .trace文件
实验给的.trace是这样子的
每行表示一个或两个内存访问。每一行的格式为
[space] operation address,size
operation字段表示内存访问的类型：“I” 表示指令加载，“L” 表示数据加载，“S” 表示数据存储，“M” 表示数据修改(即，数据加载后跟着一个数据存储)。在每个 “I” 之前从来没有空格。每个 “M”、“L” 和 “S” 前面总是有一个空格。address字段指定一个64位的十六进制内存地址。size字段指定操作访问的字节数。
``` c
 L 0,1
 L 1,1
 L 2,1
 L 3,1
 S 4,1
 L 5,1
 S 6,1
 L 7,1
 S 8,1
 L 9,1
 S a,1
 L b,1
 S c,1
 L d,1
 S e,1
 M f,1 
``` 
有"I"就是这样的  
``` c
 S 00600aa0,1
I  004005b6,5
I  004005bb,5
I  004005c0,5
```
