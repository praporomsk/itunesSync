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
    NSError* error;
    
    NSFileManager* fm = [[NSFileManager alloc] init];
    BOOL success = [fm createDirectoryAtPath:[NSString stringWithUTF8String:urlStr.c_str()]
  withIntermediateDirectories:YES
                   attributes:nil
                        error:&error];
    if (!success) {
        NSLog(@"\n Create Folders failed with error: %@", error.localizedDescription);
    }
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

void deleteEmptyFolders(const std::string& path)
{
    NSFileManager *fileManager = [[NSFileManager alloc] init];
    NSString *documentsDirectory = [NSString stringWithUTF8String:path.c_str()];
    
    BOOL isDir = NO;
    
    [[NSFileManager defaultManager] fileExistsAtPath:documentsDirectory
                                         isDirectory:&isDir];
    if (!isDir)
        return;
    
    NSArray *files = [fileManager contentsOfDirectoryAtPath:documentsDirectory error:nil];
    for (NSString *file in files) {
        if([file hasPrefix:@"."])//ignore hidden files
            continue;
        
        NSString *fullPath = [documentsDirectory stringByAppendingPathComponent:file];
        deleteEmptyFolders([fullPath UTF8String]);
    
        NSError *error;
        NSInteger count = [[fileManager contentsOfDirectoryAtPath:fullPath error:&error] count];
        if (count==0) {
            // check if number of files == 0 ==> empty directory
            if (!error) {
                // check if error set (fullPath is not a directory and we should leave it alone)
                NSError* error;
                BOOL success = [fileManager removeItemAtPath:fullPath error:&error];
                if (!success) {
                    NSLog(@"\n Delete Empty Folder failed with error: %@", error.localizedDescription);
                }
            }
        }
    }
}

void copyFile(const std::string& sorce, const std::string& sdFolder)
{
    NSError* error;
    BOOL success = [[NSFileManager defaultManager] copyItemAtPath:[NSString stringWithUTF8String:sorce.c_str()] toPath:[NSString stringWithUTF8String:sdFolder.c_str()] error:&error];
    if (!success) {
        NSLog(@"\n Write failed with error: %@", error.localizedDescription);
    }
}
