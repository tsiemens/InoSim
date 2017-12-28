// A simple program that computes the square root of a number
#include <chrono>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <ncurses.h>
#include <string>
#include <map>
#include "SimulatorConfig.h"
#include "Arduino.h"
#include "SysState.h"

bool cursesIsStarted = false;

bool stringIsBefore(std::string a, std::string b) {
  return a.compare(b) < 0;
}

void cleanup() {
  if (cursesIsStarted) {
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

int topLine = 0;
int starty = 0;
bool stateIsDirty = true;

void draw() {
  int top = starty;
  if (top < 0) top = 0;
  if (top >= (int)sysStateMap.size()) top = sysStateMap.size() - 1;
  clear();
  refresh();
  int i = 0;
  int lines = 5;
  for (auto it = sysStateMap.begin(); it != sysStateMap.end() && i < lines + top;
       it++, i++) {
    if (i < top) {
      continue;
    }
    printw("%s: %s\n", it->first.c_str(), it->second.c_str());
  }

  mvprintw(LINES - 1, 0, "Press q to exit");
  refresh();
}

void onStateChanged(const std::string & state) {
  stateIsDirty = true;
}

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *win);

void initNcurses() {
  onSysStateChanged = onStateChanged;
	// WINDOW *my_win;
	int startx, width;
	int ch;

	initscr();			/* Start curses mode 		*/
  cursesIsStarted = true;
  noecho();
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need the arrow keys 	*/

	width = 10;
	// starty = (LINES - height) / 2;	[> Calculating for a center placement <]
  starty = 0;
	startx = (COLS - width) / 2;	/* of the window		*/
	printw("Press q to exit");
	refresh();
	// my_win = create_newwin(height, width, starty, startx);
  // ncurses getch timeout in ms
  timeout(10);

  return;
  // Test code below
	while((ch = getch()) != 'q')
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

uint64_t now() {
  using namespace std::chrono;
  return (uint64_t)duration_cast< milliseconds >(
      system_clock::now().time_since_epoch()
    ).count();
}

// We should aim to draw this often
#define DRAW_MIN_MS (1000 / 5)

uint64_t lastDrawTime = 0;

void tryDraw() {
  if (stateIsDirty && now() - lastDrawTime > DRAW_MIN_MS) {
    lastDrawTime = now();
    draw();
  }
}

void tryHandleKey() {
	int ch = getch();
  switch (ch) {
   case 'q':
    doExit(0);
  }
}

void runIno() {
  tryDraw();

  setup();

  while (true) {
    loop();
    tryDraw();
    tryHandleKey();
  }
}

void inoDelayYield() {
  tryDraw();
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
