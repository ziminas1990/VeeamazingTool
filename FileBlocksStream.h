#pragma once

#include <memory>
#include <mutex>
#include <atomic>
#include <boost/fiber/barrier.hpp>

#include "IBlocksStream.h"
#include "IBlockDevice.h"
#include "SimpleBlocksStream.h"

class FileBlocksStream : public IBlocksStream
{
  enum InternalState {
    ePreparing,
    eInProgress,
    eEndOfFile
  };

public:
  FileBlocksStream(size_t nBlockSize, size_t nPageSize, size_t nWorkersCount);

  void proceed(IBlockDevicePtr pFile);

  // overrides from IBlocksStream
  Block yield() override;

private:
  // returns total number of loaded blocks
  size_t loadBackgroundPage(IBlockDevicePtr pFile, size_t nFirstBlockIndex);

private:
  size_t m_nBlockSize;
  size_t m_nPageSize;
  std::unique_ptr<uint8_t[]> m_pActivePage;
  std::unique_ptr<uint8_t[]> m_pBackgroundPage;

  IBlockDeviceWeakPtr    m_pFile;
  SimpleBlocksStream     m_blocksQueue;
  std::atomic<bool>      m_lEndOfFile;
  boost::fibers::barrier m_greenBarrier;
  boost::fibers::barrier m_redBarrier;
};

using FileBlocksStreamPtr = std::shared_ptr<FileBlocksStream>;
