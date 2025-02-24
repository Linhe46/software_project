#include <curses.h>

void checkAndScrollIfNeeded(WINDOW* win) {
    int x, y, maxX, maxY;
    getyx(win, y, x); // 获取当前光标位置
    getmaxyx(win, maxY, maxX); // 获取窗口尺寸

    if (y >= maxY - 1) { // 如果接近或到达底部
        scroll(win); // 滚动窗口内容
    }
}

int main() {
    initscr(); // 初始化ncurses模式
    WINDOW* win = stdscr;
    scrollok(win, TRUE); // 启用滚动

    // 示例：添加多行文本，确保不会因到达底部而卡死
    for (int i = 0; i < 100; ++i) {
        checkAndScrollIfNeeded(win);
        printw("Line %d\n", i);
    }

    refresh(); // 刷新屏幕以显示内容
    getch(); // 等待用户输入
    endwin(); // 退出ncurses模式

    return 0;
}