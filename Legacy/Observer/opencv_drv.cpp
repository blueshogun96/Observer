//
//  opencv_drv.cpp
//  Observer
//
//  Created by admin on 3/4/13.
//  Copyright (c) 2013 Shogun3D. All rights reserved.
//

#include "opencv_drv.h"
#include "osx_util.h"

/* Global scaling factor */
const int scale = 2;

opencv_drv::opencv_drv()
{
    memset( &m_hand, 0, sizeof( object_t ) );
    memset( &m_face, 0, sizeof( object_t ) );
}

opencv_drv::~opencv_drv()
{
}

bool opencv_drv::open_cascades()
{
    char string[512];
    
    /* Load the hand tracking cascade */
    sprintf( string, "Observer.app/Contents/Resources/HaarCascades/hands/aGest.xml" );
    m_hand_cascade = (CvHaarClassifierCascade*) cvLoad( string, 0, 0, 0 );
    if( !m_hand_cascade )
    {
        printf( "opencv_drv::open_cascades(): Error loading hand gesture cascade!\n" );
        return false;
    }
    
    /* Load the face tracking cascade */
    sprintf( string, "Observer.app/Contents/Resources/HaarCascades/face/haarcascade_frontalface_alt2.xml" );
    m_face_cascade = (CvHaarClassifierCascade*) cvLoad( string, 0, 0, 0 );
    if( !m_hand_cascade )
    {
        printf( "opencv_drv::open_cascades(): Error loading front face cascade!\n" );
        return false;
    }
    
    return true;
}

bool opencv_drv::enable( char* window_name )
{
    /* Create an OpenCV window with the default size */
    strcpy( m_window_name, window_name );
    cvNamedWindow( window_name );
    
    /* Initialize any available cameras */
    m_camera = cvCreateCameraCapture( CV_CAP_ANY );
    if( !m_camera )
    {
        printf( "opencv_drv::enable(): Could not initialize camera!\n" );
        return false;
    }
    
    /* Load all cascade files */
    if( !this->open_cascades() )
        return false;
    
    /* Create mem sorage */
    m_storage = cvCreateMemStorage();
    if( !m_storage )
    {
        printf( "opencv_drv::enable(): Could not create memory storage!\n" );
        return false;
    }
    
    /* Query the first camera frame */
    m_current_frame = cvQueryFrame( m_camera );
    if( !m_current_frame )
    {
        printf( "opencv_drv::enable(): Could not query initial frame!\n" );
        return false;
    }
    
    /* Create drawing image */
    m_draw_image = cvCreateImage( cvSize( m_current_frame->width, m_current_frame->height ), IPL_DEPTH_8U, 3 );
    if( !m_draw_image )
    {
        printf( "opencv_drv::enable(): Could not create drawing image!\n" );
        return false;
    }
    
    /* Create an image used for gray scale */
    m_gray_image = cvCreateImage( cvSize( m_current_frame->width, m_current_frame->height ), IPL_DEPTH_8U, 1 );
    if( !m_gray_image )
    {
        printf( "opencv_drv::enable(): Could not create gray scale image!\n" );
        return false;
    }
    
    m_small_image = cvCreateImage( cvSize( m_current_frame->width/scale, m_current_frame->height/scale ), IPL_DEPTH_8U, 1 );
    if( !m_small_image )
    {
        printf( "opencv_drv::enable(): Could not create small grey scale image!\n" );
        return false;
    }
    
    return true;
}

void opencv_drv::disable()
{
    /* Release OpenCV resources */
    cvReleaseImage( &m_small_image );
    cvReleaseImage( &m_gray_image );
    cvReleaseImage( &m_draw_image );
    cvReleaseMemStorage( &m_storage );
    cvDestroyWindow( m_window_name );
}

void opencv_drv::update()
{
    /* Get the next frame */
    m_current_frame = cvQueryFrame( m_camera );
    
    /* Convert to gray scale and downsize it */
    cvCvtColor( m_current_frame, m_gray_image, CV_BGR2GRAY );
    cvResize( m_gray_image, m_small_image, CV_INTER_LINEAR );
    
    /* Flip the image vertically */
    cvFlip( m_current_frame, m_draw_image, 1 );
    
    /* Detect hands and faces */
    detect_hands();
    detect_faces();
    
    /* Show resulting image */
    cvShowImage( m_window_name, m_draw_image );
}

bool opencv_drv::detect_faces()
{
    int bradius = 0;
    
    /* Detect face gestures */
    /* TODO: Face angles */
    CvSeq* faces = cvHaarDetectObjects( m_small_image, m_face_cascade, m_storage, 1.1,
                                       scale, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );
    
    /* Return false if we didn't detect any hand gestures */
    if( !faces )
        return false;
    
    /* Draw circiles around hand gestures */
    for( int i = 0; i < ( faces ? faces->total : 0 ); i++ )
    {
        CvRect* r = (CvRect*) cvGetSeqElem( faces, i );
        CvPoint center;
        int radius;
        
        center.x = cvRound( ( m_small_image->width - r->width * 0.5 - r->x ) * scale );
        center.y = cvRound( ( r->y + r->height * 0.5 ) * scale );
        radius = cvRound( ( r->width + r->height ) * 0.25 * scale );
        cvCircle( m_draw_image, center, radius, CV_RGB( 255, 0, 0 ), 3, 8, 0 );
        
        /* Check for a circle with a bigger radius. The bigger the radius, the closer
           this face is to the camera. Save this as the active face. */
        if( radius > bradius )
        {
            m_face.x = center.x;
            m_face.y = center.y;
            m_face.radius = radius;
            bradius = radius;
        }
    }
    
    return true;
}

bool opencv_drv::detect_hands()
{
    int bradius = 0;
    
    /* Detect hand gestures */
    /* TODO: More hand gestures (not just a fist) */
    CvSeq* hands = cvHaarDetectObjects( m_small_image, m_hand_cascade, m_storage, 1.1,
                                       scale, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30) );
    
    /* Return false if we didn't detect any hand gestures */
    if( !hands )
        return false;
    
    /* Draw circiles around hand gestures */
    for( int i = 0; i < ( hands ? hands->total : 0 ); i++ )
    {
        CvRect* r = (CvRect*) cvGetSeqElem( hands, i );
        CvPoint center;
        int radius;
        
        center.x = cvRound( ( m_small_image->width - r->width * 0.5 - r->x ) * scale );
        center.y = cvRound( ( r->y + r->height * 0.5 ) * scale );
        radius = cvRound( ( r->width + r->height ) * 0.25 * scale );
        cvCircle( m_draw_image, center, radius, CV_RGB( 0, 255, 0 ), 3, 8, 0 );
        
        /* Check for a circle with a bigger radius. The bigger the radius, the closer
           this hand is to the camera. Save this as the active hand. */
        /* TODO: Accurately check for left and right hands while discerning which had is
           the left from the right. */
        if( radius > bradius )
        {
            m_hand.x = center.x;
            m_hand.y = center.y;
            m_hand.radius = radius;
            bradius = radius;
        }
    }
    
    return true;
}

opencv_drv opencv;

bool opencv_enable( char* window_name )
{
    return opencv.enable( window_name );
}

void opencv_update()
{
    opencv.update();
}

void opencv_disable()
{
    opencv.disable();
}

bool opencv_get_hand( object_t* obj )
{
    return opencv.get_hand(obj);
}

bool opencv_get_face( object_t* obj )
{
    return opencv.get_face(obj);
}