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

#include <tdebug.h>
#include "avifile.h"
#include "aviproperties.h"

using namespace TagLib;

class RIFF::AVI::Properties::PropertiesPrivate
{
public:
  PropertiesPrivate() :
    format(0),
    length(0),
    bitrate(0),
    sampleRate(0),
    channels(0),
    bitsPerSample(0),
    sampleFrames(0) {}

  int format;
  int length;
  int bitrate;
  int sampleRate;
  int channels;
  int bitsPerSample;
  unsigned int sampleFrames;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

TagLib::RIFF::AVI::Properties::Properties(File *file, ReadStyle style) :
  AudioProperties(style),
  d(new PropertiesPrivate())
{
  read(file);
}

RIFF::AVI::Properties::~Properties()
{
  delete d;
}

int RIFF::AVI::Properties::length() const
{
  return d->length / 1000;
}

int RIFF::AVI::Properties::bitrate() const
{
  return d->bitrate;
}

int RIFF::AVI::Properties::sampleRate() const
{
  return d->sampleRate;
}

int RIFF::AVI::Properties::channels() const
{
  return d->channels;
}


////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void RIFF::AVI::Properties::read(File *file)
{
  ByteVector data;

  for(unsigned int i = 0; i < file->chunkCount(); ++i) {
    const ByteVector name = file->chunkName(i);
    if(name == "LIST") {
      data = file->chunkData(i);

      if (data.startsWith("hdrl")) {
        const unsigned int avihBlockOffset = 4;

        readAVIHeader(file, data, avihBlockOffset);
      }
    }
  }
}

static const int AVIF_HASINDEX = 0x00000010;

void RIFF::AVI::Properties::readAVIHeader(File *file, const ByteVector &data, unsigned int avihBlockOffset)
{
    const ByteVector& avihBlockName = data.mid(avihBlockOffset, 4);

    if (avihBlockName == "avih") {
      const unsigned int chunkSizeOffset = avihBlockOffset + 4;

      const size_t blockSize = data.mid(chunkSizeOffset, 4).toUInt(false);
      const size_t expectedBlockSize = 0x38;

      if (blockSize != expectedBlockSize) {
        debug("Corrupted avi header found");
        return;
      }

      const unsigned int dataOffset = 12;

      const unsigned int flagsOffset = dataOffset + 12;
      const size_t flags = data.mid(flagsOffset, 4).toUInt(false);

      /*
       * Dirty hack that allows avi file to be valid.
       * AVI container has AVI Index Entry, that is considered by RIFF File reading as
       * invalid chunk, because index entry doesn't have a name, it's first entry is an ID,
       * which is out of ASCII chars. So, RIFF::File consider file as invalid, that is not true.
       * Here we fix this status by reading HASINDEX value in AVI header flags.
       */

      if ((flags & AVIF_HASINDEX) && file->lastError == File::ParseStatus::ERROR_INVALID_CHUNK_NAME) {
        file->setValid(true);
        file->lastError = File::ParseStatus::NO_ERROR;
      }

      const unsigned int microSecondsBlockOffset = dataOffset;
      const unsigned int totalFramesBlockOffset = dataOffset + 16;

      const size_t microSecondsPerFrame = data.mid(microSecondsBlockOffset, 4).toUInt(false);
      const size_t totalFrames = data.mid(totalFramesBlockOffset, 4).toUInt(false);

      d->length = static_cast<int>(totalFrames * microSecondsPerFrame / 1000);
    } else {
      debug("AVI: No 'avih' chunk found");
      return;
    }
}
