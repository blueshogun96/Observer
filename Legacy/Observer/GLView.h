#pragma once

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

@interface GLView : NSView { }
-(void)drawRect:(NSRect)rect;
@end

// Mini-mini class for the timer
@interface TimerController : NSObject { }
-(void)timerFireMethod:(NSTimer *)t;
@end

