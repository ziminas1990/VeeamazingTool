#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

#include "IBlockDevice.h"

// Just a wrapper for array, that represent signature file in memory
class SignatureFile
{
public:
  SignatureFile(size_t nTotalSignatures);

  void addSignature(size_t nBlockId, uint32_t nBlockSignature);

  void dumpToFile(IBlockDevicePtr pFile);

private:
  // index - blockId, value - block's signature
  std::vector<uint32_t> m_nSignatures;
};
