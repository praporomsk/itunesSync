//
//  FileMng.hpp
//  itunesSync
//
//  Created by Roman Semenov on 27.12.15.
//  Copyright © 2015 Roman. All rights reserved.
//

#ifndef FileMng_hpp
#define FileMng_hpp

#include <stdio.h>
#include "ItunesParser.hpp"
#include <set>

typedef std::vector<std::string> Files;

class FileMng
{
    CC_SYNTHESIZE(std::string, _SDFolder, SDFolder);
    CC_SYNTHESIZE(std::string, _plKey, PlKey);
public:
    bool init(ItunesParser* p);
    void scan();
    void sync();
private:
    void saveTrack(ItunesTrack* t);
    
    ItunesParser* parser;
    
    Files _filesToDelete;
    std::set<ItunesTrack*> _tracks;
    int _fileToCopyCount;
};
#endif /* FileMng_hpp */
