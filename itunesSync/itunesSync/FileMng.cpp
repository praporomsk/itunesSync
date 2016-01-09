//
//  FileMng.cpp
//  itunesSync
//
//  Created by Roman Semenov on 27.12.15.
//  Copyright © 2015 Roman. All rights reserved.
//

#include "FileMng.hpp"
#include "Platform.h"

const std::string itFolder = "iTunes/iTunes Media/Music";

bool FileMng::init(ItunesParser* p)
{
    parser = p;
    return true;
}

void FileMng::saveFile(const std::string& file)
{
    char buf[BUFSIZ];
    size_t size;
    
    std::string path(file);
    urlToPath(path);
    
    std::string destPath(_SDFolder);
    size_t pos = path.find(itFolder);
    destPath.append(path.substr(pos + itFolder.length()));

    pos = destPath.find_last_of("/");
    
    createFolders(destPath.substr(0, pos));
    copyFile(path, destPath);
//    FILE* source = fopen(path.c_str(), "rb");
//    FILE* dest = fopen(destPath.c_str(), "rb+");
//
//    if(dest == NULL) //if file does not exist, create it
//    {
//        dest = fopen(destPath.c_str(), "wb");
//    }
//    
//    while ((size = fread(buf, 1, BUFSIZ, source))) {
//        fwrite(buf, 1, size, dest);
//    }
//    
//    fclose(source);
//    fclose(dest);
}

void FileMng::scan()
{
    printf("FileMng::scan() \n");
    ItunesPlaylists playlists;
    parser->getPlaylists(playlists, _plKey.c_str());
    std::for_each(playlists.begin(), playlists.end(), [this] (ItunesPlaylist* p) {
        std::for_each(p->getTracks().begin(), p->getTracks().end(), [this] (int id) {
            ItunesTrack* track = parser->getTrack(id);
            if (track) {
                _tracks.insert(track);
            }
            
        });
    });
    
   printf("1FileMng::scan() \n");
    std::vector<std::string> flashFiles;
    scanPath(flashFiles,_SDFolder);
    _filesToDelete.clear();
    printf("flashFiles size =%lu\n",flashFiles.size());
    
    for (Files::iterator it = flashFiles.begin(); it != flashFiles.end(); ++it) {
        const std::string flFile = *it;
        
        auto iter = std::find_if(_tracks.begin(), _tracks.end(), [this,flFile](ItunesTrack* track){
            std::string path(track->getLocation());
            urlToPath(path);
            size_t pos = path.find(itFolder);
            path = path.substr(pos + itFolder.length());
            std::string filePath (flFile.substr(_SDFolder.length()));
            return !filePath.compare(path);
        });

        if (iter != _tracks.end()) {
            ItunesTrack* track = *iter;
            printf("Have File =%s \n",track->getLocation().c_str());
            track->setHave(true);
        }else{
            //printf("_filesToDelete =%s \n",(*it).c_str());
            _filesToDelete.push_back(*it);
        }
    }
    printf("100\n");
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
    
    float files = _fileToCopyCount;
    
    std::for_each(_tracks.begin(), _tracks.end(), [this,files](ItunesTrack* track){
        if(!track->getHave()){
            //copyFile(track->getLocation(),_SDFolder);
            saveFile(track->getLocation());
            printf("\r copy %d / %.0f progress %.1f%%", (int)files - _fileToCopyCount, files, ((files - (float)_fileToCopyCount)/files) * 100.0f);
            --_fileToCopyCount;
           
        }
    });
    printf("\r copy %d / %.0f progress %.1f%%", (int)files - _fileToCopyCount, files, ((files - (float)_fileToCopyCount)/files) * 100.0f);
    printf("\n files synced\n");
}