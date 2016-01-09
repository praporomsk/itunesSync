//
//  main.cpp
//  itunesSync
//
//  Created by Roman Semenov on 27.12.15.
//  Copyright © 2015 Roman. All rights reserved.
//

#include <iostream>
#include "ItunesParser.hpp"
#include "FileMng.hpp"

#define FOR_DEBUG 1

int main(int argc, const char * argv[]) {

    std::string itunesDBPath;
    std::string sdFolderPath;
    std::string plKey;
    //добавить / в путь к флешке
    if (FOR_DEBUG) {
        itunesDBPath = "/Users/romansemenov/Music/iTunes/iTunes\ Music\ Library.xml";
        sdFolderPath = "/Volumes/FIIO/";
        plKey = "BOX";
    }else{
        
        if (argc != 4) {
            printf("usage: ./itunesSync  /Users/USERNAME/Music/iTunes/iTunes\\ Music\\ Library.xml  /SDCARD_FOLDER_PATH /PLAYLIST_KEY \n itunesSync only copies playlists with the word PLAYLIST_KEY in them \n");
            exit(0);
        }
        
        itunesDBPath = argv[1];
        sdFolderPath = argv[2];
        plKey = argv[3];
    }

    
    ItunesParser* parser = new ItunesParser();
    parser->setITunesDatabasePath(itunesDBPath);
    if (!parser->parse())
        return 0;
    
    
    FileMng* fileMng = new FileMng();
    fileMng->setSDFolder(sdFolderPath);
    fileMng->setPlKey(plKey);
    fileMng->init(parser);
    fileMng->scan();
    
    if (FOR_DEBUG) {
        fileMng->sync();
    }else{
        char str [80];
        scanf ("%79s",str);
        if (!std::strcmp(str, "y")) {
            fileMng->sync();
        }
    }

    delete fileMng;
    delete parser;
    return 0;
}
