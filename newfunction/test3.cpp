#include <curses.h>

int main() {
    initscr(); // 初始化ncurses模式
    noecho(); // 关闭输入回显
    cbreak(); // 禁用行缓冲

    // 使用printw在当前光标位置打印
    printw("Hello, ncurses!");

    // 移动光标并使用mvprintw在指定位置打印
    mvprintw(2, 0, "This is printed at row 3, column 1.");

    refresh(); // 刷新屏幕以显示内容
    getch(); // 等待用户输入

    endwin(); // 结束ncurses模式

    return 0;
}