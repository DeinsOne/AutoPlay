-- OpenCV dependencies
opencv = {}

opencv['bin'] = '/home/deins/Code/opencv/libopencv/bin'

opencv['include'] = {
    cv4 = 'vendor/libopencv/include',
    cv2 = 'vendor/libopencv/include/opencv4'
}

opencv['lib'] = 'vendor/libopencv/lib'

opencv['libs'] = {
    core = 'opencv_core',
    calib3d = 'opencv_calib3d',
    dnn = 'opencv_dnn',
    features2d = 'opencv_features2d',
    flann = 'opencv_flann',
    highgui = 'opencv_highgui',
    imgcodecs = 'opencv_imgcodecs',
    imgproc = 'opencv_imgproc',
    ml = 'opencv_ml',
    objdetect = 'opencv_objdetect',
    photo = 'opencv_photo',
    stitching = 'opencv_stitching',
    video = 'opencv_video',
    videoio = 'opencv_videoio'
}
