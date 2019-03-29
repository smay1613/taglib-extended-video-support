/***************************************************************************
    copyright            : (C) 2008 by Scott Wheeler
    email                : wheeler@kde.org
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
#include <tagutils.h>

#include "avifile.h"
#include "infotag.h"

using namespace TagLib;

class RIFF::AVI::File::FilePrivate
{
public:
  FilePrivate() :
    properties(0),
    tag(0) {}

  ~FilePrivate()
  {
    delete properties;
    delete tag;
  }

  Properties *properties;
  RIFF::Info::Tag *tag;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

RIFF::AVI::File::File(FileName file, bool readProperties, Properties::ReadStyle) :
  RIFF::File(file, LittleEndian),
  d(new FilePrivate())
{
  if(isOpen())
    read(readProperties);
}

RIFF::AVI::File::File(IOStream *stream, bool readProperties, Properties::ReadStyle) :
  RIFF::File(stream, LittleEndian),
  d(new FilePrivate())
{
  if(isOpen())
    read(readProperties);
}

RIFF::AVI::File::~File()
{
  delete d;
}

RIFF::Info::Tag *RIFF::AVI::File::tag() const
{
  return d->tag;
}

bool RIFF::AVI::File::save()
{
  // STUB!
  return false;
}

RIFF::AVI::Properties *RIFF::AVI::File::audioProperties() const
{
  return d->properties;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void RIFF::AVI::File::read(bool readProperties)
{
  for(unsigned int i = 0; i < chunkCount(); ++i) {
    const ByteVector name = chunkName(i);

    if(name == "LIST") {
        const ByteVector &data = chunkData(i);
        if(data.startsWith("INFO")) {
            d->tag = new RIFF::Info::Tag(data);
        }
    }
  }

  if(readProperties)
    d->properties = new Properties(this, Properties::Average);
}
