// A simple program that computes the square root of a number
#include <cstring>
#include <execinfo.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <unistd.h>

#include <ncurses.h>

#include "Arduino.h"
#include "InoLibUtils.h"
#include "NcursesInterface.h"
#include "SimulatorConfig.h"
#include "SysState.h"

// Windows
WindowDrawer *stateWin;
WindowDrawer *logWin;
WindowDrawer *helpWin;

bool cursesIsStarted = false;

void cleanup() {
  if (cursesIsStarted) {
    delete stateWin;
    stateWin = NULL;
    delete logWin;
    logWin = NULL;
    delete helpWin;
    helpWin = NULL;
    endwin();
  }
}

void doExit(int code) {
  cleanup();
  exit(code);
}

void sigHandler(int sig) {
  void * bt[10];
  // Get void*'s for all the entries on the stack
  size_t size = backtrace(bt, 10);

  // Resets the stdout and stderr, so we can print to it again.
  cleanup();

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(bt, size, STDERR_FILENO);
  exit(1);
}

void initNcurses();

bool stateIsDirty = true;

void onStateChanged(const std::string & state) {
  stateIsDirty = true;
}

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *win);

WINDOW *_newwin(int height, int width, int starty, int startx) {
  WINDOW *win = newwin(height, width, starty, startx);
  // ncurses getch timeout in ms
  wtimeout(win, 10);
  return win;
}

void initNcurses() {
  onSysStateChanged = onStateChanged;

  // Start curses mode
  initscr();
  cursesIsStarted = true;
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

  for (int i = 0; i < 22; i++) {
    char thebuf[20];
    sprintf(thebuf, "Foo%d", i);
    logBuff.push_back(std::string(thebuf));
  }
  NLOG(__func__);

  // Test code below
  /*
  int ch;
	while((ch = getch()) != 'c')
	{	switch(ch)
		{	case KEY_LEFT:
        startx--;
				// destroy_win(my_win);
				// my_win = create_newwin(height, width, starty,--startx);
				break;
			case KEY_RIGHT:
        startx++;
				// destroy_win(my_win);
				// my_win = create_newwin(height, width, starty,++startx);
				break;
			case KEY_UP:
        starty--;
				// destroy_win(my_win);
				// my_win = create_newwin(height, width, --starty,startx);
				break;
			case KEY_DOWN:
        starty++;
				// destroy_win(my_win);
				// my_win = create_newwin(height, width, ++starty,startx);
				break;
      case 'w':
        starty--;
        // wclear(my_win);
        // wrefresh(my_win);
        // mvwin(my_win, starty, startx);
        // wrefresh(my_win);
        break;
		}
	}
  */
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
  WINDOW *win;

  win = newwin(height, width, starty, startx);
  // 0, 0 gives default characters for the vertical and horizontal
  // lines
  box(win, 0 , 0);
  wrefresh(win);    /* Show that box     */

  return win;
}

void destroy_win(WINDOW *win) {
  /* box(win, ' ', ' '); : This won't produce the desired
   * result of erasing the window. It will leave it's four corners
   * and so an ugly remnant of window.
   */
  wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  /* The parameters taken are
   * 1. win: the window on which to operate
   * 2. ls: character to be used for the left side of the window
   * 3. rs: character to be used for the right side of the window
   * 4. ts: character to be used for the top side of the window
   * 5. bs: character to be used for the bottom side of the window
   * 6. tl: character to be used for the top left corner of the window
   * 7. tr: character to be used for the top right corner of the window
   * 8. bl: character to be used for the bottom left corner of the window
   * 9. br: character to be used for the bottom right corner of the window
   */
  wrefresh(win);
  delwin(win);
}

// We should aim to draw this often
#define DRAW_MIN_MS (1000 / 5)

uint64_t lastDrawTime = 0;

uint64_t lastDrawnLogN = 0;

void tryDraw() {
  if (stateIsDirty) {
    stateWin->draw();
  }
  if (lastDrawnLogN != lastLogN) {
    lastDrawnLogN = lastLogN;
    logWin->draw();
  }
}

void drawIfTime() {
  if (nowMs() - lastDrawTime > DRAW_MIN_MS) {
    lastDrawTime = nowMs();
    tryDraw();
  }
}

void tryHandleKey() {
  int ch = helpWin->getCh();
  switch (ch) {
   case 'q':
    doExit(0);
  }
}

void runIno() {
  drawIfTime();

  setup();

  int i = 0;
  while (true) {
    NLOG(__func__ << ": loop " << ++i);
    loop();
    drawIfTime();
    tryHandleKey();
  }
}

void inoDelayYield() {
  drawIfTime();
  tryHandleKey();
}

int main (int argc, char *argv[]) {
  signal(SIGABRT, sigHandler);

  if (argc >= 2 && !strcmp(argv[1], "-v")) {
     printf("%s Version %d.%d\n", argv[0],
             Simulator_VERSION_MAJOR,
             Simulator_VERSION_MINOR);
     return 0;
  }

  delayYieldIntervalMs = DRAW_MIN_MS;
  delayYieldCallback = inoDelayYield;

  setSysState("foo", 4);
  setSysState("bar", 5);

  initNcurses();

  runIno();
  // currently we won't actually reach here, since runIno shouldn't exit.
  cleanup();
  return 0;
}
