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

static const char *helpText =
"\n"
"-lib itunes library path (xml file) \n"
"-o output folder v\n"
"-key only copies playlists with this key word \n"
"-d debug flag create empty files\n";

int main(int argc, const char * argv[]) {
#define ABORT(msg) { puts(msg); return 0; }
    
    std::string itunesDBPath = "/Users/rsemenov/Dropbox/iTunes\ Music\ Library.xml";
    std::string sdFolderPath = "/Users/rsemenov/Desktop/cs";
    std::string plKey = "FIIO";
    bool isDebug = false;
    
    int argPos = 1;
    bool suggestHelp = false;
    while (argPos < argc) {
        const char *arg = argv[argPos];
#define ARG_CASE(s, p) if (!strcmp(arg, s) && argPos+(p) < argc)
        
        ARG_CASE("-lib", 1) {
            itunesDBPath = argv[argPos+1];
            argPos += 2;
            continue;
        }
        ARG_CASE("-o", 1) {
            sdFolderPath = argv[argPos+1];
            argPos += 2;
            continue;
        }
        ARG_CASE("-key", 1) {
            plKey = argv[argPos+1];
            argPos += 2;
            continue;
        }
        ARG_CASE("-d", 0) {
            isDebug = true;
            argPos += 1;
            continue;
        }
        ARG_CASE("-help", 0)
        ABORT(helpText);
        printf("Unknown setting or insufficient parameters: %s\n", arg);
        suggestHelp = true;
        ++argPos;
    }
    if (suggestHelp)
        printf("Use -help for more information.\n");
    
    if (sdFolderPath[sdFolderPath.length()-1] != '/')
        sdFolderPath.append("/");

    
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
        fileMng->sync(isDebug);
    }else{
        char str [80];
        scanf ("%79s",str);
        if (!std::strcmp(str, "y")) {
            fileMng->sync(isDebug);
        }
    }

    delete fileMng;
    delete parser;
    return 0;
}
