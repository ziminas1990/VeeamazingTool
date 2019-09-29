#include "FileDevice.h"

#include <iostream>

bool FileDevice::open(std::string const& sFileName, Mode eMode)
{
  std::fstream::openmode mode = std::fstream::binary;
  switch (eMode) {
    case eReadOnly:
      mode |= std::fstream::in;
      break;
    case eWriteOnly:
      mode |= std::fstream::out;
      break;
  }

  m_stream.open(sFileName.c_str(), mode);
  return m_stream.good();
}

size_t FileDevice::getSize()
{
  auto nCurrent = m_stream.tellg();
  m_stream.seekg(0, std::ios::end);
  size_t nSize = m_stream.tellg();
  m_stream.seekg(nCurrent, std::ios::beg);
  return nSize;
}

size_t FileDevice::read(long nOffset, long nBlockSize, uint8_t *pDestionation)
{
  m_stream.seekg(nOffset, std::ios_base::beg);
  if (!m_stream)
    return 0;
  m_stream.read(reinterpret_cast<char*>(pDestionation), nBlockSize);
  auto nActuallyReadBytes = m_stream.gcount();
  return (nActuallyReadBytes > 0) ? static_cast<size_t>(nActuallyReadBytes) : 0;
}

void FileDevice::write(long nOffset, long nBlockSize, const uint8_t *pSource)
{
  m_stream.seekp(nOffset, std::ios_base::beg);
  if (!m_stream)
    return;
  m_stream.write(reinterpret_cast<char const*>(pSource), nBlockSize);
  if (!m_stream)
    std::cerr << "Write error!" << std::endl;
}
