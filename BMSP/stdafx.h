#pragma once
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include <array>
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <AL/al.h>
#include <AL/alc.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}