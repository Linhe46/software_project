#include <curses.h>

int main() {
    initscr(); // 初始化curses模式
    cbreak(); // 启用cbreak模式
    noecho(); // 关闭回显
    keypad(stdscr, TRUE); // 启用键盘增强模式，允许读取小键盘等特殊键位

    int ch;
    printw("Enter any character: ");
    ch = getch(); // 立即读取一个字符
    printw("\nYou entered: %c", ch);
    getch();
    endwin(); // 退出curses模式
    return 0;
}