#ifndef TAGLIB_EBMLREADER_H
#define TAGLIB_EBMLREADER_H
#include "matroskafile.h"
#include "matroskatypes.h"

namespace TagLib {
  namespace Matroska {

    class EBMLReader
    {
    public:
      EBMLReader(File *_file, long long position);
      EBMLReader(const EBMLReader &parent, long long position, uint id = 0, long long size = 0);

      MatroskaID id() const;
      long long size() const;
      long long getDataSize() const;
      long long getDataOffset() const;
      long long getOffset() const;
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

      long long offset;
      uint ebmlId;

      long long dataOffset;
      long long dataSize;

      bool isSuccessRead;

      bool isAbstract() const;

      char getVINTLength(int maxSize, const ByteVector &data, int &mask) const;
    };

  }
}


#endif // EBMLREADER_H