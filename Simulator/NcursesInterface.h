#ifndef NCURSES_INTERFACE_H
#define NCURSES_INTERFACE_H

#include <ncurses.h>

// We should aim to draw this often
#define DRAW_MIN_MS (1000 / 5)

struct Coords {
  int x;
  int y;
};

struct WinDimens {
  int width;
  int height;
  Coords start;
};

class WindowDrawer {
 public:
  virtual ~WindowDrawer();

  WinDimens getDimens();
  Coords getCursor();
  int getCh();
  virtual void draw() = 0;

 protected:
  WINDOW *win_;
};

class StateWin : public WindowDrawer {
 public:
  StateWin(int height, int startY);
  virtual void draw();
};

class LogWin : public WindowDrawer {
 public:
  LogWin(int height, int startY);
  virtual void draw();
  // Can't use name "scroll" since it is a ncurses macro
  void scrol(int nlines);

 private:
  int firstEntryToShow();

  int manualTopEntry;
};

class HelpWin : public WindowDrawer {
 public:
  HelpWin(int startY);
  virtual void draw();
};

class NcursesCtrlr {
 public:
  NcursesCtrlr();
  void start();
  void stop();
  bool isStarted();

  void doPeriodicActions();
  void maybeHandleKey();
  void drawIfIsTime();

  void (*onExitHandler)() = NULL;
  bool sysStateIsDirty;

 private:
  void drawStaleWins();

  uint64_t lastDrawTime = 0;
  uint64_t lastDrawnLogN = 0;

  StateWin *stateWin;
  LogWin *logWin;
  HelpWin *helpWin;
};

#endif // NCURSES_INTERFACE_H
