#include "SignatureFile.h"

#include <stdexcept>

SignatureFile::SignatureFile(size_t nTotalSignatures)
  : m_nSignatures(nTotalSignatures)
{}

void SignatureFile::addSignature(size_t nBlockId, uint32_t nBlockSignature)
{
  if (nBlockId >= m_nSignatures.size()) {
    throw std::out_of_range("");
  }
  // It should be threadsafe, because two different workers will never get block with
  // the same nBlockId
  m_nSignatures[nBlockId] = nBlockSignature;
}

void SignatureFile::dumpToFile(IBlockDevicePtr pFile)
{
  pFile->write(0, m_nSignatures.size() * sizeof(uint32_t),
               reinterpret_cast<uint8_t*>(m_nSignatures.data()));
}
