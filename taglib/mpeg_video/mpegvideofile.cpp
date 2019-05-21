#include "mpegvideofile.h"
#include "tdebug.h"
#include "id3v1/id3v1tag.h"

using namespace TagLib;

class MPEG_VIDEO::File::FilePrivate
{
public:
  FilePrivate() :
    properties (0),
    tag (0)
  {
  }
  ~FilePrivate() {
    delete properties;
    delete tag;
  }

  const static ByteVector markerStart;
  double startTime = -1.0;
  double endTime = -1.0;
  Version version;
  int averageBitrate = 0;

  Properties* properties;
  ID3v1::Tag* tag;
};

const ByteVector MPEG_VIDEO::File::FilePrivate::markerStart = ByteVector::fromUInt(0x00000001).mid(1, 3);

TagLib::MPEG_VIDEO::File::File(TagLib::FileName file, bool readProperties, TagLib::AudioProperties::ReadStyle propertiesStyle)
  : TagLib::File(file),
    d(new FilePrivate())
{
  if (isOpen())
    read(readProperties);
}

MPEG_VIDEO::File::File(IOStream *stream, bool readProperties, AudioProperties::ReadStyle propertiesStyle)
  : TagLib::File(stream),
    d(new FilePrivate())
{
  if (isOpen())
    read(readProperties);
}

MPEG_VIDEO::File::~File()
{
  delete d;
}

Tag *MPEG_VIDEO::File::tag() const
{
  return d->tag;
}

MPEG_VIDEO::Properties *MPEG_VIDEO::File::audioProperties() const
{
  return d->properties;
}

bool MPEG_VIDEO::File::save()
{
  return false;
}

void MPEG_VIDEO::File::read(bool readProperties)
{
  readStart();
  readEnd();

  if (d->startTime < 0) {
    d->startTime = 0;
  }
  d->properties = new Properties(Properties::Average, d->endTime - d->startTime);
  if (isValid()) {
    d->tag = new ID3v1::Tag();
  }
}

void MPEG_VIDEO::File::readStart()
{
  long long position = 0;
  findMarker(position, VideoSyncPacket);
  if (position >= 0) {
    readVideoHeader(position);
  }
  long long start = 0;
  findMarker(start, SystemSyncPacket);
  if (start >= 0) {
    readSystemFile(start);
  }
}

void MPEG_VIDEO::File::readEnd()
{
  long long end = length();

  rfindMarker(end, SystemSyncPacket);
  if (end >= 0) {
    d->endTime = readTimeStamp (end + 4);
  }
}

void MPEG_VIDEO::File::findMarker(long long &position, MPEG_VIDEO::Marker marker)
{
  ByteVector packet (MPEG_VIDEO::File::FilePrivate::markerStart);
  packet.append(static_cast<char>(marker));

  position = find(packet, position);

  if (position < 0) {
      debug("Marker not found");
  }
}

MPEG_VIDEO::Marker MPEG_VIDEO::File::findMarker(long long &position)
{
  position = find(MPEG_VIDEO::File::FilePrivate::markerStart, position);
  if (position < 0) {
      debug("Marker not found");
  }
  return getMarker(position);
}

void MPEG_VIDEO::File::rfindMarker(long long &position, MPEG_VIDEO::Marker marker)
{
  ByteVector packet (MPEG_VIDEO::File::FilePrivate::markerStart);
  packet.append(static_cast<char>(marker));
  position = rfind (packet, position);

  if (position < 0) {
    debug("Marker not found");
  }
}

MPEG_VIDEO::Marker MPEG_VIDEO::File::getMarker(long long &position)
{
  seek(position);

  ByteVector identifier = readBlock(4);
  if (identifier.size() == 4 && identifier.startsWith(MPEG_VIDEO::File::FilePrivate::markerStart)) {
    return static_cast<Marker>(identifier[3]);
  } else {
    String message ("Invalid marker at position");
    debug (message);
  }
  return Corrupt;
}

void MPEG_VIDEO::File::readSystemFile(long long position)
{
  int sanityLimit = 100;

  for (int i = 0; (i < sanityLimit) && (d->startTime < 0.0); i++) {
      Marker marker = findMarker(position);

      switch (static_cast<unsigned char>(marker)) {
        case SystemSyncPacket:
          readSystemSyncPacket(position);
          break;
        case SystemPacket:
        case PaddingPacket:
          seek (position + 4);
          position += readBlock (2).toUShort() + 6;
          break;
        case VideoPacket:
        case AudioPacket:
          seek (position + 4);
          position += readBlock (2).toUShort ();
          break;
        case EndOfStream:
          return;
        default:
          position += 4;
          break;
        }
    }
}

void MPEG_VIDEO::File::readSystemSyncPacket(long long &position)
{
  int packetSize = 0;
  seek (position + 4);
  char versionInfo = readBlock(1)[0];

  if ((versionInfo & 0xF0) == 0x20) {
      d->version = Version1;
      packetSize = 12;
  } else if ((versionInfo & 0xC0) == 0x40) {
      d->version = Version2;
      seek (position + 13);
      packetSize = 14 + (readBlock (1)[0] & 0x07);
  } else {
      debug ("Unknown MPEG version.");
      return;
  }

  if (d->startTime < 0.0) {
    d->startTime = readTimeStamp(position + 4);
  }

  position += packetSize;
}

double MPEG_VIDEO::File::readTimeStamp(long long position)
{
  double high;
  uint low;

  seek(position);

  if (d->version == Version1) {
      ByteVector data = readBlock (5);
      high = (data[0] >> 3) & 0x01;

      low = (static_cast<uint>((static_cast<uchar>(data[0]) >> 1) & 0x03) << 30) |
          static_cast<uint>(static_cast<uchar>(data[1]) << 22) |
          static_cast<uint>((static_cast<uchar>(data[2]) >> 1) << 15) |
          static_cast<uint>(static_cast<uchar>(data[3]) << 7) |
          static_cast<uint>(static_cast<uchar>(data[4]) >> 1);
  } else {
      ByteVector data = readBlock (6);
      high = (data[0] & 0x20) >> 5;

      low = (static_cast<uint>((static_cast<uchar>(data[0]) & 0x18) >> 3) << 30) |
          static_cast<uint>((static_cast<uchar>(data[0]) & 0x03) << 28) |
          static_cast<uint>(static_cast<uchar>(data[1]) << 20) |
          static_cast<uint>((static_cast<uchar>(data[2]) & 0xF8) << 12) |
          static_cast<uint>((static_cast<uchar>(data[2]) & 0x03) << 13) |
          static_cast<uint>(static_cast<uchar>(data[3]) << 5) |
          static_cast<uint>(static_cast<uchar>(data[4]) >> 3);
  }

  return (((high * 0x10000) * 0x10000) + low) / 90000.0;
}

void MPEG_VIDEO::File::readVideoHeader(long long &position)
{
  int dataLength = readBlock(2).toUShort();

  long long dataOffset = position + 4;
  seek(dataOffset);
  ByteVector data = readBlock(7);
  if (data.size() < 7) {
      debug("Insufficient data in header");
      setValid(false);
  }
  d->averageBitrate = (static_cast<int>(data.mid(4, 3).toUInt() >> 6) & 0x3FFFF) * 400;
  d->endTime = length() * 8 / (d->averageBitrate + 0.5);
  position += dataLength;
}
