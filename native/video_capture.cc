
#define _CRT_SECURE_NO_WARNINGS

#include "video_capture.h"

#define kInuWebcamStream 9
#define kInuDepthStream 5

bool VideoCapture::init() {
    CInuError err(eInitError);
    ECameraName iCameraName = eWebCam;
    CDeviceParamsExt prm;
    CHwInformation channels;
    std::map<CEntityVersion::EEntitiesID, CEntityVersion> oVersion;
    bool rc;
    int init_tries = 0;

    this->image_frame = shared_ptr<CImageFrame>(new CImageFrame());
    this->depth_frame = shared_ptr<CImageFrame>(new CImageFrame());

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

    for (auto const &x : channels.GetChannels()) {
        std::cout << "type: " << x.second.ChannelType
                  << " id: " << x.second.ChannelId << std::endl;
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

    this->webcam_stream = this->sensor->CreateImageStream(kInuWebcamStream);
    err = this->webcam_stream->Init();
    if (err != eOK) {
        std::cout << "Webcam stream failed to Init" << std::endl;
        return false;
    }

    this->depth_stream = this->sensor->CreateDepthStream(kInuDepthStream);
    err = this->depth_stream->Init(
        CDepthStream::eDepthRegistration, false, kInuWebcamStream);

    if (err != eOK) {
        std::cout << "Depth stream failed to Init" << std::endl;
        return false;
    }

    err = this->webcam_stream->Start();
    if (err != eOK) {
        std::cout << "Webcam stream failed to Start" << std::endl;
        return false;
    }
    std::cout << "WebCam Started" << std::endl;

    err = this->depth_stream->Start();
    if (err != eOK) {
        std::cout << "Depth stream failed to Start" << std::endl;
        return false;
    }
    std::cout << "DepthCam Started" << std::endl;
    return (true);
}

VideoCapture::VideoCapture() {
    if (!init()) {
        exit(-1);
    }
    this->webcam_stream->GetFrame(this->image_frame);
    this->depth_stream->GetFrame(this->depth_frame);
}

void VideoCapture::GetShape(int *height, int *width) {
    *height = this->image_frame->Height();
    *width = this->image_frame->Width();
}

cv::Mat VideoCapture::ReadDepthImage() {
    CInuError err(eInitError);
    this->depth_stream->GetFrame(this->depth_frame);
    int image_width = this->depth_frame->Width();
    int image_height = this->depth_frame->Height();

    assert(this->depth_frame->Format() == CImageFrame::eDepth);
    cv::Mat img = cv::Mat(
        image_height, image_width, CV_16U,
        (uchar *)this->depth_frame->GetData());
    cv::flip(img, img, 2);
    return img;
}

cv::Mat VideoCapture::ReadBGRImage() {
    CInuError err(eInitError);
    this->webcam_stream->GetFrame(this->image_frame);
    cv::Mat img;

    int image_width = this->image_frame->Width();
    int image_height = this->image_frame->Height();

    switch (this->image_frame->Format()) {
        case CImageFrame::eRGB:
            img = cv::Mat(
                image_height, image_width, CV_8UC3,
                (uchar *)this->image_frame->GetData());
            cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
            break;
        case CImageFrame::eBGR:
            img = cv::Mat(
                image_height, image_width, CV_8UC3,
                (uchar *)this->image_frame->GetData());
            break;
        case CImageFrame::eBGRA:
            img = cv::Mat(
                image_height, image_width, CV_8UC4,
                (uchar *)this->image_frame->GetData());
            // TODO: inu bug? the frame format seems like RGBA instead of
            // reported BGRA
            cv::cvtColor(img, img, cv::COLOR_RGBA2BGR);
            break;
        case CImageFrame::eRGBA:
            img = cv::Mat(
                image_height, image_width, CV_8UC4,
                (uchar *)this->image_frame->GetData());
            cv::cvtColor(img, img, cv::COLOR_RGBA2RGB);
            break;
        default:
            printf(
                "Got unrecognised format: %d\n", this->image_frame->Format());
            break;
    }
    cv::flip(img, img, 2);
    return img;
}
VideoCapture::~VideoCapture() { this->sensor->Terminate(); }
