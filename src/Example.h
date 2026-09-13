/*
 * Copyright (c) 2024 - 2026 ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef _WIN32
    #include <windows.h>
    #ifndef PATH_MAX
        #define PATH_MAX MAX_PATH
    #endif
#else
    #include <dirent.h>
    #include <unistd.h>
    #include <limits.h>
    #include <sys/stat.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <thread>
#include <thorvg_toolkit.h>

/************************************************************************/
/* Common Template Code                                                 */
/************************************************************************/

namespace tvgexam
{

bool verify(tvg::Result result, std::string failMsg = "")
{
    switch (result) {
        case tvg::Result::FailedAllocation: {
            std::cout << "FailedAllocation! " << failMsg << std::endl;
            return false;
        }
        case tvg::Result::InsufficientCondition: {
            std::cout << "InsufficientCondition! " << failMsg << std::endl;
            return false;
        }
        case tvg::Result::InvalidArguments: {
            std::cout << "InvalidArguments! " << failMsg << std::endl;
            return false;
        }
        case tvg::Result::MemoryCorruption: {
            std::cout << "MemoryCorruption! " << failMsg << std::endl;
            return false;
        }
        case tvg::Result::NonSupport: {
            std::cout << "NonSupport! " << failMsg << std::endl;
            return false;
        }
        case tvg::Result::SystemError: {
            std::cout << "SystemError! " << failMsg << std::endl;
            return false;
        }
        case tvg::Result::Unknown: {
            std::cout << "Unknown! " << failMsg << std::endl;
            return false;
        }
        default: break;
    };
    return true;
}

struct Params
{
    int argc;
    char** argv;

    std::string name = "ThorVG Example (CPU)";
    tvg::toolkit::App::Size size;
    tvg::toolkit::RenderEngine engine = tvg::toolkit::RenderEngine::CPU;
    uint32_t threads = 4;
    bool fps = false;

    Params(int argc, char** argv, tvg::toolkit::App::Size size)
        : argc(argc), argv(argv), size(size) {}
};

struct Example : tvg::toolkit::App
{
    Example(const Params& params, bool clear = false) :
        tvg::toolkit::App(params.name, params.size, clear)
    {
        fps.enabled = params.fps;
    }

    virtual void populate(const char* path) {}

    bool update(tvg::Canvas* canvas, size_t elapsed) override
    {
        // print fps per 1 sec.
        if (fps.enabled && elapsed - fps.elapsed >= 1000) {
            const auto currentFps = tvg::toolkit::App::fps();
            fps.total += size_t(currentFps);
            std::cout << "[" << std::setw(3) << fps.count << "]: " << currentFps
                      << " / " << static_cast<uint32_t>(fps.total / (fps.count + 1)) << " fps" << std::endl;
            ++fps.count;
            fps.elapsed = elapsed;
        }
        return false;
    }

    bool keydown(tvg::Canvas* canvas, tvg::toolkit::Key key) override
    {
        if (key == tvg::toolkit::Key::Escape) quit();
        if (key == tvg::toolkit::Key::LeftShift) lshift = true;
        return false;
    }

    bool keyup(tvg::Canvas* canvas, tvg::toolkit::Key key) override
    {
        if (key == tvg::toolkit::Key::LeftShift) lshift = false;
        return false;
    }

    void scandir(const char* path)
    {
        char buf[PATH_MAX];

        // real path
#ifdef _WIN32
        auto rpath = _fullpath(buf, path, PATH_MAX);
#else
        auto rpath = realpath(path, buf);
#endif

        if (!rpath) return;

            // open directory
#ifdef _WIN32
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileExA((std::string(rpath) + "\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
        if (h == INVALID_HANDLE_VALUE) {
            std::cout << "Couldn't open directory \"" << rpath << "\"." << std::endl;
            return;
        }
        do {
            if (*fd.cFileName == '.' || *fd.cFileName == '$') continue;
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                auto fullpath = std::string(path);
                fullpath += '\\';
                fullpath += fd.cFileName;
                populate(fullpath.c_str());
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
#else
        DIR* dir = opendir(rpath);
        if (!dir) {
            std::cout << "Couldn't open directory \"" << rpath << "\"." << std::endl;
            return;
        }

        // list directory
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (*entry->d_name == '.' || *entry->d_name == '$') continue;
            if (entry->d_type != DT_DIR) {
                auto fullpath = std::string(path);
                fullpath += '/';
                fullpath += entry->d_name;
                populate(fullpath.c_str());
            }
        }
        closedir(dir);
#endif
    }

    bool lshift = false;  // left shift key

private:
    struct {
        size_t elapsed = 0;
        size_t count = 0;
        size_t total = 0;
        bool enabled;
    } fps;
};

// -i <filepath>
void filepath(const Params& params, std::string& out)
{
    // -i <filepath>
    for (int i = 1; i + 1 < params.argc; ++i) {
        if (std::strcmp(params.argv[i], "-i") == 0) {
            out = params.argv[i + 1];
            break;
        }
    }
}

Params options(int argc, char** argv, const tvg::toolkit::App::Size& size)
{
    Params params(argc, argv, size);

    // -r [width]x[height]
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], "-r") != 0) continue;
        auto value = argv[i + 1];
        auto separator = std::strchr(value, 'x');
        if (separator) {
            auto w = std::atoi(value);
            auto h = std::atoi(separator + 1);
            if (w > 0 && h > 0) {
                params.size.w = static_cast<uint32_t>(w);
                params.size.h = static_cast<uint32_t>(h);
            }
        }
        break;
    }

    // -e <engine>
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], "-e") == 0) {
            if (std::strcmp(argv[i + 1], "gl") == 0) {
                params.engine = tvg::toolkit::RenderEngine::GL;
                params.name = "ThorVG Example (GL)";
            }
            if (std::strcmp(argv[i + 1], "wg") == 0) {
                params.engine = tvg::toolkit::RenderEngine::WEBGPU;
                params.name = "ThorVG Example (WebGPU)";
            }
            break;
        }
    }

    // -t <thread>
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], "-t") != 0) continue;
        auto value = argv[i + 1];
        uint32_t threads = 0;
        bool valid = *value != '\0';
        for (; *value; ++value) {
            if (*value < '0' || *value > '9' || threads > (UINT32_MAX - (*value - '0')) / 10) {
                valid = false;
                break;
            }
            threads = threads * 10 + (*value - '0');
        }
        if (valid) params.threads = threads;
        break;
    }

    const auto maxThreads = std::thread::hardware_concurrency();
    if (params.threads > maxThreads) params.threads = maxThreads;

    // --fps
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--fps") == 0) {
            params.fps = true;
            break;
        }
    }

    return params;
}

void summary(const Params& params)
{
    const char* engine = "CPU";
    if (params.engine == tvg::toolkit::RenderEngine::GL) engine = "GL";
    else if (params.engine == tvg::toolkit::RenderEngine::WEBGPU) engine = "WebGPU";
    std::cout << "options: engine=" << engine
              << ", resolution=" << params.size.w << 'x' << params.size.h
              << ", threads=" << params.threads << '\n';
}

int run(Example* app, const Params& params)
{
    summary(params);

    if (!verify(tvg::Initializer::init(params.threads))) return 1;
    if (!verify(tvg::toolkit::run(app, params.engine))) return 1;
    if (!verify(tvg::Initializer::term())) return 1;
    return 0;
}

};  // namespace tvgexam
