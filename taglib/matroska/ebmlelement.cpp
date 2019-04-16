#include "ebmlelement.h"
#include <tstring.h>

using namespace TagLib;

Matroska::EBMLElement::EBMLElement()
{

}

Matroska::EBMLElement::EBMLElement(Matroska::MatroskaID ebmlId, const ByteVector &_data)
  : id (ebmlId),
    data (_data)
{

}

String Matroska::EBMLElement::getString() const
{
  int nullTerminationIndex = data.find(0x00);
  if (nullTerminationIndex >= 0) {
      const ByteVector& string = data.mid(0, static_cast<unsigned int>(nullTerminationIndex));
      return String(string, String::UTF8);
    }
  return String(data, String::UTF8);
}

ulong Matroska::EBMLElement::getUInt() const
{
  return data.isEmpty() ? 0 : data.toUInt();
}

double Matroska::EBMLElement::getDouble() const
{
  double result = 0.0;

  if (data.size() == 4) {
      result = data.toFloat32BE(0);
    } else if (data.size() == 8) {
      result = data.toFloat64BE(0);
    }

  return result;
}
