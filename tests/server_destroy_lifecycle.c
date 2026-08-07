#if !defined(_WIN32) && !defined(__APPLE__)
#define _POSIX_C_SOURCE 199309L
#endif

#include "webui.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <unistd.h>
#else
#include <time.h>
#endif

static void sleep_ms(unsigned int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#elif defined(__APPLE__)
    usleep(milliseconds * 1000U);
#else
    struct timespec duration;
    duration.tv_sec = milliseconds / 1000;
    duration.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
    nanosleep(&duration, NULL);
#endif
}

int main(void) {
    const char* html = "<html><body>server lifecycle test</body></html>";

    for (size_t attempt = 0; attempt < 32; ++attempt) {
        size_t window = webui_new_window();
        if (window == 0) {
            fprintf(stderr, "Could not create window on attempt %zu.\n", attempt);
            return EXIT_FAILURE;
        }

        const char* url = webui_start_server(window, html);
        if (url == NULL || url[0] == '\0') {
            fprintf(stderr, "Could not start server on attempt %zu.\n", attempt);
            return EXIT_FAILURE;
        }

        // Alternate between destroying in the startup gap and while the server
        // thread is likely inside its infinite wait loop.
        if ((attempt % 2) != 0)
            sleep_ms(25);

        webui_destroy(window);
    }

    webui_clean();
    puts("server destroy lifecycle test passed");
    return EXIT_SUCCESS;
}
