#include <kazen/common.h>

#include <iomanip>

#if defined(__LINUX__)
#  if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#  endif
#  include <dlfcn.h>
#  include <unistd.h>
#  include <limits.h>
#  include <sys/ioctl.h>
#elif defined(__OSX__)
#  include <sys/sysctl.h>
#  include <mach-o/dyld.h>
#  include <unistd.h>
#  include <sys/ioctl.h>
#elif defined(__WINDOWS__)
#  include <windows.h>
#endif


NAMESPACE_BEGIN(kazen)

NAMESPACE_BEGIN(util)
    // timeString
    std::string timeString(double time, bool precise)
    {
        if (std::isnan(time) || std::isinf(time))
            return "inf";

        std::string suffix = "ms";
        if (time > 1000) {
            time /= 1000; suffix = "s";
            if (time > 60) {
                time /= 60; suffix = "m";
                if (time > 60) {
                    time /= 60; suffix = "h";
                    if (time > 12) {
                        time /= 12; suffix = "d";
                    }
                }
            }
        }

        std::ostringstream os;
        os << std::setprecision(precise ? 4 : 1)
        << std::fixed << time << suffix;

        return os.str();
    }

    // terminalWidth
    int terminalWidth() {
        static int cachedWidth = -1;

        if (cachedWidth == -1) {
#if defined(__WINDOWS__)
            HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
            if (h != INVALID_HANDLE_VALUE && h != nullptr) {
                CONSOLE_SCREEN_BUFFER_INFO bufferInfo = {0};
                GetConsoleScreenBufferInfo(h, &bufferInfo);
                cachedWidth = bufferInfo.dwSize.X;
            }
#else
            struct winsize w;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) >= 0)
                cachedWidth = w.ws_col;
#endif
            if (cachedWidth == -1)
                cachedWidth = 80;
        }

        return cachedWidth;
    }

    // memString
    std::string memString(size_t size, bool precise) {
        const char *orders[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB"};
        float value = (float) size;
        
        int i=0;
        for (i = 0; i < 6 && value > 1024.f; ++i)
            value /= 1024.f;
        
        return fmt::format(precise ? "{:.0f}{}" : "{:.3f}{}", value, orders[i]);
    }

NAMESPACE_END(util)


NAMESPACE_BEGIN(string)
    std::string indent(const std::string &string, size_t amount) {
        std::string result;
        result.reserve(string.size());
        for (size_t i = 0; i < string.length(); ++i) {
            char ch = string[i];
            result += ch;
            if (ch == '\n') {
                for (size_t j = 0; j < amount; ++j)
                    result += ' ';
            }
        }
        return result;
    }
NAMESPACE_END(string)



NAMESPACE_END(kazen)