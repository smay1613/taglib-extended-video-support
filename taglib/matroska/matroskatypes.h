#ifndef TAGLIB_MATROSKATYPES_H
#define TAGLIB_MATROSKATYPES_H
#include <utility>

namespace TagLib {
  namespace Matroska {

    enum class EBMLID
    {
      EBMLHeader = 0x1A45DFA3,

      EBMLVersion = 0x4286,

      EBMLReadVersion = 0x42F7,

      EBMLMaxIDLength = 0x42F2,

      EBMLMaxSizeLength = 0x42F3,

      EBMLDocType = 0x4282,

      EBMLDocTypeVersion = 0x4287,

      EBMLDocTypeReadVersion = 0x4285,

      EBMLVoid = 0xEC
    };

    enum class MatroskaID : unsigned int
    {
      Segment = 0x18538067,

      SegmentInfo = 0x1549A966,

      Tracks = 0x1654AE6B,

      Cues = 0x1C53BB6B,

      Tags = 0x1254C367,

      SeekHead = 0x114D9B74,

      Cluster = 0x1F43B675,

      Attachments = 0x1941A469,

      Chapters = 0x1043A770,

      /* IDs in the SegmentInfo master */

      TimeCodeScale = 0x2AD7B1,

      Duration = 0x4489,

      WrittingApp = 0x5741,

      MuxingApp = 0x4D80,

      DateUTC = 0x4461,

      SegmentUID = 0x73A4,

      SegmentFileName = 0x7384,

      PrevUID = 0x3CB923,

      PrevFileName = 0x3C83AB,

      NexUID = 0x3EB923,

      NexFileName = 0x3E83BB,

      Title = 0x7BA9,

      SegmentFamily = 0x4444,

      ChapterTranslate = 0x6924,

      /* ID in the Tracks master */

      TrackEntry = 0xAE,

      /* IDs in the TrackEntry master */

      TrackNumber = 0xD7,

      TrackUID = 0x73C5,

      TrackType = 0x83,

      TrackAudio = 0xE1,

      TrackVideo = 0xE0,

      Void = 0xEC,

      CRC32 = 0xBF,

      ContentEncodings = 0x6D80,

      CodecID = 0x86,

      CodecPrivate = 0x63A2,

      CodecName = 0x258688,

      TrackName = 0x536E,

      TrackLanguage = 0x22B59C,

      TrackFlagEnabled = 0xB9,

      TrackFlagDefault = 0x88,

      TrackFlagForced = 0x55AA,

      TrackFlagLacing = 0x9C,

      TrackMinCache = 0x6DE7,

      TrackMaxCache = 0x6DF8,

      TrackDefaultDuration = 0x23E383,

      TrackTimeCodeScale = 0x23314F,

      MaxBlockAdditionID = 0x55EE,

      TrackAttachmentLink = 0x7446,

      TrackOverlay = 0x6FAB,

      TrackTranslate = 0x6624,

      TrackOffset = 0x537F,

      CodecSettings = 0x3A9697,

      CodecInfoUrl = 0x3B4040,

      CodecDownloadUrl = 0x26B240,

      CodecDecodeAll = 0xAA,

      /* IDs in the TrackVideo master */

      VideoFrameRate = 0x2383E3,

      VideoDisplayWidth = 0x54B0,

      VideoDisplayHeight = 0x54BA,

      VideoDisplayUnit = 0x54B2,

      VideoPixelWidth = 0xB0,

      VideoPixelHeight = 0xBA,

      VideoPixelCropBottom = 0x54AA,

      VideoPixelCropTop = 0x54BB,

      VideoPixelCropLeft = 0x54CC,

      VideoPixelCropRight = 0x54DD,

      VideoFlagInterlaced = 0x9A,

      VideoStereoMode = 0x53B8,

      VideoAspectRatioType = 0x54B3,

      VideoColourSpace = 0x2EB524,

      VideoGammaValue = 0x2FB523,

      /* in the Matroska Seek Head master */

      Seek = 0x4DBB,

      SeekID = 0x53AB,

      SeekPosition = 0x53AC,

      /* IDs in the TrackAudio master */

      AudioSamplingFreq = 0xB5,

      AudioBitDepth = 0x6264,

      AudioChannels = 0x9F,

      AudioChannelsPositions = 0x7D7B,

      AudioOutputSamplingFreq = 0x78B5,

      /* IDs in the Tags master */

      Tag = 0x7373,

      /* in the Tag master */

      SimpleTag = 0x67C8,

      Targets = 0x63C0,

      /* in the SimpleTag master */

      TagName = 0x45A3,

      TagString = 0x4487,

      TagLanguage = 0x447A,

      TagDefault = 0x4484,

      TagBinary = 0x4485,

      /* in the Targets master */

      TargetTypeValue = 0x68CA,

      TargetType = 0x63CA,

      TagTrackUID = 0x63C5,

      TagEditionUID = 0x63C9,

      TagChapterUID = 0x63C4,

      TagAttachmentUID = 0x63C6,

      /* in the Attachments master */

      AttachedFile = 0x61A7,

      FileDescription = 0x467E,

      FileName = 0x466E,

      FileMimeType = 0x4660,

      FileData = 0x465C,

      FileUID = 0x46AE
    };

    enum class TargetType
    {
      COLLECTION = 70,

      VOLUME = 60,

      EDITION = 61,

      ISSUE = 62,

      OPUS = 63,

      SEASON = 64,

      SEQUEL = 65,

      ALBUM = 50,

      OPERA = 51,

      CONCERT = 52,

      MOVIE = 53,

      EPISODE = 54,

      DEFAULT = 55,

      PART = 40,

      SESSION = 41,

      TRACK = 30,

      SONG = 31,

      CHAPTER = 32,

      SUBTRACK = 20,

      MOVEMENT = 21,

      SCENE = 22,

      SHOT = 10
    };

    using UIDElement = std::pair<MatroskaID, unsigned long>;
  }
}

#endif // MATROSKATYPES_H
