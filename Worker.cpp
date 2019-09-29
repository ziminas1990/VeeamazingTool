#include "Worker.h"

#include <boost/crc.hpp>

void Worker::run(IBlocksStreamPtr pBlockStream, SignatureFile& pSignatureFile)
{
  Block block = pBlockStream->yield();
  while (block.isValid()) {
    // process_bytes() is not a pure function, so we have to create new crcCalculator
    // instance for each block
    boost::crc_32_type crcCalculator;
    crcCalculator.process_bytes(block.m_pData, block.m_nSize);
    uint32_t nSignature = crcCalculator.checksum();

    pSignatureFile.addSignature(block.m_nIndex, nSignature);
    block = pBlockStream->yield();
  }
}
