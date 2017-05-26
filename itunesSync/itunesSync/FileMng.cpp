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
#include <fstream>
#include "copyFile.h"
#include "dirent.h"

bool FileMng::init(ItunesParser* p)
{
    parser = p;
    return true;
}

void FileMng::saveTrack(ItunesTrack* t, bool isEmpty)
{
    std::string path(t->getLocation());
    urlToPath(path);
    
    std::string destPath(_SDFolder);
    destPath.append(t->getGenPath());

    const std::string folder = destPath.substr(0, destPath.length() - t->getName().length() - t->getExtention().length() - 4);
    createFolders(folder);

    if (isEmpty) {
        std::ofstream out;
        out.open(destPath);
        if (out.bad()) {
            printf("ERROR");
        }
        
        
    }else{
        int error = copyfile(path.c_str(), destPath.c_str(), NULL,  COPYFILE_DATA);
        if (!error)
            printf("ERROR copyfile:%s \n",destPath.c_str());
        
    }    
}

bool icompare_pred(unsigned char a, unsigned char b)
{
    return std::tolower(a) == std::tolower(b);
}

bool FileMng::icompare(const std::string& a,const std::string& b, bool& flag)
{
    if (a.length()!=b.length())
        return false;
    
    auto aIt = a.begin();
    auto bIt = b.begin();
    while (aIt != a.end() && bIt != b.end()) {
        if (std::tolower(*aIt) != std::tolower(*bIt))
            return false;
        
        if (!flag && *aIt != *bIt)
            flag = true;
        
        ++aIt;
        ++bIt;
        
    }

    return true;
}


void FileMng::getFilesFromPathRecursively(std::vector<std::string>& files, const std::string& path)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0] == '.') //ignore hide files
                continue;
            
            static char buff[1024];
            snprintf(buff, 1024, "%s/%s",path.c_str(),ent->d_name);
            const std::string& npath(buff);
            
            if (ent->d_type == DT_DIR) {
                getFilesFromPathRecursively(files, npath);
            }else{
                files.push_back(npath);
            }
        }
        closedir (dir);
    } else {
        printf("could not open directory:%s\n",path.c_str());
        return;
    }
}

void FileMng::scan()
{
    playlists.clear();
    playlists = parser->getPlaylists(_plKey.c_str());
    std::for_each(playlists.begin(), playlists.end(), [&] (ItunesPlaylist* p) {
        std::for_each(p->getTracks().begin(), p->getTracks().end(), [&] (int id) {
            ItunesTrack* track = parser->getTrack(id);
            if (track) {
                _tracks.insert(track);
            }
            
        });
    });
    
    std::vector<std::string> flashFiles;
    std::string folder(_SDFolder.substr(0,_SDFolder.length()-1));

    getFilesFromPathRecursively(flashFiles,folder);
    _filesToDelete.clear();
    _playlistToDelete.clear();
    
    for (Files::iterator it = flashFiles.begin(); it != flashFiles.end(); ++it) {
        const std::string flFile = *it;
        
        auto iter = std::find_if(_tracks.begin(), _tracks.end(), [this,flFile](ItunesTrack* track){
            const std::string filePath (flFile.substr(_SDFolder.length()));
            const std::string& genPath = track->getGenPath();
            bool flag = false;
            bool isEqualInsensitive = icompare(filePath, genPath, flag);
            if (flag) {
                printf("names with different case: \n%s\n%s\n",filePath.c_str(),genPath.c_str());
            }

            return isEqualInsensitive;
        });

        if (iter != _tracks.end()) {
            ItunesTrack* track = *iter;
            track->setHave(true);
        }else{
            size_t size = (*it).size();
            if ((*it)[size-3] == 'm' && (*it)[size-2] == '3' && (*it)[size-1] == 'u') {
                 _playlistToDelete.push_back(*it);
            }else{
                _filesToDelete.push_back(*it);
            }
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

void FileMng::sync(bool isDebug)
{
    std::for_each(_filesToDelete.begin(), _filesToDelete.end(), [](const std::string& path){
        if(remove(path.c_str()) != 0 )
            printf("Error deleting file%s \n",path.c_str());
    });
    
    printf("files deleted\n");
    
    deleteEmptyFolders(_SDFolder);
    
    float files = _fileToCopyCount;
    
    std::for_each(_tracks.begin(), _tracks.end(), [&](ItunesTrack* track){
        if(!track->getHave()){
            saveTrack(track, isDebug);
            --_fileToCopyCount;

            std::cout << "\r" << "copy " << ((int)files - _fileToCopyCount) << " / " << files
            << " progress:" << ((files - (float)_fileToCopyCount)/files) * 100.0f << "%"
            << std::flush;

        }
    });
    
    
    updatePlaylists();
    printf("\n files synced\n");
}

void FileMng::updatePlaylists()
{
    std::for_each(_playlistToDelete.begin(), _playlistToDelete.end(), [](const std::string& path){
        if(remove(path.c_str()) != 0 )
            printf("Error deleting file%s \n",path.c_str());
    });
    
    std::for_each(playlists.begin(), playlists.end(), [this](ItunesPlaylist* pList) {
        createPlaylist(pList);
    });
}

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
        std::string s = track->getGenPath();
        std::replace( s.begin(), s.end(), '/', '\\');
        snprintf(buff, 256, "TF1:\\%s\n",s.c_str());
        file << buff;
    });
    
    file.close();
}
