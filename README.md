sokoban
=====================

linux 下基于 curses 的 C 语言推箱子自动求解程序
----------------------------------------------

### 运行环境
* Liunx，需安装 curses 或 ncurses 库

### 核心算法
* IDA-star 算法
* 二分图最小权值匹配
* zobrist 哈希

### 应用说明
* 该程序可以求解按照 .txt 例子的格式输入的地图(10x10 的 6 个箱子以内的地图大部分可以在几秒内解决)

### 功能演示
![image](https://github.com/QYPan/dynamic_gif/blob/master/sokoban/sokoban.gif)

### 注意
* 该程序尚未完善，可能无法良好运行，代码风格糟糕，难以重构

### 相关博客链接
* http://blog.csdn.net/u013351484/article/details/51598270
