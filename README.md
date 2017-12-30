# InoSim - An Arduino Simulator

## Supported Platforms
- Linux

## Installation
Install the necessary package dependencies

Fedora:
```
sudo dnf install ncurses-devel boost-devel
```

Ubuntu:
```
sudo apt update
sudo apt install libncurses5-dev libboost-all-dev
```

## Usage
Clone this repository. We'll say into ~/InoSim.
To compile and run your ino project (Let's say it is located in ~/src/MyProject/thing.ino) on the simulator, run
```
cd ~/InoSim/Simulator
make INO=~/src/MyProject/thing.ino
./build/Simulator
```

This should start the simulator with an NCurses interface, which will show the output, and allow you to input values to read pins.
