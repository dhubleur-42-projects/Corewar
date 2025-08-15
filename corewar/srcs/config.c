#include "config.h"

// Maximum number of champions that can be loaded
const int CHAMP_MAX_COUNT = 4;

// Number of registers each process can use (in addition to the PC)
const int REG_NUMBER = 16;

// Number of octets of each register
const int REG_SIZE = 4;

// Each process has to execute at least a live instruction every CYCLE_TO_DIE cycles
const int CYCLE_TO_DIE = 42;

// Decrease CYCLE_TO_DIE if it was not decreased for this checks
const int MAX_CHECKS = 10;

// Numer of live instructions in a cycle to trigger a decrease of CYCLE_TO_DIE
const int NBR_LIVE = 10;

// Reduce CYCLE_TO_DIE by CYCLE_DELTA when decreasing
const int CYCLE_DELTA = 5;

// Size of the memory in octets
const int MEM_SIZE = 4096;

// Maximum size of each champion
const int CHAMP_MAX_SIZE = MEM_SIZE / 6;