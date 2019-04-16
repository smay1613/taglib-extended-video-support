#include "ebmlreader.h"
#include "ebmlelement.h"
#include <tdebug.h>

using namespace TagLib;

Matroska::EBMLReader::EBMLReader(File *_file, ulong position)
  : file (_file),
    parent (0),
    offset (position),
    ebmlId (0),
    dataOffset (position),
    dataSize(0)
{
  isSuccessRead = read();
}

Matroska::EBMLReader::EBMLReader(const EBMLReader &parent, ulong position, uint id, ulong size)
  : offset(position),
    ebmlId(id),
    dataOffset(position),
    dataSize(size)
{
  file = parent.file;
  this->parent = &parent;
  isSuccessRead = read();
}

Matroska::MatroskaID Matroska::EBMLReader::id() const
{
  return static_cast<MatroskaID>(ebmlId);
}

ulong Matroska::EBMLReader::getDataSize() const
{
  return dataSize;
}

ulong Matroska::EBMLReader::size() const
{
  return (dataOffset - offset) + dataSize;
}

bool Matroska::EBMLReader::isValid() const
{
  return isSuccessRead;
}

String Matroska::EBMLReader::readString() const
{
  String result;
  if (!file || dataSize == 0) {
      return result;
    }
  ByteVector data (readBytes());
  EBMLElement ebml (id(), data);
  result = ebml.getString();
  return result;
}

ByteVector Matroska::EBMLReader::readBytes() const
{
  ByteVector result;

  if (!file || dataSize == 0) {
      return result;
    }

  file->seek(static_cast<long>(dataOffset));

  result = file->readBlock(dataSize);

  return result;
}

ulong Matroska::EBMLReader::readUInt() const
{
  ulong result = 0;
  if (!file || dataSize == 0) {
      return result;
    }
  ByteVector data (readBytes());
  EBMLElement ebml (id(), data);
  return ebml.getUInt();
}

double Matroska::EBMLReader::readDouble() const
{
  double result = 0.0;
  if (!file || dataSize == 0) {
      return result;
    }
  ByteVector data (readBytes());
  EBMLElement ebml (id(), data);
  return ebml.getDouble();
}

const Matroska::EBMLReader *Matroska::EBMLReader::getParent() const
{
  return parent;
}

ulong Matroska::EBMLReader::getOffset() const
{
  return offset;
}

ulong Matroska::EBMLReader::getDataOffset() const
{
  return dataOffset;
}

bool Matroska::EBMLReader::read()
{
  if (!isAbstract()) {
      return true;
    }

  if (!file) {
      debug("EBMLReader - null file");
      return false;
    }

  if (offset >= static_cast<ulong>(file->length()) - 1) {
      debug("EBMLReader - invalid offset");
      return false;
    }

  // Prepare for Consistency check
  uint ebmlIdCheck = ebmlId;
  ulong ebmlSizeCheck = size();
  file->seek(static_cast<long>(offset));

  int idMaxLength = 4;

  const ByteVector& headerByteVector = file->readBlock(1);
  int mask = 0x80;
  long idLength = getVINTLength(idMaxLength, headerByteVector, mask);

  if (idLength > idMaxLength) {
      debug("Invalid EBML format read");
      return false;
    }

  ByteVector idData = headerByteVector;
  if (idLength > 1) {
      idData.append(file->readBlock(static_cast<ulong>(idLength - 1)));
    }

  ebmlId = idData.toUInt();

  ByteVector sizeLengthVector = file->readBlock (1);
  int maxSizeLength = 8;

  mask = 0x80; // reset mask
  long sizeLength = getVINTLength(maxSizeLength, sizeLengthVector, mask);

  if (sizeLength > 8) {
      sizeLength = 1; // Special: Empty element (all zero state)
    } else {
      sizeLengthVector[0] &= static_cast<char>(mask - 1);  // Clear the marker bit
    }

  // Now read the rest of the EBML element size
  if (sizeLength > 1) {
      sizeLengthVector.append(file->readBlock(static_cast<ulong>(sizeLength) - 1));
    }

  dataSize = static_cast<ulong>(sizeLengthVector.toLongLong());

  // Special: Auto-size (0xFF byte)
  if (sizeLength == 1 && dataSize == 0x7F) {
      // Resolve auto-size to fill in to its containing element
      ulong bound = parent ? parent->offset + parent->size() : static_cast<ulong>(file->length());
      dataSize = bound - offset - static_cast<ulong>(idLength + sizeLength);
    }
  dataOffset = offset + static_cast<ulong>(idLength + sizeLength);
  // Consistency check: Detect descrepencies between read data and abstract data
  if ((ebmlIdCheck != 0 && ebmlIdCheck != ebmlId) ||
      (ebmlSizeCheck != 0 && ebmlSizeCheck != size())) {
      debug("Consistensy check failed");
      return false;
    }

  return true;
}

bool Matroska::EBMLReader::isAbstract() const
{
  return offset == dataOffset;
}

char Matroska::EBMLReader::getVINTLength(int maxSize, const ByteVector &data, int& mask) const
{
  if (!data.size()) {
      debug("Invalid data for reading VINT");
      return -1;
    }
  const char headerByte = data[0];

  char length = 1;

  // VINT size reading
  while (length <= maxSize && (headerByte & mask) == 0) {
      length++;
      mask >>= 1;
    }
  return length;
}
