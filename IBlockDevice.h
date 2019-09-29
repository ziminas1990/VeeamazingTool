#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <memory>

class IBlockDevice
{
public:
  virtual ~IBlockDevice() = default;

  virtual size_t getSize() /*const*/ = 0;

  // returns total read bytes
  virtual size_t read(long nOffset, long nBlockSize, uint8_t* pDestionation) = 0;

  virtual void write(long nOffset, long nBlockSize, uint8_t const* pSource) = 0;
};

using IBlockDevicePtr     = std::shared_ptr<IBlockDevice>;
using IBlockDeviceWeakPtr = std::weak_ptr<IBlockDevice>;
