#include "matroskatag.h"
#include "simpletag.h"
#include <tdebug.h>
#include <tstring.h>
#include <algorithm>

using namespace TagLib;

struct Matroska::Tag::TagPrivate {
  TagPrivate(std::vector<Tag*>* tags) : m_tags(tags) {}
  std::vector<Tag*>* m_tags;
  std::vector<UIDElement> m_elements;
  std::map<String, std::vector<SimpleTag> > m_simpleTags;
  TargetType m_targetType = DEFAULT;

  String album;
  String title;
  String genre;
  String artist;
  unsigned int index = 0;
};

Matroska::Tag::Tag(std::vector<Tag*>& tags, TargetType targetTypeValue, UIDElement id)
  : d (new TagPrivate (&tags))
{
  if (targetTypeValue != static_cast<TargetType>(0)) {
      d->m_targetType = targetTypeValue;
    }
  if (id != UIDElement()) {
      d->m_elements.clear();
      d->m_elements.push_back(id);
    }
}

Matroska::Tag::~Tag()
{
  delete d;
}

Matroska::TargetType Matroska::Tag::makeTargetType(unsigned int targetTypeValue)
{
  TargetType ret = static_cast<TargetType>(0);

  switch (targetTypeValue) {
    case 70: ret = COLLECTION; break;
    case 60: ret = SEASON; break;
    case 50: ret = MOVIE; break;
    case 40: ret = PART; break;
    case 30: ret = CHAPTER; break;
    case 20: ret = SCENE; break;
    case 10: ret = SHOT; break;
    default:
      debug("Invalid target type");
    }

  return ret;
}

String Matroska::Tag::title() const
{
  return d->title;
}

String Matroska::Tag::artist() const
{
  return d->artist;
}

String Matroska::Tag::album() const
{
  return d->album;
}

String Matroska::Tag::comment() const
{
  return "";
}

String Matroska::Tag::genre() const
{
  return d->genre;
}

unsigned int Matroska::Tag::year() const
{
  return 0;
}

unsigned int Matroska::Tag::track() const
{
  return d->index;
}

void Matroska::Tag::setTitle(const String &s)
{
  d->title = s;
}

void Matroska::Tag::setArtist(const String &s)
{
  d->artist = s;
}

void Matroska::Tag::setAlbum(const String &s)
{
  d->album = s;
}

void Matroska::Tag::setComment(const String &s)
{
  return;
}

void Matroska::Tag::setGenre(const String &s)
{
  d->genre = s;
}

void Matroska::Tag::setYear(unsigned int i)
{
  return;
}

void Matroska::Tag::setTrack(unsigned int i)
{
  d->index = i;
}

void Matroska::Tag::setTargetType(const Matroska::TargetType type)
{
  d->m_targetType = type;
}

void Matroska::Tag::setElements(const std::vector<Matroska::UIDElement> &elements)
{
  d->m_elements = elements;
}

std::map<String, std::vector<Matroska::SimpleTag> > &Matroska::Tag::simpleTags() const
{
  return d->m_simpleTags;
}

Matroska::TargetType Matroska::Tag::getTargetType() const
{
  return d->m_targetType;
}

void Matroska::Tag::read()
{
  readTagAlbum();
  readTagTitle();
  readGenre();
  readArtist();
  readTrack();
}

String Matroska::Tag::getString(const String &key, const String &subkey, bool recursive) const
{
  String ret;
  const std::vector<String>& list = get(key, subkey, recursive);
  if (list.size() > 0) {
      ret = list[0];
    }
  return ret;
}

void Matroska::Tag::readTagAlbum()
{
  if (d->m_targetType != COLLECTION) {
      return;
    }
  const String &ret = getString("TITLE");
  if (!ret.isEmpty()) {
      d->album = ret;
    }
}

void Matroska::Tag::readTagTitle()
{
  const String &ret = getString("TITLE");
  if (!ret.isEmpty()) {
      d->title = ret;
    }
}

void Matroska::Tag::readGenre()
{
  const String &ret = getString("GENRE");
  if (!ret.isEmpty()) {
      d->genre = ret;
    }
}

void Matroska::Tag::readArtist()
{
  const String &ret = getString("ARTIST");
  if (!ret.isEmpty()) {
      d->artist = ret;
    }
}

void Matroska::Tag::readTrack()
{
  const String &ret = getString("PART_NUMBER");
  if (!ret.isEmpty()) {
      d->index = ret.toInt();
    }
}

Matroska::Tag *Matroska::Tag::get(Matroska::TargetType targetType) const
{
  Tag* ret = NULL;

  // Find first match of the given targetValue
  for (size_t i = 0; i < d->m_tags->size(); i++) {
      if (targetType == (d->m_tags->at(i))->d->m_targetType) {
          ret = d->m_tags->at(i);
        }
    }

  return ret;
}

std::vector<String> Matroska::Tag::get(const String &key, const String &subkey, bool recursive) const
{
  std::vector<String> ret;
  std::vector<SimpleTag> &mtags = d->m_simpleTags[key];

  if (!mtags.empty() && recursive) {
      const Tag* tag = this;
      while ((tag = tag->getParent()) && recursive) {
          mtags = tag->d->m_simpleTags[key];
        }
    }

  ret.resize(mtags.size());

  if (!subkey.isEmpty() && !mtags.empty()) {

      // Handle Nested SimpleTags
      for (size_t i = 0; i < mtags.size(); i++) {
          String str;

          SimpleTag& stag = mtags[i];
          if (!stag.simpleTags().empty()) {
              std::vector<Matroska::SimpleTag>& list = stag.simpleTags()[subkey];
              if (list.size() == 1) {
                  str = list[0];
                } else {
                  for (std::vector<SimpleTag>::const_iterator p = list.begin();
                       p != list.end(); ++p) {
                      str += p->value();
                      if (p != list.end() - 1)
                        str += "; ";
                    }
                }
            }

          ret[i] = str;
        }
    } else {
      for (size_t i = 0; i < mtags.size(); i++) {
          ret[i] = mtags[i].value();
        }
    }
  return ret;
}

Matroska::Tag *Matroska::Tag::getParent() const
{
  if (d->m_tags->empty()) {
      return NULL;
    }

  std::vector<Tag*>::iterator i = std::find(d->m_tags->begin(), d->m_tags->end(), this);
  while (i != d->m_tags->end() && i != d->m_tags->begin()) {
      --i;
      bool match = true;

      if (!*i) {
          continue;
        }
      if (d->m_elements.empty()) {
          match = false;
          break;
        }

      // All UID in the reference should be found also in the parent
      for (std::vector<UIDElement>::iterator refUid = d->m_elements.begin();
           refUid != d->m_elements.end();
           ++refUid)
        {
          bool submatch = false;
          for (std::vector<UIDElement>::iterator uid = (*i)->d->m_elements.begin();
               uid != (*i)->d->m_elements.end();
               ++uid)
            {
              if (*uid == *refUid) {
                  submatch = true;
                  break;
                }
            }
          match = match && submatch;
        }

      if (match) {
          return *i;
        }
    }

  return NULL;
}

