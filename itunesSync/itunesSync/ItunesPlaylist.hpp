//
//  ItunesPlaylist.hpp
//  itunessync
//
//  Created by Roman Semenov on 26.12.15.
//
//

#ifndef ItunesPlaylist_hpp
#define ItunesPlaylist_hpp

#include <stdio.h>
#include "ItunesTrack.hpp"
#include "ezxml.h"

typedef std::vector<int> Tracks;

class ItunesPlaylist
{
public:
    bool init(ezxml_t data);
    
    const Tracks& getTracks() {return _tracks;}
    CC_SYNTHESIZE(std::string, _name, Name);
private:
    void parsePlaylistItems(ezxml_t array);
    
    Tracks _tracks;
};

typedef std::vector<ItunesPlaylist*> ItunesPlaylists;
#endif /* ItunesPlaylist_hpp */
