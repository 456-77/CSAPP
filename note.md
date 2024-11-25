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
