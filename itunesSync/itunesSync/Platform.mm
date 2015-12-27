//
//  Platform.cpp
//  itunesSync
//
//  Created by Roman Semenov on 27.12.15.
//  Copyright © 2015 Roman. All rights reserved.
//

#include "Platform.h"
#import <Foundation/Foundation.h>

void urlToPath(std::string& urlStr)
{
    NSURL* url = [NSURL URLWithString:[NSString stringWithUTF8String:urlStr.c_str()]];
    urlStr = [[url path] UTF8String];
}

void createFolders(const std::string& urlStr)
{
    NSFileManager* fm = [[NSFileManager alloc] init];
    [fm createDirectoryAtPath:[NSString stringWithUTF8String:urlStr.c_str()]
  withIntermediateDirectories:YES
                   attributes:nil
                        error:nil];
}

void scanPath(std::vector<std::string>& files, const std::string& path)
{
    NSString* sPath = [NSString stringWithUTF8String:path.c_str()];
    
    BOOL isDir;
    [[NSFileManager defaultManager] fileExistsAtPath:sPath isDirectory:&isDir];
    if (isDir) {
        NSArray *contentOfDirectory=[[NSFileManager defaultManager] contentsOfDirectoryAtPath:sPath error:NULL];
        
        int contentcount = [contentOfDirectory count];
        int i;
        for(i=0;i<contentcount;i++)
        {
            NSString *fileName = [contentOfDirectory objectAtIndex:i];
            NSString *path = [sPath stringByAppendingFormat:@"%@%@",@"/",fileName];
            
            if([[NSFileManager defaultManager] isDeletableFileAtPath:path])
            {
                scanPath(files, [path UTF8String]);
            }
        }
    }else{
        files.push_back(path);
    }
}