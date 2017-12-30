#ifndef NCURSES_INTERFACE_H
#define NCURSES_INTERFACE_H

#include <ncurses.h>

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
};

class HelpWin : public WindowDrawer {
 public:
  HelpWin(int startY);
  virtual void draw();
};

#endif // NCURSES_INTERFACE_H
