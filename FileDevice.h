#pragma once

#include "IBlockDevice.h"
#include <memory>
#include <fstream>

class FileDevice : public IBlockDevice
{
public:
  enum Mode {
    eReadOnly,
    eWriteOnly
  };

  FileDevice() = default;
  ~FileDevice() { m_stream.close(); }

  bool open(std::string const& sFileName, Mode eMode);

  // overrides from IBlockDevice interface
  size_t getSize() override;
  size_t read(long nOffset, long nBlockSize, uint8_t *pDestionation)  override;
  void   write(long nOffset, long nBlockSize, const uint8_t *pSource) override;

private:
  std::fstream m_stream;
};

using FileDevicePtr = std::shared_ptr<FileDevice>;
