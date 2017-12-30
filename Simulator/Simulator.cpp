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

NcursesCtrlr *cursesCtrlr;

bool cursesIsStarted = false;

void cleanup() {
  if (cursesCtrlr) {
    cursesCtrlr->stop();
    delete cursesCtrlr;
    cursesCtrlr = NULL;
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

void onStateChanged(const std::string & state) {
  if (cursesCtrlr) {
    cursesCtrlr->sysStateIsDirty = true;
  }
}

void handleCursesExit() {
  doExit(0);
}

void inoDelayYield() {
  if (cursesCtrlr) {
    cursesCtrlr->doPeriodicActions();
  }
}

void initNcurses() {
  cursesCtrlr = new NcursesCtrlr();
  cursesCtrlr->onExitHandler = handleCursesExit;
  cursesCtrlr->start();

  for (int i = 0; i < 22; i++) {
    char thebuf[20];
    sprintf(thebuf, "Foo%d", i);
    logBuff.push_back(std::string(thebuf));
  }
}

void runIno() {
  cursesCtrlr->doPeriodicActions();

  setup();

  int i = 0;
  while (true) {
    NLOG(__func__ << ": loop " << ++i);
    loop();
    cursesCtrlr->doPeriodicActions();
  }
}

int main (int argc, char *argv[]) {
  signal(SIGABRT, sigHandler);

  if (argc >= 2 && !strcmp(argv[1], "-v")) {
     printf("%s Version %d.%d\n", argv[0],
             Simulator_VERSION_MAJOR,
             Simulator_VERSION_MINOR);
     return 0;
  }

  // Globals in InoLibUtils
  delayYieldIntervalMs = DRAW_MIN_MS;
  delayYieldCallback = inoDelayYield;

  onSysStateChanged = onStateChanged;

  setSysState("foo", 4);
  setSysState("bar", 5);

  initNcurses();

  runIno();
  // currently we won't actually reach here, since runIno shouldn't exit.
  cleanup();
  return 0;
}
