#include "FileBlocksStream.h"

#include <assert.h>
#include <memory.h>

inline size_t getSuitablePageSize(size_t nPageSize, size_t nBlockSize)
{
  return nPageSize - nPageSize % nBlockSize;
}

FileBlocksStream::FileBlocksStream(
    size_t nBlockSize, size_t nPageSize, size_t nWorkersCount)
  : m_nBlockSize(nBlockSize),
    m_nPageSize(getSuitablePageSize(nPageSize, nBlockSize)),
    m_pActivePage(new uint8_t[m_nPageSize]),
    m_pBackgroundPage(new uint8_t[m_nPageSize]),
    m_lEndOfFile(false),
    m_greenBarrier(nWorkersCount + 1),
    m_redBarrier(nWorkersCount + 1)
{}


void FileBlocksStream::proceed(IBlockDevicePtr pFile)
{
  size_t nTotalLoadedBlocks = 0;
  while (!m_lEndOfFile) {
    // loading backgroung page while workers are processing active page
    size_t nLoadedBlocks = loadBackgroundPage(pFile, nTotalLoadedBlocks);
    m_redBarrier.wait();

    if (!nLoadedBlocks) {
      m_lEndOfFile = true;
      // all workers now on the green barrier - we should unlock them and exit
      m_greenBarrier.wait();
      return;
    }

    std::swap(m_pBackgroundPage, m_pActivePage);
    m_blocksQueue.reset(m_pActivePage.get(), m_nBlockSize, nTotalLoadedBlocks,
                        nTotalLoadedBlocks + nLoadedBlocks - 1);
    nTotalLoadedBlocks += nLoadedBlocks;

    // Unlocking workers to proceed active page
    m_greenBarrier.wait();
  }
}

Block FileBlocksStream::yield()
{
  Block block = m_blocksQueue.yield();
  while (!block.isValid() && !m_lEndOfFile) {
    m_redBarrier.wait();
    // This thread does nothing here, but thread in proceedFile() loads new batch of
    // blocks into m_blocksQueue
    m_greenBarrier.wait();
    block = m_blocksQueue.yield();
  }
  return block;
}

size_t FileBlocksStream::loadBackgroundPage(
    IBlockDevicePtr pFile, size_t nFirstBlockIndex)
{
  long nOffset = nFirstBlockIndex * m_nBlockSize;
  std::cout << "FileBlocksStream::loadBackgroundPage: Loading blocks from " <<
               nFirstBlockIndex << " (offset: " << nOffset << " bytes)" << std::endl;
  size_t nBytesRead = pFile->read(nOffset, m_nPageSize, m_pBackgroundPage.get());
  if (!nBytesRead) {
    std::cout << "Got 0 bytes" << std::endl;
    return 0;
  }
  size_t nTotalBlocksOnPage = nBytesRead / m_nBlockSize;
  size_t nLastBlockLength   = nBytesRead % m_nBlockSize;
  if (nLastBlockLength) {
    nTotalBlocksOnPage++;
    assert(nBytesRead + nLastBlockLength <= m_nPageSize);
    memset(m_pBackgroundPage.get() + nBytesRead, 0, nLastBlockLength);
  }
  std::cout << "FileBlocksStream::loadBackgroundPage: Got " << nBytesRead <<
               " bytes in " << nTotalBlocksOnPage << " blocks: " <<
            std::string((const char*)m_pBackgroundPage.get(), nBytesRead) << std::endl;
  return nTotalBlocksOnPage;
}
