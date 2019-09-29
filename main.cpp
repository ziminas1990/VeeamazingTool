#include <thread>

#include "SimpleBlocksStream.h"
#include "FileBlocksStream.h"
#include "FileDevice.h"
#include "Worker.h"
#include "SignatureFile.h"
#include "Configuration.h"

inline size_t getTotalBlocksInFile(IBlockDevicePtr pDevice, size_t nBlockSize)
{
  size_t nTotalLength = pDevice->getSize();
  return nTotalLength / nBlockSize + (nTotalLength % nBlockSize ? 1 : 0);
}

int main(int argc, char* argv[])
{
  Configuration cfg;
  if (!cfg.read(argc, argv)) {
    cfg.printHelp();
    return 1;
  }

  // Initializing components:
  FileDevicePtr pInput  = std::make_shared<FileDevice>();
  FileDevicePtr pOutput = std::make_shared<FileDevice>();
  try {
    if (!pInput->open(cfg.sInputFile, FileDevice::eReadOnly)) {
      std::cerr << "Failed to open input file " << cfg.sInputFile << std::endl;
      return 1;
    }
    if (!pOutput->open(cfg.sOutputFile, FileDevice::eWriteOnly)) {
      std::cerr << "Failed to open signature file " << cfg.sOutputFile << std::endl;
      return 1;
    }
  } catch(...) {
    std::cerr << "Failed to open input or output file!" << std::endl;
    return 1;
  }

  size_t nBlockSize = cfg.nBlockSizeKb * 1024;
  size_t nPageSize  = cfg.nPageSizeKb  * 1024;

  FileBlocksStreamPtr pSourceStream =
      std::make_shared<FileBlocksStream>(nBlockSize, nPageSize, cfg.nCoresCount);

  SignatureFile signatures(getTotalBlocksInFile(pInput, nBlockSize));

  // Starting workers:
  std::vector<std::thread> workers;
  workers.reserve(cfg.nCoresCount);
  for (size_t i = 0; i < cfg.nCoresCount; ++i) {
    workers.emplace_back([&signatures, &pSourceStream]() {
      Worker worker;
      worker.run(pSourceStream, signatures);
    });
  }

  // Proceeding file
  try {
    pSourceStream->proceed(pInput);
  } catch (std::exception& exception) {
    std::cerr << "Unexpected error: " << exception.what() << "! Terminated!";
    return 1;
  }

  // Waiting forkers to finish
  for (std::thread& worker : workers)
    worker.join();

  // Writing signature file
  signatures.dumpToFile(pOutput);
  return 0;
}
