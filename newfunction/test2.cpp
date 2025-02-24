#include <curses.h>

int main() {
    // 初始化ncurses
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();

    // 定义颜色对
    init_pair(1, COLOR_RED, COLOR_BLACK); // 红色文字，黑色背景
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // 绿色文字，黑色背景

    int ch;
    while ((ch = getch()) != KEY_F(1)) { // 按F1键退出
        switch (ch) {
            case 'a': // 如果输入是'a'，使用颜色对1
                attron(COLOR_PAIR(1));
                printw("a");
                attroff(COLOR_PAIR(1));
                break;
            case 'b': // 如果输入是'b'，使用颜色对2
                attron(COLOR_PAIR(2));
                printw("b");
                attroff(COLOR_PAIR(2));
                break;
            case '\b':
                
            default:
                printw("%c", ch); // 其他字符不变色
                break;
        }
        refresh();
    }

    // 结束ncurses会话
    endwin();

    return 0;
}