#ifndef TAGLIB_MPEGVIDEOPROPERTIES_H
#define TAGLIB_MPEGVIDEOPROPERTIES_H
#include "audioproperties.h"

namespace TagLib {
  namespace MPEG_VIDEO {
    class File;
    class TAGLIB_EXPORT Properties : public TagLib::AudioProperties
    {
    public:
      /*!
       * Create an instance of MPEG::Properties with the data read from the
       * MPEG::File \a file.
       */
      Properties(ReadStyle style = Average, double time = 0);

      /*!
       * Destroys this MPEG Properties instance.
       */
      virtual ~Properties();

      /*!
       * Returns the length of the file in seconds.  The length is rounded down to
       * the nearest whole second.
       *
       * \note This method is just an alias of lengthInSeconds().
       */
      virtual int length() const;

      /*!
       * STUB! Not implemented!
       */
      virtual int bitrate() const;

      /*!
       * STUB! Not implemented!
       */
      virtual int sampleRate() const;

      /*!
       * STUB! Not implemented!
       */
      virtual int channels() const;
    private:
      Properties(const Properties &);
      Properties &operator=(const Properties &);

      void read(File *file);

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }

}

#endif // MPEGVIDEOPROPERTIES_H