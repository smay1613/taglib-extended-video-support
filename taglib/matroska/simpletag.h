#ifndef SIMPLETAG_H
#define SIMPLETAG_H
#include <map>
#include <vector>
#include <tstring.h>
#include <tbytevector.h>

namespace TagLib {
  namespace Matroska {

    class SimpleTag : public TagLib::ByteVector
    {
    public:
      SimpleTag();
      void setBinary(bool isBinary);

      ByteVector value() const;
      void setValue(const ByteVector &value);

      std::map<String, std::vector<SimpleTag> >& simpleTags();
    private:
      ByteVector m_value;
      std::map<String, std::vector<SimpleTag> > m_simpleTags;
      bool m_isBinary;
    };
  }
}


#endif // SIMPLETAG_H