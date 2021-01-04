#ifndef _COMMON_H
#define _COMMON_H

#include <dirent.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <queue>
#include <string>
#include "config.h"

#include "InuSensorExt.h"
#include "InuStreams.h"

using namespace std;
using namespace InuDev;

#endif
