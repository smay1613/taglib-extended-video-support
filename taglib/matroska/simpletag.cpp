#include "simpletag.h"

using namespace TagLib;

Matroska::SimpleTag::SimpleTag()
  : m_isBinary (false)
{

}

void Matroska::SimpleTag::setBinary(bool isBinary)
{
  m_isBinary = isBinary;
}

ByteVector Matroska::SimpleTag::value() const
{
  return m_value;
}

void Matroska::SimpleTag::setValue(const ByteVector &value)
{
  m_value = value;
}

std::map<String, std::vector<Matroska::SimpleTag>>& Matroska::SimpleTag::simpleTags()
{
  return m_simpleTags;
}
