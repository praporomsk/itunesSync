//
//  ItunesParser.cpp
//  itunessync
//
//  Created by Roman Semenov on 26.12.15.
//
//

#include "ItunesParser.hpp"
#include "ItunesTrack.hpp"

const ItunesPlaylists& ItunesParser::getPlaylists(ItunesPlaylists& plist,const char* conteinName)
{
    plist.clear();
    for (ItunesPlaylists::const_iterator it = playlists.begin(); it != playlists.end(); ++it) {
        ItunesPlaylist* playlist = *it;
        size_t found = playlist->getName().find(conteinName);
        if (found!=std::string::npos)
            plist.push_back(playlist);
    }
    printf("find playlists %lu \n",plist.size());
    return plist;
}

template<class Iter, class T>
Iter binary_find(Iter begin, Iter end, T val)
{
    Iter i = std::lower_bound(begin, end, val);
    if (i != end && !(val < *i))
        return i; // found
    else
        return end; // not found
}



bool cmpFunc (ItunesTrack* track,int id) {
    
    return (track->getTrackID()< id);
}

ItunesTrack* ItunesParser::getTrack(int trackId)
{
    ItunesTracks::iterator it = std::lower_bound(tracks.begin(), tracks.end(), trackId, cmpFunc);
    if (it != tracks.end() && !(trackId < (*it)->getTrackID()))
        return *it; // found
    printf("Track id:%d not found \n", trackId);
    
    return NULL;
}

bool ItunesParser::parse()
{
    unsigned char * buffer = nullptr;
    size_t size;
    FILE *fp = fopen(iTunesDatabasePath.c_str(), "r");
    if (!fp) {
        printf("file not found:%s \n",iTunesDatabasePath.c_str());
        return false;
    }
    fseek(fp,0,SEEK_END);
    size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    buffer = (unsigned char*)malloc(size);
    size = fread(buffer,sizeof(unsigned char),size, fp);
    fclose(fp);
    
    parse(buffer, size);

    
    //free(buffer);
  
    
    
    return true;
}


void ItunesParser::parse(unsigned char *data, size_t size)
{
    ezxml_t xmlData = ezxml_create_with_buf((char*)data, size);
    ezxml_t dict = ezxml_child(xmlData, "dict");
    
    for (ezxml_t key = ezxml_child(dict, "key"); key != NULL; key=key->next) {
        ezxml_t value = key->ordered;
        if (!std::strcmp(key->txt, "Major Version")) {
            majorVersion = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Minor Version")){
            minorVersion = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Application Version")){
            applicationVersion = value->txt;
        }else if (!std::strcmp(key->txt, "Date")){
            date = value->txt;
        }else if (!std::strcmp(key->txt, "Features")){
            features = atoi(value->txt);
        }else if (!std::strcmp(key->txt, "Show Content Ratings")){
            //showContentRatings = ;
        }else if (!std::strcmp(key->txt, "Music Folder")){
            musicFolderURL = value->txt;
        }else if (!std::strcmp(key->txt, "Tracks")){
            parseTracks(value);
        }else if (!std::strcmp(key->txt, "Playlists")){
            parsePlaylists(value);
        }
        
    }
    
    //printf("Tracks =%lu \n",tracks.size());
    //printf("Playlists =%lu \n",playlists.size());
    ezxml_free(xmlData);
}

bool cmp(ItunesTrack* lhs,  ItunesTrack* rhs)
{
    return lhs->getTrackID() < rhs->getTrackID();
}

void ItunesParser::parseTracks(ezxml_t data)
{
    for (ezxml_t key = ezxml_child(data, "key"); key != NULL; key=key->next) {
        ezxml_t value = key->ordered;
        ItunesTrack* track = new ItunesTrack();
        track->init(value);
        tracks.push_back(track);
    }

    std::sort(tracks.begin(), tracks.end(), cmp);
}

void ItunesParser::parsePlaylists(ezxml_t data)
{
    for (ezxml_t dict = ezxml_child(data, "dict"); dict != NULL; dict=dict->next) {
        ezxml_t value = dict->ordered;
        ItunesPlaylist* playlist = new ItunesPlaylist();
        playlist->init(value);
        playlists.push_back(playlist);
    }
}