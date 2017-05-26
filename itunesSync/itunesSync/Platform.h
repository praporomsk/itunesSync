//
//  Platform.hpp
//  itunesSync
//
//  Created by Roman Semenov on 27.12.15.
//  Copyright © 2015 Roman. All rights reserved.
//

#ifndef Platform_hpp
#define Platform_hpp

#include <stdio.h>
#include <string>
#include <vector>

void urlToPath(std::string& urlStr);
void createFolders(const std::string& urlStr);
void deleteEmptyFolders(const std::string& path);

#endif /* Platform_hpp */
