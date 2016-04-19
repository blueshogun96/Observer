//
//  osx_util.cpp
//  NinjaRun3D
//
//  Created by admin on 12/17/12.
//  Copyright (c) 2012 Shogun3D. All rights reserved.
//

#include "osx_util.h"
#include <cassert>


char* get_resource_url( CFStringRef filename, CFStringRef ext )
{
    CFBundleRef bundle;
    CFURLRef cascade_url;
    Boolean got_url;
    char string[2048];
    CFIndex length = 2048;
    
    bundle = CFBundleGetMainBundle();
    if( !bundle )
        return NULL;
    
    cascade_url = CFBundleCopyResourceURL( bundle, filename, ext, NULL );
    if( !cascade_url )
        return NULL;
    
    got_url = CFURLGetFileSystemRepresentation( cascade_url, true, reinterpret_cast<UInt8*>(string), length );
    if( !got_url )
        return NULL;
    
    return string;
}
