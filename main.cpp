#include "IBlocksStream.h"
#include <thread>

#include <boost/fiber/barrier.hpp>

#include "SimpleBlocksStream.h"
#include "FileBlocksStream.h"
#include "FileDevice.h"
#include "Worker.h"
#include "SignatureFile.h"

inline size_t getTotalBlocksInFile(IBlockDevice /*const*/ & device, size_t nBlockSize)
{
  size_t nTotalLength = device.getSize();
  return nTotalLength / nBlockSize + (nTotalLength % nBlockSize ? 1 : 0);
}

int main(int argc, char* argv[])
{
  size_t nBlockSize = 4;

  FileDevice source;
  source.open(argv[1], FileDevice::eReadOnly);

  FileDevice dest;
  dest.open(argv[2], FileDevice::eWriteOnly);

  size_t nTotalBlocks = getTotalBlocksInFile(source, nBlockSize);

  SignatureFile signatures(nTotalBlocks);

  FileBlocksStreamPtr pSourceStream =
      std::make_shared<FileBlocksStream>(nBlockSize, 256, 4);

  std::thread workerThread1([&signatures, &pSourceStream]() {
    Worker worker;
    worker.run(pSourceStream, signatures);
  });
  std::thread workerThread2([&signatures, &pSourceStream]() {
    Worker worker;
    worker.run(pSourceStream, signatures);
  });
  std::thread workerThread3([&signatures, &pSourceStream]() {
    Worker worker;
    worker.run(pSourceStream, signatures);
  });
  std::thread workerThread4([&signatures, &pSourceStream]() {
    Worker worker;
    worker.run(pSourceStream, signatures);
  });

  pSourceStream->proceed(source);

  std::cout << "Proceeding finished! Waiting for workers..." << std::endl;
  workerThread1.join();
  workerThread2.join();
  workerThread3.join();
  workerThread4.join();

  signatures.dumpToFile(dest);

  return 0;
}
