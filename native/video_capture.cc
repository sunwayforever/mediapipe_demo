
#define _CRT_SECURE_NO_WARNINGS

#include "video_capture.h"

bool VideoCapture::init() {
    CInuError err(eInitError);
    ECameraName iCameraName = eWebCam;
    CDeviceParamsExt prm;
    CHwInformation channels;
    std::map<CEntityVersion::EEntitiesID, CEntityVersion> oVersion;
    bool rc;
    int init_tries = 0;

    this->frame = shared_ptr<CImageFrame>(new CImageFrame());
    this->sensor = CInuSensorExt::Create();
    prm.SensorRes = eBinning;
    prm.FPS = 60;

    while (err != eOK && init_tries < 10) {
        err = this->sensor->Init(channels, prm);
        if (err != eOK) {
            std::cout << "InuSensor failed to Init. Retry..." << string(err)
                      << std::endl;
            init_tries++;
        }
    }
    if (err != eOK) {
        return false;
    }

    std::cout << "InuSensor Init OK" << std::endl;

    err = this->sensor->GetVersion(oVersion);
    if (err != eOK) {
        std::cout << "this->sensor->GetVersion failed  " << string(err)
                  << std::endl;
        return false;
    }

    std::cout << "Module version:  "
              << oVersion[CEntityVersion::EEntitiesID::eHWVersion].VersionName
              << std::endl;

    err = this->sensor->Start();
    if (err != eOK) {
        std::cout << "InuSensor failed to Start" << std::endl;
        return false;
    }
    std::cout << "Sensor Started" << std::endl;

    /* Create webcam stream */
    this->webcam_stream = this->sensor->CreateImageStream(9);

    err = this->webcam_stream->Init();
    if (err != eOK) {
        std::cout << "Webcam stream failed to Init" << std::endl;
        return false;
    }

    err = this->webcam_stream->Start();
    if (err != eOK) {
        std::cout << "Webcam stream failed to Start" << std::endl;
        return false;
    }
    std::cout << "WebCam Started" << std::endl;
    return (true);
}

VideoCapture::VideoCapture() {
    if (!init()) {
        exit(-1);
    }
}

cv::Mat VideoCapture::read() {
    CInuError err(eInitError);
    this->webcam_stream->GetFrame(this->frame);
    cv::Mat img;

    int image_width = this->frame->Width();
    int image_height = this->frame->Height();

    switch (this->frame->Format()) {
        case CImageFrame::eRGB:
            img = cv::Mat(image_height, image_width, CV_8UC3,
                          (uchar *)this->frame->GetData());
            cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
            break;
        case CImageFrame::eBGR:
            img = cv::Mat(image_height, image_width, CV_8UC3,
                          (uchar *)this->frame->GetData());
            break;
        case CImageFrame::eBGRA:
            img = cv::Mat(image_height, image_width, CV_8UC4,
                          (uchar *)this->frame->GetData());
            // TODO: inu bug? the frame format seems like RGBA instead of
            // reported BGRA
            cv::cvtColor(img, img, cv::COLOR_RGBA2BGR);
            break;
        case CImageFrame::eRGBA:
            img = cv::Mat(image_height, image_width, CV_8UC4,
                          (uchar *)this->frame->GetData());
            cv::cvtColor(img, img, cv::COLOR_RGBA2RGB);
            break;
        default:
            printf("Got unrecognised format: %d\n", this->frame->Format());
            break;
    }
    cv::flip(img, img, 2);
    return img;
}
VideoCapture::~VideoCapture() { this->sensor->Terminate(); }
