//
//  osx_util.h
//  NinjaRun3D
//
//  Created by admin on 12/17/12.
//  Copyright (c) 2012 Shogun3D. All rights reserved.
//

#ifndef __NinjaRun3D__osx_util__
#define __NinjaRun3D__osx_util__

//#include <iostream>
#include <CoreFoundation/CoreFoundation.h>

#ifdef __cplusplus
extern "C" {
#endif

char* get_resource_url( CFStringRef filename, CFStringRef ext );

#ifdef __cplusplus
}
#endif

#endif /* defined(__NinjaRun3D__osx_util__) */
