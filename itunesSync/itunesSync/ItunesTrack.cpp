//
//  itunesTrack.cpp
//  itunessync
//
//  Created by Roman Semenov on 26.12.15.
//
//

#include "ItunesTrack.hpp"

bool ItunesTrack::init(ezxml_t data)
{
    _trackNumber = -1;
    _have = false;
    
    for (ezxml_t key = ezxml_child(data, "key"); key != NULL; key=key->next) {
        ezxml_t value = key->ordered;
        if (!std::strcmp(key->txt, "Track ID")) {
            _trackID = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Size")){
            _size = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Location")){
            _location = value->txt;
        }else if (!std::strcmp(key->txt, "Artist")){
            _artist = value->txt;
        }else if (!std::strcmp(key->txt, "Album")){
            _album = value->txt;
        }else if (!std::strcmp(key->txt, "Track Number")){
            _trackNumber = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Name")){
            _name = value->txt;
        }else if (!std::strcmp(key->txt, "Total Time")){
            _secTime = atoi(value->txt) / 1000;
        }
    }
    
    return generatePath();
}

bool ItunesTrack::generatePath()
{
    if (!_artist.empty()) {
        _genPath.append(_artist);
        _genPath.append("/");
    }

    if (!_album.empty()) {
        _genPath.append(_album);
        _genPath.append("/");
    }

    if (_trackNumber != -1)
    {
        static char buff[32];
        sprintf(buff, "%02d ",_trackNumber);
        _genPath.append(buff);
    }


    _genPath.append(_name);
    
    size_t pos = _location.find_last_of(".");
    if (pos ==std::string::npos)
        return false;
    
    _genPath.append(_location.substr(pos));
    return true;
}