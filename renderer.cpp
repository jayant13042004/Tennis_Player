static
void render_pixel() {
    u32* pixel = (u32*) render_state.mem;
    for (int i = 0; i < render_state.h; i++) {
        for (int j = 0; j < render_state.w; j++) {
            *pixel++ = 3 * i - 2 * j;
        }
    }
}

static
void clear_screen(u32 color) {
    u32* pixel = (u32*)render_state.mem;
    for (int i = 0; i < render_state.h; i++) {
        for (int j = 0; j < render_state.w; j++) {
            *pixel++ = color;
        }
    }
}

static
void draw_rect_pixels(int x0, int y0, int x1, int y1, u32 color) {
    x0 = clamp(0, x0, render_state.w);
    y0 = clamp(0, y0, render_state.h);
    x1 = clamp(0, x1, render_state.w);
    y1 = clamp(0, y1, render_state.h);

    for (int y = y0; y< y1; y++) {
        u32* pixel = (u32*)render_state.mem + x0 + y * render_state.w;
        for (int x = x0; x < x1; x++) {
            *pixel++ = color;
        }
    }
}

static 
void draw_rect(float x0, float y0, float hw, float hh, u32 color) {
    x0 += (render_state.w / 2.0);
    y0 += (render_state.h / 2.0);

    int xul = clamp(0, x0 - hw, render_state.w);
    int xbr = clamp(0, x0 + hw, render_state.w);
    int yul = clamp(0, y0 - hh, render_state.h);
    int ybr = clamp(0, y0 + hh, render_state.h);

    draw_rect_pixels(xul, yul, xbr, ybr, color);
}
