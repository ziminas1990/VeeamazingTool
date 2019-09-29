#pragma once

#include <string>

struct Configuration
{
  Configuration() = default;

  bool read(int argc, char* argv[]);
  bool check();
  void printHelp();

  std::string sInputFile;
  std::string sOutputFile;
  size_t      nBlockSizeKb = 1024;       // 1 Mb is default
  size_t      nPageSizeKb  = 32 * 1024;
  uint16_t    nCoresCount  = 4;
};
