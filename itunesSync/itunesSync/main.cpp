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

int main(int argc, const char * argv[]) {

    if (argc != 4) {
        printf("usage: ./itunesSync  /Users/USERNAME/Music/iTunes/iTunes\\ Music\\ Library.xml  /SDCARD_FOLDER_PATH /PLAYLIST_KEY \n itunesSync only copies playlists with the word PLAYLIST_KEY in them \n");
        exit(0);
    }
    
    ItunesParser* parser = new ItunesParser();
    parser->setITunesDatabasePath(argv[1]);
    parser->parse();
    
    FileMng* fileMng = new FileMng();
    fileMng->setSDFolder(argv[2]);
    fileMng->setPlKey(argv[3]);
    fileMng->init(parser);
    fileMng->scan();
    
    char str [80];
    scanf ("%79s",str);
    if (!std::strcmp(str, "y")) {
        fileMng->sync();
    }
    
    
    delete fileMng;
    delete parser;
    return 0;
}
