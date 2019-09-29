#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

#include "IBlockDevice.h"

class SignatureFile
{
public:
  SignatureFile(size_t nTotalSignatures);

  void addSignature(size_t nBlockId, uint32_t nBlockSignature);

  void dumpToFile(IBlockDevice& file);

private:
  // index - blockId, value - block's signature
  std::vector<uint32_t> m_nSignatures;
};
