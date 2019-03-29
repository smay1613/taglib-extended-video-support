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

#ifndef TAGLIB_AVIFILE_H
#define TAGLIB_AVIFILE_H

#include "rifffile.h"
#include "infotag.h"
#include "aviproperties.h"

namespace TagLib {

  namespace RIFF {

    //! An implementation of AVI metadata

    /*!
     * This is implementation of AVI metadata.
     */

    namespace AVI {

      //! An implementation of TagLib::File with AVI specific methods

      /*!
       * This implements and provides an interface for AVI files to the
       * TagLib::Tag and TagLib::AudioProperties interfaces by way of implementing
       * the abstract TagLib::File API as well as providing some additional
       * information specific to AVI files.
       */

      class TAGLIB_EXPORT File : public TagLib::RIFF::File
      {
      public:
        /*!
         * Constructs a AVI file from \a file.  If \a readProperties is true the
         * file's audio properties will also be read.
         *
         * \note In the current implementation, \a propertiesStyle is ignored.
         */
        File(FileName file, bool readProperties = true,
             Properties::ReadStyle propertiesStyle = Properties::Average);

        File(IOStream* stream, bool readProperties = true,
             Properties::ReadStyle propertiesStyle = Properties::Average);
        /*!
         * Destroys this instance of the File.
         */
        virtual ~File();

        /*!
         * Returns the RIFF INFO Tag for this file.
         *
         * \note This always returns a valid pointer regardless of whether or not
         * the file on disk has a RIFF INFO tag.  Use hasInfoTag() to check if the
         * file on disk actually has a RIFF INFO tag.
         *
         * \see hasInfoTag()
         */
        Info::Tag *tag() const;

        /*!
         * \warning STUB! Not implemented.
         */
        bool save();

        /*!
         * Returns the AVI::Properties for this file.  If no audio properties
         * were read then this will return a null pointer.
         */
        virtual Properties *audioProperties() const;

      private:
        File(const File &);
        File &operator=(const File &);

        void read(bool readProperties);

        friend class Properties;

        class FilePrivate;
        FilePrivate *d;
      };
    }
  }
}

#endif
