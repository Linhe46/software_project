#include <curses.h>
#include <thread>
#include <chrono>
#include <vector>
#include <string>


const int LEFT_MAX=3, UP_MAX=2;

void initialize() {
    initscr();          // 初始化屏幕
    start_color();      // 初始化颜色系统
    cbreak();           // 按键不需要输入回车直接交云
    noecho();           // 按键不显示
    curs_set(1);
    // 定义颜色对
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // 定义颜色对1为红色字体，黑色背景
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK); // 定义颜色对2为黄色字体，蓝色背景，用于高亮显示
}
void curs_shine(){
    while(1){
    curs_set(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    curs_set(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}
void text_move(int y,int x){
    move(y+UP_MAX, x+LEFT_MAX);
}
void text_mvaddch(int y, int x, char ch){
    mvaddch(y+UP_MAX, x+LEFT_MAX, ch); // 显示字符
}
void text_mvaddstr(int& y,int& x,const std::string& s){
    for(char ch:s)
        mvaddch(y+UP_MAX, x+LEFT_MAX, ch);
    x++;
}
void addArrow(int y){
    mvaddch(y+UP_MAX,0,'>');
    mvaddch(y+UP_MAX,1,'>');
    mvaddch(y+UP_MAX,2,'>');
}


std::vector<int> line_lengths(1,0);
void shutdown() {
    endwin();           // 清理ncurses
}
int main() {
    initialize();

    int ch;
    int x = 0, y = 0; // 初始位置

    // 使用颜色对1
    attron(COLOR_PAIR(1));
    mvprintw(y, x, "Mini-Lisp: ");
    attroff(COLOR_PAIR(1));

    addArrow(y);
    text_move(y, x); // 移动到下一行开始输入
    line_lengths.push_back(0);

    std::thread t1(curs_shine);//光标闪烁

    while((ch = getch()) != char(27)) {
        switch(ch){
            case '\b':
                if(x > 0) { // 确保不会删除提示文本
                    text_move(y, --x);
                    delch();
                    line_lengths[y]--;
                }
                else if(x==0 && y > 0){
                    y--;
                    x=line_lengths[y];
                    text_move(y,x);
                    delch();
                }
                break;
            case '\n':
                x=0;
                y++;
                if(y+1>line_lengths.size())
                    line_lengths.push_back(0);
                addArrow(y);
                text_move(y,x);
                break;          
            default:
        // 读取字符直到'q'
                attron(COLOR_PAIR(2)); // 启用高亮颜色对
                text_mvaddch(y, x++, ch); // 显示字符
                attroff(COLOR_PAIR(2)); // 禁用高亮颜色对
                line_lengths[y]++;
                break;
        }
    }
    text_mvaddstr(++y,++x,"output this");

    t1.detach(); // 分离线程，让它在后台运行


    shutdown();
    return 0;
}