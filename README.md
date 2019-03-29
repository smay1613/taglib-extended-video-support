# TagLib

[![Build Status](https://travis-ci.org/taglib/taglib.svg?branch=master)](https://travis-ci.org/taglib/taglib)

### TagLib Audio Metadata Library

http://taglib.org/

TagLib is a library for reading and editing the metadata of several
popular audio formats. Currently it supports both ID3v1 and [ID3v2][]
for MP3 files, [Ogg Vorbis][] comments and ID3 tags 
in [FLAC][], MPC, Speex, WavPack, TrueAudio, WAV, AIFF, MP4, APE,
DSF, DFF, and ASF files.

TagLib is distributed under the [GNU Lesser General Public License][]
(LGPL) and [Mozilla Public License][] (MPL). Essentially that means that
it may be used in proprietary applications, but if changes are made to
TagLib they must be contributed back to the project. Please review the
licenses if you are considering using TagLib in your project.

  [ID3v2]: http://www.id3.org 
  [Ogg Vorbis]: http://vorbis.com/
  [FLAC]: https://xiph.org/flac/
  [GNU Lesser General Public License]: http://www.gnu.org/licenses/lgpl.html
  [Mozilla Public License]: http://www.mozilla.org/MPL/MPL-1.1.html


### Clarion info

TagLib was modified for handling video properties and tags.
AV processing component uses taglib for metadata extraction, but it works only for audio,
so the final solution was to modify the taglib sources
(and not use slow gstreamer or another metadata extraction library for consistency).
Also, this repository follows the taglib codestyle, but it doesn't implement all features, only that are
needed by Clarion components.
Implemented formats: mp4 video, avi, mkv, mpeg-1, mpeg-2, wmv
