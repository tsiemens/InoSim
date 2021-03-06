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
  manualTopEntry = -1;
}

void LogWin::draw() {
  WinDimens d = getDimens();

  wclear(win_);
  // Draw a separator line along the top of the window
  whline(win_, 0, COLS);
  wmove(win_, 1, 0);
  for (auto it = logBuff.begin() + firstEntryToShow(); it != logBuff.end(); it++) {
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

void LogWin::scrol(int nlines) {
  int top = firstEntryToShow();
  top += nlines;

  if (logBuff.size() == 0 || top < 0) {
    top = 0;
  } else if (top >= (int)logBuff.size()) {
    top = logBuff.size() - 1;
  }

  manualTopEntry = top;

}

int LogWin::firstEntryToShow() {
  if (manualTopEntry >= 0) {
    return manualTopEntry;
  } else {
    return std::max((int)logBuff.size() - 3, 0);
  }
}

HelpWin::HelpWin(int startY) {
  win_ = newWindow(1, COLS, startY, 0);
}

void HelpWin::draw() {
  wmove(win_, 0, 0);
  wprintw(win_, "q:quit  w/s:scroll logs");
  wrefresh(win_);
}

// Controller
NcursesCtrlr::NcursesCtrlr() {
  sysStateIsDirty = true;
}

void NcursesCtrlr::start() {
  assert(!isStarted());

  initscr();
  noecho();
  // Disable line buffering. Pass all keypresses to getch
  cbreak();
  keypad(stdscr, TRUE);    /* I need the arrow keys   */

  // ncurses getch timeout in ms
  timeout(10);

  /* State
   * ---------------
   * Logs
   * Help
   */

  int starty = 0;
  int logsH = LINES / 3;
  int helpH = 1;
  int stateH = LINES - logsH - helpH;

  stateWin = new StateWin(stateH, starty);

  starty += stateH;
  logWin = new LogWin(logsH, starty);

  starty += logsH;
  helpWin = new HelpWin(starty);
  helpWin->draw();
}

void NcursesCtrlr::stop() {
  if (isStarted()) {
    delete stateWin;
    stateWin = NULL;
    delete logWin;
    logWin = NULL;
    delete helpWin;
    helpWin = NULL;
    endwin();
  }
}

bool NcursesCtrlr::isStarted() {
  return helpWin != NULL;
}

void NcursesCtrlr::maybeHandleKey() {
  int ch = helpWin->getCh();
  switch (ch) {
   case 'q':
    if (onExitHandler) {
      onExitHandler();
    }
   case KEY_LEFT:
   case KEY_RIGHT:
   case KEY_UP:
   case KEY_DOWN:
    // TODO
    break;
   case 'w':
    logWin->scrol(-1);
    lastDrawnLogN = 0;
    break;
   case 's':
    logWin->scrol(1);
    lastDrawnLogN = 0;
    break;
  }
}

void NcursesCtrlr::drawStaleWins() {
  if (sysStateIsDirty) {
    stateWin->draw();
  }
  if (lastDrawnLogN != lastLogN) {
    lastDrawnLogN = lastLogN;
    logWin->draw();
  }
}

void NcursesCtrlr::drawIfIsTime() {
  if (nowMs() - lastDrawTime > DRAW_MIN_MS) {
    lastDrawTime = nowMs();
    drawStaleWins();
  }
}

void NcursesCtrlr::doPeriodicActions() {
  drawIfIsTime();
  maybeHandleKey();
}
