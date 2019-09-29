#include "Configuration.h"

#include <iostream>
#include <getopt.h>

enum
{
  eUnknownCode      = 0,
  eHelpCode         = 'h',
  eInputFileCode    = 'i',
  eOutputFileCode   = 'o',
  eBlockSizeCode    = 'b',
  ePageSizeCode     = 'p',
  eCoresCount       = 'c',
};

bool Configuration::read(int argc, char* argv[])
{

  static option long_options[] = {
    { "help",         0,  nullptr, eHelpCode       },
    { "input",        1,  nullptr, eInputFileCode  },
    { "output",       1,  nullptr, eOutputFileCode },
    { "block-size",   1,  nullptr, eBlockSizeCode  },
    { "page-size",    1,  nullptr, ePageSizeCode   },
    { "cores-count",  1,  nullptr, eCoresCount     },
    { nullptr,        0,  nullptr, eUnknownCode    }
  };

  while(true) {
    int index   = 0;
    int option  = getopt_long(argc, argv, "hi:o:b:p:c:", long_options, &index);
    if(option == -1)
      break;

    switch(option) {
      case eHelpCode:
        printHelp();
        exit(0);
      case eInputFileCode:
        sInputFile = optarg;
        break;
      case eOutputFileCode:
        sOutputFile = optarg;
        break;
      case eBlockSizeCode: {
        try {
          nBlockSizeKb = std::stoul(optarg);
        } catch(...) {
          std::cerr << "block-size has invalid value: " << optarg << "!" << std::endl;
          return false;
        }
        break;
      }
      case ePageSizeCode: {
        try {
          nPageSizeKb = std::stoul(optarg);
        } catch(...) {
          std::cerr << "page-size has invalid value: " << optarg << "!" << std::endl;
          return false;
        }
        break;
      }
      case eCoresCount: {
        try {
          nCoresCount = static_cast<uint16_t>(std::stoul(optarg));
        } catch(...) {
          std::cerr << "cores-count has invalid value: " << optarg << "!" << std::endl;
          printHelp();
          return false;
        }
        break;
      }
      case '?':
        return false;
      default:
        std::cout << "Unknown option " << argv[optind] << "!" << std::endl;
        return false;
    }
  }
  return !sInputFile.empty() && !sOutputFile.empty() && nBlockSizeKb > 0;
}

void Configuration::printHelp()
{
  std::cout << "Usage:\n"
            << "  VeeamazingTool -i <input file> -o <output file> -b 64\n\n"
            << "Mandatory arguments:\n"
            << "  -i, --input=PATH      : path to input file\n"
            << "  -o, --output=PATH     : path to output (signature) file\n\n"
            << "Optional arguments:\n"
            << "  -b, --block-size=NUM  : length of each signed block in KB; default: 1024\n"
            << "  -p, --page-size=NUL   : length of page (in KB); default: 128 * 1024\n"
            << "  -c, --cores-count=NUL : total number of workers; default: 4\n"
            << "  -h, --help            : print this message and exit\n"
            << std::endl;
}
