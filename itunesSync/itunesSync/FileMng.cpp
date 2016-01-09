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
    
    size_t pos = destPath.find_last_of("/");
    createFolders(destPath.substr(0, pos));
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
            return !filePath.compare(track->getGenPath());
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
    printf("\n files synced\n");
}