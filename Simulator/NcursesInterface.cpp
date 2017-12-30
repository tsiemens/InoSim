#include "NcursesInterface.h"

#include "InoLibUtils.h"
#include "SysState.h"

WINDOW *newWindow(int height, int width, int starty, int startx) {
  WINDOW *win = newwin(height, width, starty, startx);
  // ncurses getch timeout in ms
  wtimeout(win, 10);
  return win;
}

WindowDrawer::~WindowDrawer() {
  delwin(win_);
}

WinDimens WindowDrawer::getDimens() {
  WinDimens d;
  getmaxyx(win_, d.height, d.width);
  getbegyx(win_, d.start.y, d.start.x);
  return d;
}

Coords WindowDrawer::getCursor() {
  Coords c;
  getyx(win_, c.y, c.x);
  return c;
}

int WindowDrawer::getCh() {
  return wgetch(win_);
}

StateWin::StateWin(int height, int startY) {
  win_ = newWindow(height, COLS, startY, 0);
}

void StateWin::draw() {
  WinDimens d = getDimens();

  wclear(win_);
  wmove(win_, 0, 0);
  int i = 0;
  for (auto it = sysStateMap.begin(); it != sysStateMap.end() && i < d.height;
       it++, i++) {
    wprintw(win_, "%s: %s\n", it->first.c_str(), it->second.c_str());
  }

  wrefresh(win_);
}

LogWin::LogWin(int height, int startY) {
  win_ = newWindow(height, COLS, startY, 0);
}

void LogWin::draw() {
  WinDimens d = getDimens();
  int firstEntry = std::max((int)logBuff.size() - 3, 0);

  wclear(win_);
  // Draw a separator line along the top of the window
  whline(win_, 0, COLS);
  wmove(win_, 1, 0);
  for (auto it = logBuff.begin() + firstEntry; it != logBuff.end(); it++) {
    wprintw(win_, (*it).c_str());
    wprintw(win_, "\n");
    Coords c = getCursor();
    // y caps out at maxY - 1, even though semantically, this doesn't sound right.
    if (c.y + 1 >= d.height && c.x != 0) {
      // nothing more can be printed
      break;
    }
  }
  wrefresh(win_);
}

HelpWin::HelpWin(int startY) {
  win_ = newWindow(1, COLS, startY, 0);
}

void HelpWin::draw() {
  wmove(win_, 0, 0);
  wprintw(win_, "q:quit  w/s:scroll logs");
  wrefresh(win_);
}
