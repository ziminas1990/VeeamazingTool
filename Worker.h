#pragma once

#include "IBlocksStream.h"
#include "SignatureFile.h"

class Worker
{
public:
  void run(IBlocksStreamPtr pBlockStream, SignatureFile& pSignatureFile);
};
