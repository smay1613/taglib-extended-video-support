/***************************************************************************
 copyright            : (C) 2013 - 2018 by Stephen F. Booth
 email                : me@sbooth.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <tbytevector.h>
#include <tdebug.h>
#include "tstring.h"
#include "matroskafile.h"
#include "tag.h"
#include "ebmlreader.h"
#include "matroskatypes.h"
#include "simpletag.h"

using namespace TagLib;

class Matroska::File::FilePrivate
{
public:
  FilePrivate() :
    properties(0),
    unifiedTag(0)
  {
  }

  ~FilePrivate()
  {
    delete properties;
    delete unifiedTag;
    for (std::vector<Tag*>::iterator i = m_tags.begin(); i != m_tags.end(); ++i) {
        delete (*i);
      }
    m_tags.clear();
  }

  Properties *properties;
  std::vector<Tag*> m_tags;
  Tag* unifiedTag;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Matroska::File::File(IOStream *stream, bool readProperties,
                     Properties::ReadStyle propertiesStyle) :
  TagLib::File(stream),
  d(new FilePrivate())
{
  if(isOpen())
    read(readProperties, propertiesStyle);
}

Matroska::File::File(FileName file, bool readProperties,
                     Properties::ReadStyle propertiesStyle) :
  TagLib::File(file),
  d(new FilePrivate())
{
  if(isOpen())
    read(readProperties, propertiesStyle);
}

Matroska::File::~File()
{
  delete d;
}

Matroska::Properties *Matroska::File::audioProperties() const
{
  return d->properties;
}

Matroska::Tag *Matroska::File::tag() const
{
  return d->unifiedTag;
}

bool Matroska::File::save()
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////


void Matroska::File::read(bool readProperties, Properties::ReadStyle propertiesStyle)
{
  ulong offset = readLeadText();

  if (offset == -1u) {
      setValid(false);
      return;
    }

  bool hasSegment = false;
  while (offset < static_cast<ulong>(length())) {
      EBMLReader element(this, offset);

      if (!element.isValid()) {
          debug("Matroska file - invalid element found"); // Sometimes, the file has zero padding at the end
          setValid(false);
          if (hasSegment) {
              break;
            }
          return;
        }

      EBMLID ebmlId = static_cast<EBMLID>(element.id());
      MatroskaID matroskaId = element.id();

      switch (ebmlId) {
        case EBMLHeader:
          readHeader (element);
          break;
        default:
          break;
        }

      switch (matroskaId) {
        case Segment:
          readSegment(element, propertiesStyle);
          hasSegment = true;
          break;
        default:
          break;
        }

      offset += element.size();
    }
}

ulong Matroska::File::readLeadText()
{
  ulong offset = 0;

  // look up the 0x1A start byte
  const int buffer_size = 64;
  int index;
  do {
      seek(static_cast<long>(offset));
      const ByteVector& leadText = readBlock(buffer_size);
      index = leadText.find(0x1A);
      offset += buffer_size;
    } while (index < 0 && offset < static_cast<ulong>(length()));

  if (index < 0) {
      debug("Invalid Matroska file, missing data 0x1A.");
      return -1u;
    }

  offset = (offset + static_cast<ulong>(index)) - buffer_size;

  return offset;
}

void Matroska::File::readHeader(const EBMLReader &header)
{
  String docType;
  ulong i = 0;

  while (i < header.getDataSize()) {
      EBMLReader child (header, header.getDataOffset() + i);

      EBMLID ebml_id = static_cast<EBMLID>(child.id());

      switch (ebml_id) {
        case EBMLDocType:
          docType = child.readString ();
          break;
        default:
          break;
        }

      i += child.size();
    }

  if (docType.isEmpty() || (docType != "matroska")) {
      debug("DocType is not matroska or webm");
      setValid(false);
    }
}

void Matroska::File::readSegment(const Matroska::EBMLReader &element, Properties::ReadStyle propertiesStyle, bool retry)
{
  // First make reference of all EBML elements at level 1 (top) in the Segment
  std::vector<Matroska::EBMLReader> segmentationList = readSegments(element, retry); // Try to get it from SeekHead the first time (way faster)

  // Now process (read) the referenced elements we care about

  bool isValid = false;
  for (std::vector<Matroska::EBMLReader>::iterator child = segmentationList.begin();
       child < segmentationList.end();
       child++) {
      // the child here may be Abstract if it has been retrieved in the SeekHead,
      // so child.Read() must be used to retrieve the full EBML header
      MatroskaID matroskaId = child->id();

      switch (matroskaId) {
        case SeekHead:
          isValid = child->read();
          break;

        case SegmentInfo:
          if ((isValid = child->read())) {
              readSegmentInfo(*child);
            }
          break;
        case Tags:
          isValid = child->read();
          if (isValid) {
              readTags(*child);
              makeUnifiedTag();
            }
          break;

        case CRC32: // We don't support it
          isValid = child->read();
          break;

        default:
          break;
        }

      if (!isValid) {
          break;
        }
    }
  if (!isValid) {
      if (retry) {
          debug("Invalid Meta Seek");
          setValid(false);

          // Retry the readSegment without using SeekHead

          d->m_tags.clear ();

          // Retry it one last time
          readSegment (element, propertiesStyle, false);
        } else {
          debug ("Invalid EBML element Read");
          setValid(false);
        }
    }
}

std::vector<Matroska::EBMLReader> Matroska::File::readSegments(const EBMLReader &element, bool allowSeekHead)
{
  std::vector<EBMLReader> segmentsList;

  bool foundCluster = false;

  ulong i = 0;

  while (i < element.getDataSize()) {
      EBMLReader child (element, element.getDataOffset() + i);
      if (!child.isValid()) {
          debug("Truncated file or invalid EBML entry");
          setValid(false);
          break;
        }
      MatroskaID matroskaId = child.id();
      bool refInSeekHead = false;

      switch (matroskaId) {
        case SeekHead:
          if (allowSeekHead) {
              // Take only the first SeekHead into account
              std::vector<EBMLReader> ebmlSeekList;
              ebmlSeekList.push_back(child);
              if (readSeekHead (child, ebmlSeekList)) {
                  // Always reference the first element
                  if (ebmlSeekList[0].getOffset() > element.getDataOffset())
                    ebmlSeekList.insert(ebmlSeekList.begin(), segmentsList[0]);

                  segmentsList = ebmlSeekList;
                  i = element.getDataSize(); // Exit the loop: we got what we need
                } else {
                  debug("Invalid Meta Seek");
                  setValid(false);
                  refInSeekHead = true;
                }
            } else {
              refInSeekHead = true;
            }
          break;

        case Cluster: // reference first Cluster only (too many)
          refInSeekHead = !foundCluster;
          foundCluster = true;
          break;

          // Reference the following elements
        case Cues:
        case Tracks:
        case SegmentInfo:
        case Tags:
        case Attachments:
        default:
          refInSeekHead = true;
          break;
        }

      i += child.size();

      if (refInSeekHead || i == 0) {
          segmentsList.push_back(child);
        }
    }

  return segmentsList;
}

void Matroska::File::readSegmentInfo(const Matroska::EBMLReader &element)
{
  if (!d->properties) {
      d->properties = new Matroska::Properties(element);
    }
}

bool Matroska::File::readSeekHead(const Matroska::EBMLReader &element, std::vector<EBMLReader> &segmentationList)
{
  MatroskaID ebmlId;
  ulong ebmlPosition = 0;

  ulong i = 0;
  while (i < element.getDataSize()) {
      EBMLReader ebmlSeek (element, element.getDataOffset() + i);
      MatroskaID matroskaId = ebmlSeek.id();

      if (matroskaId == CRC32) // Skip the CRC-32 element
        {
          i += ebmlSeek.size();
          continue;
        }

      if (matroskaId != Seek) {
          return false; // corrupted SeekHead
        }

      ulong j = 0;
      while (j < ebmlSeek.getDataSize()) {
          EBMLReader child (ebmlSeek, ebmlSeek.getDataOffset() + j);
          matroskaId = child.id();

          switch (matroskaId) {
            case SeekID:
              ebmlId = static_cast<MatroskaID>(child.readUInt());
              break;
            case SeekPosition:
              ebmlPosition = child.readUInt() + element.getOffset();
              break;
            default:
              break;
            }

          j += child.size();
        }

      if (static_cast<int>(ebmlId) > 0 && ebmlPosition > 0) {
          // Create abstract EBML representation of the segment EBML
          EBMLReader ebml (*element.getParent(), ebmlPosition, static_cast<uint>(ebmlId));

          int k = 0;
          // Sort the seek-entries by increasing position order
          for (k = static_cast<int>(segmentationList.size() - 1); k >= 0; k--) {
              if (ebmlPosition > segmentationList[k].getOffset()) {
                  break;
                }
            }
          segmentationList.insert (segmentationList.begin() + k + 1, ebml);

          // Chained SeekHead recursive read
          if (ebmlId == SeekHead) {
              if (!ebml.isValid()) {
                  return false; // Corrupted
                }
              readSeekHead (ebml, segmentationList);
            }
        }

      i += ebmlSeek.size();
    }

  return true;
}

void Matroska::File::readTags(const Matroska::EBMLReader &element) const
{
  ulong i = 0;

  while (i < static_cast<ulong>(element.getDataSize())) {
      EBMLReader child (element, element.getDataOffset() + i);

      MatroskaID matroskaId = child.id();

      switch (matroskaId) {
        case TagID:
          readTag (child);
          break;
        default:
          break;
        }

      i += child.size();
    }
}

void Matroska::File::readTag(const Matroska::EBMLReader &element) const
{
  ulong i = 0;

  // Create new Tag
  Tag* tag = new Tag(d->m_tags);

  while (i < static_cast<ulong>(element.getDataSize())) {
      EBMLReader child (element, element.getDataOffset() + i);

      MatroskaID matroskaId = child.id();
      switch (matroskaId) {
        case Targets:
          readTargets (child, *tag);
          break;
        case SimpleTagID:
          readSimpleTag (child, tag);
          break;
        default:
          break;
        }

      i += child.size();
    }

  tag->read();
  d->m_tags.push_back(tag);
}

void Matroska::File::readTargets(const Matroska::EBMLReader &element, Matroska::Tag &tag) const
{
  ulong i = 0;

  unsigned int targetTypeValue = 0;
  String targetType;
  std::vector<UIDElement> uids;

  while (i < element.getDataSize()) {
      EBMLReader child (element, element.getDataOffset() + i);

      MatroskaID matroskaId = child.id();

      switch (matroskaId) {
        case TargetTypeValue:
          targetTypeValue = static_cast<unsigned int>(child.readUInt());
          break;
        case TargetTypeID:
          targetType = child.readString();
          break;
        case TagTrackUID:
        case TagEditionUID:
        case TagChapterUID:
        case TagAttachmentUID: {
            ulong uid = child.readUInt();
            // Value 0 => apply to all
            if (uid != 0) {
                uids.push_back(UIDElement (matroskaId, uid));
              }
            break;
          }
        default:
          break;
        }

      i += child.size();
    }

  if (targetTypeValue != 0) {
      tag.setTargetType(Tag::makeTargetType(targetTypeValue));
    }

  if (uids.size() > 0) {
      tag.setElements(uids);
    }
}

void Matroska::File::readSimpleTag(const Matroska::EBMLReader &element, Matroska::Tag *tag, Matroska::SimpleTag* simpletag) const
{
  ulong i = 0;
  String key;
  SimpleTag stag;

  while (i < element.getDataSize()) {
      EBMLReader child (element, element.getDataOffset() + i);

      MatroskaID matroskaId = child.id();

      switch (matroskaId) {
        case TagName:
          key = child.readString();
          break;
        case TagString:
          stag.setBinary(false);
          stag.setValue(child.readBytes ());;
          break;
        case TagBinary:
          stag.setBinary(true);
          stag.setValue(child.readBytes ());
          break;
        case SimpleTagID:
          readSimpleTag(child, NULL, &stag);
          break;
        default:
          break;
        }

      i += child.size();
    }

  // Add the SimpleTag reference to its parent
  if (!key.isEmpty()) {
      std::vector<SimpleTag> &entry = tag ? tag->simpleTags()[key]
                                            : simpletag->simpleTags()[key];
      entry.push_back(stag);
    }
}

void Matroska::File::makeUnifiedTag()
{
  d->unifiedTag = new Tag(d->m_tags);
  d->unifiedTag->read();
  for (std::vector<Tag*>::iterator i = d->m_tags.begin();
       i != d->m_tags.end(); ++i)
    {
      if (!(*i)) {
          continue;
        }
      switch ((*i)->getTargetType()) {
        case COLLECTION: {
            d->unifiedTag->setAlbum((*i)->album());
            if (d->unifiedTag->genre().isEmpty()) {
                d->unifiedTag->setGenre((*i)->genre());
              }
            break;
          }
        case MOVIE: {
            if (d->unifiedTag->track() == 0)
              d->unifiedTag->setTrack((*i)->track());

            if (d->unifiedTag->artist().isEmpty())
              d->unifiedTag->setArtist((*i)->artist());

            if (d->unifiedTag->title().isEmpty())
              d->unifiedTag->setTitle((*i)->title());

            if (d->unifiedTag->genre().isEmpty())
              d->unifiedTag->setGenre((*i)->genre());
            break;
          }
        }
    }
}

