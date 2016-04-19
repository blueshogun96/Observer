//
//  opencv_drv.h
//  Observer
//
//  Created by admin on 3/4/13.
//  Copyright (c) 2013 Shogun3D. All rights reserved.
//

#ifndef __Observer__opencv_drv__
#define __Observer__opencv_drv__

#include <OpenCV/OpenCV.h>
//#include <iostream>

struct object_t
{
    int x, y;   /* Screen position */
    int radius; /* Radius of space */
};

#ifndef __OBJC__
/* OpenCV class */
class opencv_drv
{
public:
    opencv_drv();
    virtual ~opencv_drv();
    
public:
    bool enable( char* window_name );
    void disable();
    void update();
    
    void set_frame_consistency( int frames )
    {
        m_frame_consistency = frames;
    }
    
    void set_max_radius( int radius )
    {
        m_max_radius = radius;
    }
    
    bool get_face( object_t* obj )
    {
        memcpy( obj, &m_face, sizeof( object_t ) );
        return true;
    }
    
    bool get_hand( object_t* obj )
    {
        memcpy( obj, &m_hand, sizeof( object_t ) );
        return true;
    }
    
protected:
    bool open_cascades();
    bool detect_hands();
    bool detect_faces();
    
protected:
    CvCapture* m_camera;
    CvMemStorage* m_storage;
    IplImage* m_current_frame;
    IplImage* m_draw_image;
    IplImage* m_gray_image;
    IplImage* m_small_image;
    
    CvHaarClassifierCascade* m_face_cascade;
    CvHaarClassifierCascade* m_hand_cascade;
    
    struct object_t m_hand;
    struct object_t m_face;
    
    int m_frame_consistency;
    int m_max_radius;
    
    char m_window_name[128];
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
bool opencv_enable( char* window_name );
void opencv_update();
void opencv_disable();
    bool opencv_get_hand( struct object_t* obj );
    bool opencv_get_face( struct object_t* obj );
    
#ifdef __cplusplus
}
#endif

#endif /* defined(__Observer__opencv_drv__) */
