//
//  ItunesParser.hpp
//  itunessync
//
//  Created by Roman Semenov on 26.12.15.
//
//

#ifndef ItunesParser_hpp
#define ItunesParser_hpp

#include <stdio.h>
#include <vector>
#include "ItunesTrack.hpp"
#include "macros.h"
#include "ezxml.h"
#include "ItunesPlaylist.hpp"

class ItunesParser
{
public:
    bool parse();
    
    CC_SYNTHESIZE(std::string, iTunesDatabasePath, ITunesDatabasePath);
    
    CC_SYNTHESIZE(int, majorVersion, MajorVersion);
    CC_SYNTHESIZE(int, minorVersion, MinorVersion);
    CC_SYNTHESIZE(std::string, applicationVersion, ApplicationVersion);
    CC_SYNTHESIZE(std::string, date, Date);
    CC_SYNTHESIZE(int, features, Features);
    CC_SYNTHESIZE(bool, showContentRatings, ShowContentRatings);
    CC_SYNTHESIZE(std::string, musicFolderURL, MusicFolderURL);
    CC_SYNTHESIZE(std::string, libraryPersistentID, LibraryPersistentID);
    CC_SYNTHESIZE(ItunesTracks, tracks, Tracks);
    CC_SYNTHESIZE(ItunesPlaylists, playlists, Playlists);
    
    const ItunesPlaylists& getPlaylists(ItunesPlaylists& plist,const char* conteinName);
    ItunesTrack* getTrack(int trackId);
private:
    void parse(unsigned char *data, size_t size);
    void parseTracks(ezxml_t data);
    void parsePlaylists(ezxml_t data);
};
#endif /* ItunesParser_hpp */
