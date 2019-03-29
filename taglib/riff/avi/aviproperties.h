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

#ifndef TAGLIB_AVIPROPERTIES_H
#define TAGLIB_AVIPROPERTIES_H

#include "taglib.h"
#include "audioproperties.h"

namespace TagLib {

  class ByteVector;

  namespace RIFF {

    namespace AVI {

      class File;

      //! An implementation of audio property reading for AVI

      /*!
       * This reads the data from an AVI stream found in the AudioProperties
       * API.
       */

      class TAGLIB_EXPORT Properties : public AudioProperties
      {
      public:
        /*!
         * Create an instance of AVI::Properties with the data read from the
         * AVI::File \a file.
         */
        Properties(File *file, ReadStyle style);

        /*!
         * Destroys this AVI::Properties instance.
         */
        virtual ~Properties();

        /*!
         * Returns the length of the file in seconds.  The length is rounded down to
         * the nearest whole second.
         */
        virtual int length() const;

        /*!
         * \warning STUB! Not implemented.
         * Returns the average bit rate of the file in kb/s.
         */
        virtual int bitrate() const;

        /*!
         * \warning STUB! Not implemented.
         * Returns the sample rate in Hz.
         */
        virtual int sampleRate() const;

        /*!
         * \warning STUB! Not implemented.
         * Returns the number of audio channels.
         */
        virtual int channels() const;

      private:
        Properties(const Properties &);
        Properties &operator=(const Properties &);

        void read(File *file);
        void readAVIHeader(File *file, const ByteVector &data, unsigned int avihBlockOffset);

        class PropertiesPrivate;
        PropertiesPrivate *d;
      };
    }
  }
}

#endif
