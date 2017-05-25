//
//  ItunesPlaylist.cpp
//  itunessync
//
//  Created by Roman Semenov on 26.12.15.
//
//

#include "ItunesPlaylist.hpp"

ItunesPlaylist::~ItunesPlaylist()
{
    
}

bool ItunesPlaylist::init(ezxml_t data)
{
    for (ezxml_t key = ezxml_child(data, "key"); key != NULL; key=key->next) {
        ezxml_t value = key->ordered;
        if (!std::strcmp(key->txt, "Name")) {
            _name = value->txt;
        }else if (!std::strcmp(key->txt, "Playlist Items")){
            parsePlaylistItems(value);
        }
    }
    
    return true;
}

void ItunesPlaylist::parsePlaylistItems(ezxml_t array)
{
    for (ezxml_t dict = ezxml_child(array, "dict"); dict != NULL; dict=dict->next)
        _tracks.push_back(atoi(ezxml_child(dict, "integer")->txt));
}
