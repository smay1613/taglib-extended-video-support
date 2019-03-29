#ifndef TAGLIB_EBMLELEMENT_H
#define TAGLIB_EBMLELEMENT_H
#include "matroskatypes.h"
#include <tbytevector.h>

namespace TagLib {
  namespace Matroska {

    class EBMLElement
    {
    public:
      EBMLElement();
      EBMLElement(MatroskaID ebmlId, const ByteVector& data);
      String getString() const;
      ulong getUInt() const;
      double getDouble() const;
    private:
      MatroskaID id;
      ByteVector data;
    };

  }
}

#endif // EBMLELEMENT_H