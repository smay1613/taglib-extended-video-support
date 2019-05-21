#ifndef TAGLIB_MPEGVIDEOFILE_H
#define TAGLIB_MPEGVIDEOFILE_H
#include "tfile.h"
#include "mpegvideoproperties.h"

namespace TagLib {

  //! An implementation of TagLib::File with MPEG (1 and 2) specific methods

  namespace MPEG_VIDEO {
    enum Marker {
      Corrupt = -1,

      Zero = 0,

      SystemSyncPacket = 0xBA,

      VideoSyncPacket = 0xB3,

      SystemPacket = 0xBB,

      PaddingPacket = 0xBE,

      AudioPacket = 0xC0,

      VideoPacket = 0xE0,

      EndOfStream = 0xB9
    };

    enum Version {
      Version1,
      Version2
    };

    //! An MPEG file class with some useful methods specific to MPEG

    /*!
     * This implements the generic TagLib::File API
     */

    class TAGLIB_EXPORT File : public TagLib::File
    {
    public:

      /*!
       * Constructs an MPEG file from \a file.  If \a readProperties is true the
       * file's audio properties will also be read.
       *
       * \note In the current implementation, \a propertiesStyle is ignored.
       */
      File(FileName file, bool readProperties = true,
           Properties::ReadStyle propertiesStyle = Properties::Average);

      /*!
       * Constructs an MPEG file from \a stream.  If \a readProperties is true the
       * file's audio properties will also be read.
       *
       * \note TagLib will *not* take ownership of the stream, the caller is
       * responsible for deleting it after the File object.
       *
       * \note In the current implementation, \a propertiesStyle is ignored.
       */
      File(IOStream *stream, bool readProperties = true,
           Properties::ReadStyle propertiesStyle = Properties::Average);

      /*!
       * Destroys this instance of the File.
       */
      virtual ~File();

      /*!
       * MPEG-1 and MPEG-2 doesn't support tags by default, so it is not implemented.
       * Always returns nullptr.
       */
      virtual Tag *tag() const;

      /*!
       * Returns the MPEG::Properties for this file.  If no properties
       * were read then this will return a null pointer.
       */
      virtual Properties *audioProperties() const;

      /*!
       * STUB! Not implemented.
       */
      virtual bool save();

    private:
      File(const File &);
      File &operator=(const File &);

      void read(bool readProperties);
      void readStart();
      void readEnd();

      void findMarker(long long &position, Marker marker);
      Marker findMarker(long long &position);
      void rfindMarker(long long &position, Marker marker);

      Marker getMarker(long long &position);
      void readSystemFile(long long position);
      void readSystemSyncPacket(long long &position);
      double readTimeStamp(long long position);
      void readVideoHeader(long long &position);

      class FilePrivate;
      FilePrivate *d;
    };
  }
}

#endif