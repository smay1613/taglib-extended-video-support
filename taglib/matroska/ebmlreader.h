#ifndef TAGLIB_EBMLREADER_H
#define TAGLIB_EBMLREADER_H
#include "matroskafile.h"
#include "matroskatypes.h"

namespace TagLib {
  namespace Matroska {

    class EBMLReader
    {
    public:
      EBMLReader(File *_file, ulong position);
      EBMLReader(const EBMLReader &parent, ulong position, uint id = 0, ulong size = 0);

      MatroskaID id() const;
      ulong size() const;
      ulong getDataSize() const;
      ulong getDataOffset() const;
      ulong getOffset() const;
      const EBMLReader *getParent() const;

      bool isValid() const;

      bool read();
      String readString() const;
      ByteVector readBytes() const;
      ulong readUInt() const;
      double readDouble() const;

    private:
      File* file;
      const EBMLReader *parent;

      ulong offset;
      uint ebmlId;

      ulong dataOffset;
      ulong dataSize;

      bool isSuccessRead;

      bool isAbstract() const;

      char getVINTLength(int maxSize, const ByteVector &data, int &mask) const;
    };

  }
}


#endif // EBMLREADER_H