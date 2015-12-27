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
    _have = false;
    
    for (ezxml_t key = ezxml_child(data, "key"); key != NULL; key=key->next) {
        ezxml_t value = key->ordered;
        if (!std::strcmp(key->txt, "Track ID")) {
            _trackID = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Size")){
            _size = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Location")){
            _location = value->txt;
        }
    }
    
    return true;
}