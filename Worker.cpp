#include "Worker.h"

// For Debugging
#include <iostream>
#include <string>
#include <thread>

void Worker::run(IBlocksStreamPtr pBlockStream, SignatureFile& pSignatureFile)
{
  Block block = pBlockStream->yield();
  while (block.isValid()) {

    std::cout << "Worker::run: Got block #" << block.m_nIndex << " (" <<
                 block.m_nSize << " bytes): " <<
                 std::string((const char*)block.m_pData, block.m_nSize) << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(block.m_nIndex % 4));

    uint32_t nSignature = (block.m_pData[3] << 24) + (block.m_pData[2] << 16) +
        (block.m_pData[1] << 8) + block.m_pData[0];
    pSignatureFile.addSignature(block.m_nIndex, nSignature);
    block = pBlockStream->yield();
  }
}
