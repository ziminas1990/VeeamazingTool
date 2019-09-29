#pragma once
#include "IBlocksStream.h"
#include <atomic>
#include <shared_mutex>
#include <exception>

class SimpleBlocksStream : public IBlocksStream
{
public:
  SimpleBlocksStream() = default;
  SimpleBlocksStream(SimpleBlocksStream const& other) = delete;
  SimpleBlocksStream(SimpleBlocksStream && other) = delete;

  void reset(uint8_t const* pBlocks, size_t nBlockSize,
             size_t nFirstBlockId, size_t nLastBlockId)
  {
    m_lEndOfStream = true;  // to prevent modification from yeild() call while resetting

    m_pBlocks       = pBlocks;
    m_nBlockSize    = nBlockSize;
    m_nFirstBlockId = nFirstBlockId;
    m_nLastBlockId  = nLastBlockId;
    m_nNextBlockId  = nFirstBlockId;
    m_lEndOfStream  = false;
  }

  Block yield() override {
    if (m_lEndOfStream)
      return Block();
    size_t nBlockId = m_nNextBlockId++;
    if (nBlockId > m_nLastBlockId) {
      m_lEndOfStream = true;
      return Block();
    }
    return Block(nBlockId, m_nBlockSize, getBlockPtr(nBlockId));
  }

private:
  uint8_t const* getBlockPtr(size_t nBlockId) const {
    return  m_pBlocks + (nBlockId - m_nFirstBlockId) * m_nBlockSize;
  }

private:
  uint8_t const* m_pBlocks       = nullptr;
  size_t         m_nBlockSize    = 0;
  size_t         m_nFirstBlockId = 0;
  size_t         m_nLastBlockId  = 0;

  std::atomic<bool>   m_lEndOfStream = true;
  std::atomic<size_t> m_nNextBlockId = 0;
};
