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

#ifndef TAGLIB_MATROSKAPROPERTIES_H
#define TAGLIB_MATROSKAPROPERTIES_H

#include "audioproperties.h"

namespace TagLib {

  namespace Matroska {
    class EBMLReader;
    class File;

    //! An implementation of audio property reading for Matroska

    /*!
     * This reads the data from a Matroska stream found in the AudioProperties
     * API.
     */

    class TAGLIB_EXPORT Properties : public TagLib::AudioProperties
    {
    public:
      /*!
       * Create an instance of Matroska::AudioProperties with the data read from the
       * EBMLReader \a data.
       */
      Properties(const Matroska::EBMLReader &data, ReadStyle style = ReadStyle::Average);

      /*!
       * Destroys this Matroska::AudioProperties instance.
        */
      virtual ~Properties();

      // Reimplementations.

      virtual int length() const;
      // STUB! Not implemented.
      virtual int bitrate() const;
      // STUB! Not implemented.
      virtual int sampleRate() const;
      // STUB! Not implemented.
      virtual int channels() const;

    private:
      Properties(const Properties &);
      Properties &operator=(const Properties &);

      void read(const TagLib::Matroska::EBMLReader &data);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }
}

#endif

