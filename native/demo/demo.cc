
#define _CRT_SECURE_NO_WARNINGS

#include "demo.h"

using namespace std;
using namespace InuDev;

std::shared_ptr<CInuSensorExt> sensor;
std::shared_ptr<CImageStream> webcamStream;

static int frames_per_sec = 60;

static int active_net_id = 0;
static bool Gl_fps_display = true;

static int image_width = 640;
static int image_height = 480;

void WebCamCallback(std::shared_ptr<CImageStream>,
                    std::shared_ptr<const InuDev::CImageFrame> iFrame,
                    CInuError) {
    cv::Mat img;

    // printf("Webcam: H:%d W:%d Size:%d format:%d\n", iFrame->Height(),
    // iFrame->Width(), iFrame->BufferSize(), iFrame->Format()); printf("-------
    // Webcamcallback. frame_id:%d\n", iFrame->FrameIndex);

    image_width = iFrame->Width();
    image_height = iFrame->Height();

    switch (iFrame->Format()) {
        case CImageFrame::eRGB:
            img = cv::Mat(image_height, image_width, CV_8UC3,
                          (uchar *)iFrame->GetData());
            cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
            break;
        case CImageFrame::eBGR:
            img = cv::Mat(image_height, image_width, CV_8UC3,
                          (uchar *)iFrame->GetData());
            break;
        case CImageFrame::eBGRA:
            img = cv::Mat(image_height, image_width, CV_8UC4,
                          (uchar *)iFrame->GetData());
            cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
            break;
        case CImageFrame::eRGBA:
            img = cv::Mat(image_height, image_width, CV_8UC4,
                          (uchar *)iFrame->GetData());
            cv::cvtColor(img, img, cv::COLOR_RGBA2RGB);
            break;
        case CImageFrame::eEmpty:
            return;  // Do nothing
        default:
            printf("Got unrecognised format: %d\n", iFrame->Format());
            return;
    }

    cv::namedWindow("Inuitive Ltd.", cv::WINDOW_NORMAL);
    cv::imshow("Inuitive Ltd.", img);
    cv::waitKey(1);
}

/* init_example */
bool init_example(void) {
    CInuError err(eInitError);
    ECameraName iCameraName = eWebCam;
    CDeviceParamsExt prm;
    CHwInformation channels;
    std::map<CEntityVersion::EEntitiesID, CEntityVersion> oVersion;
    bool rc;
    int init_tries = 0;

    sensor = CInuSensorExt::Create();
    prm.SensorRes = eBinning;
    prm.FPS = frames_per_sec;

    std::cout << "frame per second: " << frames_per_sec << ::endl;

    while (err != eOK && init_tries < 10) {
        err = sensor->Init(channels, prm);
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

    /* Determine module version */
    err = sensor->GetVersion(oVersion);
    if (err != eOK) {
        std::cout << "sensor->GetVersion failed  " << string(err) << std::endl;
        return false;
    }

    std::cout << "Module version:  "
              << oVersion[CEntityVersion::EEntitiesID::eHWVersion].VersionName
              << std::endl;

    CChannelDimensions cs;
    int image_height, image_width;

    image_height = 300;
    image_width = 304;

    cs.BufferHeight = image_height;
    cs.BufferWidth = image_width;
    cs.FrameStartX = 0;
    cs.FrameWidth = image_width;
    cs.FrameStartY = 0;
    cs.FrameHeight = image_height;

    err = sensor->SetChannelDimensions(4, cs);
    if (err != eOK) {
        std::cout << "SetChannelDimensions failed to SetChannelDimensions: "
                  << string(err) << std::endl;
    }

    err = sensor->Start();
    if (err != eOK) {
        std::cout << "InuSensor failed to Start" << std::endl;
        return false;
    }
    std::cout << "Sensor Started" << std::endl;

    /* Create webcam stream */
    webcamStream = sensor->CreateImageStream(9);

    err = webcamStream->Init();
    if (err != eOK) {
        std::cout << "Webcam stream failed to Init" << std::endl;
        return false;
    }

    err = webcamStream->Start();
    if (err != eOK) {
        std::cout << "Webcam stream failed to Start" << std::endl;
        return false;
    }
    std::cout << "WebCam Started" << std::endl;
    webcamStream->Register(WebCamCallback);
    return (true);
}

int main(int argc, char *argv[]) {
    CInuError err(eOK);
    printf("OpenCV version : %s\n", CV_VERSION);

    if (!init_example()) {
        exit(-1);
    }

    sleep(100000);
    sensor->Terminate();

    return 0;
}
