#include "attobuffer_internal.h"

#ifndef COMPRESSION_FORMAT_LZNT1
#define COMPRESSION_FORMAT_LZNT1 0x0002
#endif

#ifndef COMPRESSION_FORMAT_XPRESS
#define COMPRESSION_FORMAT_XPRESS 0x0003
#endif

#ifndef COMPRESSION_FORMAT_XPRESS_HUFF
#define COMPRESSION_FORMAT_XPRESS_HUFF 0x0004
#endif

#ifndef COMPRESSION_ENGINE_STANDARD
#define COMPRESSION_ENGINE_STANDARD 0x0000
#endif

#ifndef COMPRESSION_ENGINE_MAXIMUM
#define COMPRESSION_ENGINE_MAXIMUM 0x0100
#endif

typedef long NTSTATUS;

typedef NTSTATUS (*RtlCompressBufferFunc)(
    unsigned short CompressionFormat, unsigned char *SourceBuffer,
    unsigned long SourceBufferLength, unsigned char *DestinationBuffer,
    unsigned long DestinationBufferLength, unsigned long ChunkSize,
    unsigned long *FinalCompressedSize, void *WorkSpace);

typedef NTSTATUS (*RtlDecompressBufferFunc)(
    unsigned short CompressionFormat, unsigned char *DestinationBuffer,
    unsigned long DestinationBufferLength, unsigned char *SourceBuffer,
    unsigned long SourceBufferLength, unsigned long *FinalUncompressedSize);

typedef NTSTATUS (*RtlGetCompressionWorkSpaceSizeFunc)(
    unsigned short CompressionFormat,
    unsigned long *CompressBufferWorkSpaceSize,
    unsigned long *CompressFragmentWorkSpaceSize);

static RtlCompressBufferFunc GetRtlCompressBuffer() {
  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  if (!ntdll)
    return nullptr;
  return (RtlCompressBufferFunc)GetProcAddress(ntdll, "RtlCompressBuffer");
}

static RtlDecompressBufferFunc GetRtlDecompressBuffer() {
  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  if (!ntdll)
    return nullptr;
  return (RtlDecompressBufferFunc)GetProcAddress(ntdll, "RtlDecompressBuffer");
}

static RtlGetCompressionWorkSpaceSizeFunc GetRtlGetCompressionWorkSpaceSize() {
  HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
  if (!ntdll)
    return nullptr;
  return (RtlGetCompressionWorkSpaceSizeFunc)GetProcAddress(
      ntdll, "RtlGetCompressionWorkSpaceSize");
}

namespace attoboy {

Buffer &Buffer::trim() {
  if (!impl)
    return *this;

  WriteLockGuard guard(&impl->lock);

  if (impl->size == 0) {
    if (impl->data) {
      FreeBufferData(impl->data);
      impl->data = AllocBufferData(512);
      impl->capacity = 512;
    }
    return *this;
  }

  if (impl->capacity > impl->size) {
    unsigned char *newData = AllocBufferData(impl->size);
    if (newData) {
      for (int i = 0; i < impl->size; i++) {
        newData[i] = impl->data[i];
      }
      FreeBufferData(impl->data);
      impl->data = newData;
      impl->capacity = impl->size;
    }
  }

  return *this;
}

Buffer Buffer::compress() const {
  Buffer result;
  if (!impl || impl->size == 0) {
    return result;
  }

  ReadLockGuard guard(&impl->lock);

  RtlGetCompressionWorkSpaceSizeFunc getWorkSpaceSize =
      GetRtlGetCompressionWorkSpaceSize();
  RtlCompressBufferFunc compressBuffer = GetRtlCompressBuffer();
  if (!getWorkSpaceSize || !compressBuffer) {
    return result;
  }

  unsigned long workSpaceSize = 0;
  unsigned long fragmentWorkSpaceSize = 0;
  NTSTATUS status = getWorkSpaceSize(COMPRESSION_FORMAT_LZNT1,
                                     &workSpaceSize, &fragmentWorkSpaceSize);

  if (status != 0 || workSpaceSize == 0) {
    return result;
  }

  unsigned long totalWorkSpaceSize = workSpaceSize + fragmentWorkSpaceSize;

  if (totalWorkSpaceSize < workSpaceSize) {
    return result;
  }

  void *workSpace = VirtualAlloc(NULL, totalWorkSpaceSize,
                                 MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!workSpace) {
    return result;
  }

  unsigned long maxCompressedSize = ((unsigned long)impl->size * 2) + 1024;
  unsigned char *compressedData = AllocBufferData(maxCompressedSize);
  if (!compressedData) {
    VirtualFree(workSpace, 0, MEM_RELEASE);
    return result;
  }

  unsigned long finalCompressedSize = 0;
  status = compressBuffer(
      COMPRESSION_FORMAT_LZNT1, impl->data,
      impl->size, compressedData, maxCompressedSize, 4096, &finalCompressedSize,
      workSpace);

  VirtualFree(workSpace, 0, MEM_RELEASE);

  if (status == 0 && finalCompressedSize > 0) {
    WriteLockGuard resultGuard(&result.impl->lock);

    if (result.impl->capacity < (int)finalCompressedSize) {
      FreeBufferData(result.impl->data);
      int newCapacity = (int)finalCompressedSize;
      result.impl->data = AllocBufferData(newCapacity);
      result.impl->capacity = newCapacity;
    }

    if (result.impl->data) {
      for (unsigned long i = 0; i < finalCompressedSize; i++) {
        result.impl->data[i] = compressedData[i];
      }
      result.impl->size = (int)finalCompressedSize;
    }

    FreeBufferData(compressedData);
  } else {
    FreeBufferData(compressedData);
  }
  return result;
}

Buffer Buffer::decompress() const {
  Buffer result;
  if (!impl || impl->size == 0)
    return result;

  ReadLockGuard guard(&impl->lock);

  RtlDecompressBufferFunc decompressBuffer = GetRtlDecompressBuffer();
  if (!decompressBuffer)
    return result;

  int estimatedSize = impl->size * 64;
  if (estimatedSize < 65536)
    estimatedSize = 65536;

  unsigned char *decompressedData = AllocBufferData(estimatedSize);
  if (!decompressedData)
    return result;

  unsigned long finalUncompressedSize = 0;
  NTSTATUS status = decompressBuffer(
      COMPRESSION_FORMAT_LZNT1,
      decompressedData, estimatedSize, impl->data, impl->size,
      &finalUncompressedSize);

  if (status == 0 && finalUncompressedSize > 0) {
    WriteLockGuard resultGuard(&result.impl->lock);

    if (result.impl->capacity < (int)finalUncompressedSize) {
      FreeBufferData(result.impl->data);
      int newCapacity = (int)finalUncompressedSize;
      result.impl->data = AllocBufferData(newCapacity);
      result.impl->capacity = newCapacity;
    }

    if (result.impl->data) {
      for (unsigned long i = 0; i < finalUncompressedSize; i++) {
        result.impl->data[i] = decompressedData[i];
      }
      result.impl->size = (int)finalUncompressedSize;
    }

    FreeBufferData(decompressedData);
  } else {
    FreeBufferData(decompressedData);
  }

  return result;
}

} // namespace attoboy
