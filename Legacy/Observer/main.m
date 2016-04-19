//
//  main.m
//  Observer
//
//  Created by admin on 3/4/13.
//  Copyright (c) 2013 Shogun3D. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "GLView.h"

NSApplication *myApp;
NSView *view;
NSAutoreleasePool *pool;
NSWindow *window;
NSTimer	*gTimer;
TimerController	*myTimerController;

int main(int argc, const char *argv[])
{
	pool = [NSAutoreleasePool new];
	myApp = [NSApplication sharedApplication];
//	home();
    
	NSRect frame = NSMakeRect(10., 1000., 640., 480.);
    
	window = [NSWindow alloc];
	[window initWithContentRect:frame
                      styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask
                        backing:NSBackingStoreBuffered
                          defer:false];
	[window setTitle:@"Observer (OpenGL)"];
    
	view = [GLView alloc];
	[view initWithFrame: frame];
    
	// OpenGL init!
	MakeContext(view);
    
    init ();
//	initShaders();
    dumpInfo ();
    
	[window setContentView: view];
	[window setDelegate: view];
	[window makeKeyAndOrderFront: nil];
    
	// Timer
	myTimerController = [TimerController alloc];
	gTimer = [NSTimer
              scheduledTimerWithTimeInterval: 0.02
              target: myTimerController
              selector: @selector(timerFireMethod:)
              userInfo: nil
              repeats: YES];
    
	// Main loop
	[myApp run];
	[pool release]; // Free;
	return( EXIT_SUCCESS );
}