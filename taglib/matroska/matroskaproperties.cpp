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

#include <tstring.h>
#include <tdebug.h>

#include "matroskaproperties.h"
#include "ebmlreader.h"

using namespace TagLib;

class Matroska::Properties::PropertiesPrivate
{
public:
  PropertiesPrivate() :
    duration(0),
    timeScale(0),
    length(0)
  {
  }

  int duration;
  unsigned int timeScale;
  // Computed
  unsigned int length;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Matroska::Properties::Properties(const Matroska::EBMLReader &data, ReadStyle style) : TagLib::AudioProperties(style)
{
  d = new PropertiesPrivate;
  read(data);
}

Matroska::Properties::~Properties()
{
  delete d;
}

int Matroska::Properties::length() const
{
  return d->length / 1000;
}

int Matroska::Properties::bitrate() const
{
  return 0;
}

int Matroska::Properties::sampleRate() const
{
  return 0;
}

int Matroska::Properties::channels() const
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void Matroska::Properties::read(const Matroska::EBMLReader &data)
{
  long long i = 0;
  while (i < data.getDataSize()) {
      EBMLReader child (data, data.getDataOffset() + i);
      MatroskaID matroskaId = child.id();

      switch (matroskaId) {
        case Duration: {
            d->duration = child.readDouble();
            if (d->timeScale > 0) {
                d->length = d->duration * (d->timeScale / 1000000);
              }
            break;
          }
        case TimeCodeScale: {
            d->timeScale = child.readUInt();
            if (d->duration > 0) {
                d->length = d->duration * (d->timeScale / 1000000);
              }
            break;
          }
        default:
          break;
        }


      i += child.size();
    }
}

