#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <functional>
#include <memory>

struct Block
{
  Block() = default;
  Block(size_t nIndex, size_t nSize, uint8_t const* pData)
    : m_nIndex(nIndex), m_nSize(nSize), m_pData(pData)
  {}

  bool isValid() const { return m_pData != nullptr; }

  size_t         m_nIndex = 0;
  size_t         m_nSize  = 0;
  uint8_t const* m_pData  = nullptr;
};

class IBlocksStream
{
public:
  virtual ~IBlocksStream() = default;

  virtual Block yield() = 0;
};

using IBlocksStreamPtr = std::shared_ptr<IBlocksStream>;
