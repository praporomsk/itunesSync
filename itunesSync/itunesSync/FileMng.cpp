//
//  FileMng.cpp
//  itunesSync
//
//  Created by Roman Semenov on 27.12.15.
//  Copyright © 2015 Roman. All rights reserved.
//

#include "FileMng.hpp"
#include "Platform.h"
#include <iostream>

bool FileMng::init(ItunesParser* p)
{
    parser = p;
    return true;
}

void FileMng::saveTrack(ItunesTrack* t)
{
    std::string path(t->getLocation());
    urlToPath(path);
    
    std::string destPath(_SDFolder);
    destPath.append(t->getGenPath());

    std::string folder = destPath.substr(0, destPath.length() - t->getName().length() - t->getExtention().length() - 4);
    createFolders(folder);
    copyFile(path, destPath);
}

void FileMng::scan()
{
    playlists.clear();
    parser->getPlaylists(playlists, _plKey.c_str());
    std::for_each(playlists.begin(), playlists.end(), [this] (ItunesPlaylist* p) {
        std::for_each(p->getTracks().begin(), p->getTracks().end(), [this] (int id) {
            ItunesTrack* track = parser->getTrack(id);
            if (track) {
                _tracks.insert(track);
            }
            
        });
    });
    
    std::vector<std::string> flashFiles;
    std::string folder(_SDFolder.substr(0,_SDFolder.length()-1));

    scanPath(flashFiles,folder);
    _filesToDelete.clear();

    for (Files::iterator it = flashFiles.begin(); it != flashFiles.end(); ++it) {
        const std::string flFile = *it;
        
        auto iter = std::find_if(_tracks.begin(), _tracks.end(), [this,flFile](ItunesTrack* track){
            std::string filePath (flFile.substr(_SDFolder.length()));
            std::string genPath = track->getGenPath();
            bool isEqualInsensitive = isEqualString(filePath, genPath, true);
            bool isEqualSensitive = isEqualString(filePath, genPath, false);
            
            if (isEqualInsensitive != isEqualSensitive) {
                printf("names with different case: \n%s\n%s\n",filePath.c_str(),genPath.c_str());
            }
            return isEqualInsensitive;
        });

        if (iter != _tracks.end()) {
            ItunesTrack* track = *iter;
            track->setHave(true);
        }else{
            _filesToDelete.push_back(*it);
        }
    }

    _fileToCopyCount = 0;
    size_t size = 0;
    for (std::set<ItunesTrack*>::iterator it = _tracks.begin(); it != _tracks.end(); ++it) {
        ItunesTrack* track = *it;
        
        if (!track->getHave()) {
            _fileToCopyCount++;
            size += track->getSize();
        }
    }
    printf("%lu files to delete, %d files[%0.1fmb] will copy, do you want sync y/n?\n",_filesToDelete.size(), _fileToCopyCount, (float)size/1000000.0f);
}

void FileMng::sync()
{
    std::for_each(_filesToDelete.begin(), _filesToDelete.end(), [](const std::string& path){
        if(remove(path.c_str()) != 0 )
            printf("Error deleting file%s \n",path.c_str());
    });
    
    printf("files deleted\n");
    
    deleteEmptyFolders(_SDFolder);
    
    float files = _fileToCopyCount;
    
    std::for_each(_tracks.begin(), _tracks.end(), [this,files](ItunesTrack* track){
        if(!track->getHave()){
            saveTrack(track);
            --_fileToCopyCount;

            std::cout << "\r" << "copy " << ((int)files - _fileToCopyCount) << " / " << files
            << " progress:" << ((files - (float)_fileToCopyCount)/files) * 100.0f << "%"
            << std::flush;

        }
    });
    
    createPlaylists();
    printf("\n files synced\n");
}

void FileMng::createPlaylists()
{
    std::for_each(playlists.begin(), playlists.end(), [this](ItunesPlaylist* pList) {
        createPlaylist(pList);
    });
}

#include <fstream>

void FileMng::createPlaylist(ItunesPlaylist* pList)
{
    static char buff[256];
    
    snprintf(buff, 256, "%s/%s.m3u",_SDFolder.c_str(),pList->getName().c_str());
    
    std::ofstream file;
    file.open (buff);
    file << "#EXTM3U\n";
    
    
    
    const Tracks& tracks = pList->getTracks();
    std::for_each(tracks.begin(), tracks.end(), [&] (int id) {
        ItunesTrack* track = parser->getTrack(id);
        snprintf(buff, 256, "#EXTINF:%d,%s - %s\n",track->getSecTime(),track->getName().c_str(),track->getAlbum().c_str());
        file << buff;
        snprintf(buff, 256, "TF1:%s\n",track->getGenPath().c_str());
    });
    
    file.close();
}
