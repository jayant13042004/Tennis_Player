#include<windows.h>

struct Render_State {
    int w, h, size;
    void* mem;

    BITMAPINFO bmi;
};

#include "utils.cpp"

global_variable Render_State render_state;
global_variable bool running = true;

#include "input.cpp"
#include "renderer.cpp"
#include "game_engine.cpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    // Create Window class
    const wchar_t CLASS_NAME[] = L"Game Window";

    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = CLASS_NAME;
    window_class.lpfnWndProc = WindowProc;

    // Register Window
    RegisterClass(&window_class);

    // Create Window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        window_class.lpszClassName,                     // Window class
        L"Tennis Game!",    // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 840, 512,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    HDC hdc = GetDC(hwnd);

    Input input = {};

    float delta_time = 0.016666f;
    LARGE_INTEGER frame_begin;
    QueryPerformanceCounter(&frame_begin);

    float perf_freq;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        perf_freq = (float)perf.QuadPart;
    }

    while (running) {
        // Input
        MSG msg;

        for (int i = 0; i < BUTTON_COUNT; i++) {
            input.buttons[i].is_changed = false;
        }

        while (PeekMessageW(&msg, hwnd, 0, 0, PM_REMOVE)) {

            switch (msg.message) {
                case WM_KEYUP:
                case WM_KEYDOWN: {
                    u32 vk_code = (u32)msg.wParam;
                    bool is_down_val = ((msg.lParam & (1 << 31)) == 0);

                    switch (vk_code) {
                        case VK_UP: {
                            input.buttons[BUTTON_UP].is_down = is_down_val; 
                            input.buttons[BUTTON_UP].is_changed = true; 
                        }break;
                        case VK_DOWN: {
                            input.buttons[BUTTON_DOWN].is_down = is_down_val;
                            input.buttons[BUTTON_DOWN].is_changed = true;
                        }break;
                        case 0x57: {
                            input.buttons[BUTTON_W].is_down = is_down_val;
                            input.buttons[BUTTON_W].is_changed = true;
                        }break;
                        case 0x53: {
                            input.buttons[BUTTON_S].is_down = is_down_val;
                            input.buttons[BUTTON_S].is_changed = true;
                        }break;

                    }
                }break;
                default: {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            
        }

        // Simulate
        simulate_game(&input, delta_time);

        // Render
        StretchDIBits(hdc, 0, 0, render_state.w, render_state.h, 0, 0, render_state.w, render_state.h, render_state.mem, &render_state.bmi, DIB_RGB_COLORS, SRCCOPY);
    
        LARGE_INTEGER frame_end;
        QueryPerformanceCounter(&frame_end);
        delta_time = (float)(frame_end.QuadPart - frame_begin.QuadPart) / perf_freq;
        frame_begin = frame_end;
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_CLOSE: {
            running = false;
        }break;
        case WM_DESTROY: {
            running = false;
        }break;
        case WM_SIZE: {
            RECT rec;
            
            GetClientRect(hwnd, &rec);
            
            render_state.w = rec.right - rec.left;
            render_state.h = rec.bottom - rec.top;

            render_state.size = render_state.w * render_state.h * sizeof(unsigned int);

            if (render_state.mem) VirtualFree(render_state.mem, 0, MEM_RELEASE);
            render_state.mem = VirtualAlloc(0, render_state.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            render_state.bmi.bmiHeader.biSize = sizeof(render_state.bmi.bmiHeader);
            render_state.bmi.bmiHeader.biWidth = render_state.w;
            render_state.bmi.bmiHeader.biHeight = render_state.h;
            render_state.bmi.bmiHeader.biPlanes = 1;
            render_state.bmi.bmiHeader.biBitCount = 32;
            render_state.bmi.bmiHeader.biCompression = BI_RGB;

        }break;
        default: {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    return result;
}