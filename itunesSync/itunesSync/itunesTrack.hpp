//
//  itunesTrack.hpp
//  itunessync
//
//  Created by Roman Semenov on 26.12.15.
//
//

#ifndef ItunesTrack_hpp
#define ItunesTrack_hpp

#include <stdio.h>
#include "macros.h"
#include <string>
#include <vector>
#include "ezxml.h"

class ItunesTrack
{
public:
    bool init(ezxml_t data);
    
    CC_SYNTHESIZE(bool, _have, Have);
    CC_SYNTHESIZE(int, _trackID, TrackID);
    CC_SYNTHESIZE(size_t, _size, Size);
    CC_SYNTHESIZE(std::string, _location, Location);
    CC_SYNTHESIZE(std::string, _artist, Artist);
    CC_SYNTHESIZE(std::string, _album, Album);
    CC_SYNTHESIZE(int, _trackNumber, TrackNumber);
    CC_SYNTHESIZE(std::string, _name, Name);
    CC_SYNTHESIZE(int, _secTime, SecTime);
    CC_SYNTHESIZE(bool, _compilation, Compilation);

    CC_SYNTHESIZE(std::string, _genPath, GenPath);
    CC_SYNTHESIZE(std::string, _extention, Extention);
private:
    bool generatePath();
};

typedef std::vector<ItunesTrack*> ItunesTracks;
#endif /* itunesTrack_hpp */
