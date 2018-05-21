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
#include <future>
#include <mutex>
#include <filesystem>
#include <locale>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GL/glew.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <ft2build.h>
#include FT_FREETYPE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}