#ifndef TAGLIB_MATROSKATAG_H
#define TAGLIB_MATROSKATAG_H
#include <vector>
#include <map>
#include <tstring.h>
#include "tag.h"
#include "matroskatypes.h"

namespace TagLib {
  namespace Matroska {
    class SimpleTag;
    class TAGLIB_EXPORT Tag : public TagLib::Tag {
    public:
      Tag(std::vector<Tag *> &tags,
          TargetType targetTypeValue = static_cast<TargetType>(0),
          UIDElement id = UIDElement());
      virtual ~Tag();
      static TargetType makeTargetType(unsigned int targetTypeValue);

     /*!
     * Returns the track name; if no track name is present in the tag
     * String::null will be returned.
     */
      virtual String title() const;

     /*!
     * Returns the artist name; if no artist name is present in the tag
     * String::null will be returned.
     */
      virtual String artist() const;

     /*!
     * Returns the album name; if no album name is present in the tag
     * String::null will be returned.
     */
      virtual String album() const;

     /*!
     * STUB! Not implemented.
     */
      virtual String comment() const;

     /*!
     * Returns the genre name; if no genre is present in the tag String::null
     * will be returned.
     */
      virtual String genre() const;

     /*!
     * STUB! Not implemented.
     */
      virtual unsigned int year() const;

     /*!
     * Returns the track number; if there is no track number set, this will
     * return 0.
     */
      virtual unsigned int track() const;

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setTitle(const String &s);

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setArtist(const String &s);

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setAlbum(const String &s);

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setComment(const String &s);

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setGenre(const String &s);

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setYear(unsigned int i);

     /*!
     * STUB! Not implemented. Writes only local data.
     */
      virtual void setTrack(unsigned int i);

      void setTargetType(const TargetType);
      void setElements(const std::vector<UIDElement>& elements);
      std::map<String, std::vector<SimpleTag>>& simpleTags() const;

      Tag* get(TargetType targetType) const;
      TargetType getTargetType() const;
      void read();
    private:
      String getString(const String& key, const String& subkey = "", bool recursive = true) const;
      std::vector<String> get(const String& key, const String& subkey = "", bool recursive = true) const;
      Tag* getParent() const;

      void readArtist();
      void readTagAlbum();
      void readTagTitle();
      void readGenre();
      void readTrack();

      struct TagPrivate;
      TagPrivate *d;
    };

  }
}


#endif // MATROSKATAG_H