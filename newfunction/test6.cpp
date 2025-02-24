#include <curses.h>
#include <iostream>
int main() {
    initscr();            // 初始化屏幕
    start_color();        // 启动颜色支持
    init_pair(1, COLOR_RED, COLOR_BLACK); // 定义颜色对，红色字体，黑色背景

    printw("Hello, World!"); // 打印测试文本
    move(0, 6); // 移动光标到 "World" 的 "W" 上
    getch();

    chtype ch = mvinch(0, 7); // 获取光标位置的字符及其属性
    //std::cout<<char(ch&A_CHARTEXT);
    std::cout<<char(ch);
    getch();
    attron(COLOR_PAIR(1));    // 激活颜色对
    mvaddch(0, 7, ch);        // 在相同位置打印相同的字符，这样只改变颜色
    attroff(COLOR_PAIR(1));   // 关闭颜色对
    getch();
    move(0,2);

    refresh(); // 刷新屏幕
    getch();   // 等待用户输入
    endwin();  // 结束 ncurses 模式

    return 0;
}