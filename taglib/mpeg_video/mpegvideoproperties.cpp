#include "mpegvideoproperties.h"
#include <iostream>

using namespace TagLib;

class MPEG_VIDEO::Properties::PropertiesPrivate {
public:
  unsigned int duration = 0;
};

MPEG_VIDEO::Properties::Properties(TagLib::AudioProperties::ReadStyle style, double time)
  : AudioProperties(style),
    d(new MPEG_VIDEO::Properties::PropertiesPrivate())
{
  if (time > 0)
    d->duration = time;
}

MPEG_VIDEO::Properties::~Properties()
{
  delete d;
}

int MPEG_VIDEO::Properties::length() const
{
  return d->duration;
}

int MPEG_VIDEO::Properties::bitrate() const
{
  return 0;
}

int MPEG_VIDEO::Properties::sampleRate() const
{
  return 0;
}

int MPEG_VIDEO::Properties::channels() const
{
  return 0;
}
