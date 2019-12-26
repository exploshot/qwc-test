

#pragma once

#include <string>

#include <WalletInfo.h>

char *getCmdOption(char **begin, char **end, const std::string &option);

bool cmdOptionExists(char **begin, char **end, const std::string &option);

Config parseArguments(int argc, char **argv);

void helpMessage();

void versionMessage();