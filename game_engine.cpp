#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].is_changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].is_changed)

global_variable float player_pos_x = 0.f, player_pos_y = 0.f; // depreceatd speed = 150.f;
global_variable float player_v_y = 0.f;

global_variable float ai_pos_x = 0.f, ai_pos_y = 0.f;
global_variable float ai_v_y = 0.f;

global_variable float ball_pos_x = 0.f, ball_pos_y = 0.f;
global_variable float ball_v_x = 800.f, ball_v_y = 0.f;

global_variable float scale = 1.f;

global_variable int player_score = 0, ai_score = 0;

static void update_score(int score, int id, int x, int y, int size, int half_size, u32 color) {
    while (score) {
        int r = score % 10;
        switch (r) {
            case 0: {
                draw_rect(x - size, y, half_size, 2.5f * size, color);
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                draw_rect(x, y + size * 2.f, half_size, half_size, color);
                draw_rect(x, y - size * 2.f, half_size, half_size, color);
                x -= size * 4.f;
            } break;

            case 1: {
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                x -= size * 2.f;
            } break;

            case 2: {
                draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
                draw_rect(x, y, 1.5f * size, half_size, color);
                draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
                draw_rect(x + size, y + size, half_size, half_size, color);
                draw_rect(x - size, y - size, half_size, half_size, color);
                x -= size * 4.f;
            } break;

            case 3: {
                draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
                draw_rect(x - half_size, y, size, half_size, color);
                draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                x -= size * 4.f;
            } break;

            case 4: {
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                draw_rect(x - size, y + size, half_size, 1.5f * size, color);
                draw_rect(x, y, half_size, half_size, color);
                x -= size * 4.f;
            } break;

            case 5: {
                draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
                draw_rect(x, y, 1.5f * size, half_size, color);
                draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
                draw_rect(x - size, y + size, half_size, half_size, color);
                draw_rect(x + size, y - size, half_size, half_size, color);
                x -= size * 4.f;
            } break;

            case 6: {
                draw_rect(x + half_size, y + size * 2.f, size, half_size, color);
                draw_rect(x + half_size, y, size, half_size, color);
                draw_rect(x + half_size, y - size * 2.f, size, half_size, color);
                draw_rect(x - size, y, half_size, 2.5f * size, color);
                draw_rect(x + size, y - size, half_size, half_size, color);
                x -= size * 4.f;
            } break;

            case 7: {
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
                x -= size * 4.f;
            } break;

            case 8: {
                draw_rect(x - size, y, half_size, 2.5f * size, color);
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                draw_rect(x, y + size * 2.f, half_size, half_size, color);
                draw_rect(x, y - size * 2.f, half_size, half_size, color);
                draw_rect(x, y, half_size, half_size, color);
                x -= size * 4.f;
            } break;

            case 9: {
                draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
                draw_rect(x - half_size, y, size, half_size, color);
                draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
                draw_rect(x + size, y, half_size, 2.5f * size, color);
                draw_rect(x - size, y + size, half_size, half_size, color);
                x -= size * 4.f;
            } break;
        }
        score /= 10;
    }
}

static void simulate_game(struct Input* input, float dt) {
    clear_screen(0xdecea9);

    // init
    player_pos_x = 0.9 * (render_state.w / 2.f);
    ai_pos_x = -0.9 * (render_state.w / 2.f);

    // borders and props
    float border_thickness = 6.f;

        // border
    draw_rect(0.f, (render_state.h / 2.f), (render_state.w / 2.f), border_thickness, 0xff0000);
    draw_rect(0.f, -(render_state.h / 2.f), (render_state.w / 2.f), border_thickness, 0xff0000);
    draw_rect((render_state.w / 2.f), 0.f, border_thickness, (render_state.h / 2.f), 0xff0000);
    draw_rect(-(render_state.w / 2.f), 0.f, border_thickness, (render_state.h / 2.f), 0xff0000);

        // game-net
    draw_rect(0.f, 0.f, 4.f, (render_state.h / 2.f), 0xffffff);

    // physics
    float player_acc_y = 0.f, ai_acc_y = 0.f;

    if (is_down(BUTTON_UP)) player_acc_y += 1100.f;
    if (is_down(BUTTON_DOWN)) player_acc_y -= 1100.f;

    if (is_down(BUTTON_W)) ai_acc_y += 1100.f;
    if (is_down(BUTTON_S)) ai_acc_y -= 1100.f;
        // if (is_down(BUTTON_LEFT)) player_pos_x -= (speed * dt);
        // if (is_down(BUTTON_RIGHT)) player_pos_x += (speed * dt);

    // friction
    player_acc_y -= (player_v_y * 1.11f);
    ai_acc_y -= (ai_v_y * 1.11f);

    // motion equations
        // player
    player_pos_y += ((player_v_y * dt) + (player_acc_y * dt * dt * 0.5));
    player_v_y += (player_acc_y * dt);
        
        // ai
    ai_pos_y += ((ai_v_y * dt) + (ai_acc_y * dt * dt * 0.5));
    ai_v_y += (ai_acc_y * dt);

        // ball
    ball_pos_x += (ball_v_x * dt);
    ball_pos_y += (ball_v_y * dt);
    
    // collision mechanics
        // player
    if (player_pos_y + 75.f > (render_state.h / 2.f) - border_thickness) {
        player_pos_y = (render_state.h / 2.f) - border_thickness - 75.f;
        player_v_y *= -.33f;
    }
    else if (player_pos_y - 75.f < -(render_state.h / 2.f) + border_thickness) {
        player_pos_y = -(render_state.h / 2.f) + border_thickness + 75.f;
        player_v_y *= -.33f;
    }
        // ai
    if (ai_pos_y + 75.f > (render_state.h / 2.f) - border_thickness) {
        ai_pos_y = (render_state.h / 2.f) - border_thickness - 75.f;
        ai_v_y *= -.33f;
    }
    else if (ai_pos_y - 75.f < -(render_state.h / 2.f) + border_thickness) {
        ai_pos_y = -(render_state.h / 2.f) + border_thickness + 75.f;
        ai_v_y *= -.33f;
    }
        // ball
    if ((ball_pos_x + 6.f > player_pos_x - 15.f) && (ball_pos_y + 6.f < player_pos_y + 75.f) && (ball_pos_y - 6.f > player_pos_y - 75.f)) {
        ball_v_x *= -scale;
        ball_v_y = 0.5f * player_v_y;
    }
    else if ((ball_pos_x - 6.f < ai_pos_x + 15.f) && (ball_pos_y + 6.f < ai_pos_y + 75.f) && (ball_pos_y - 6.f > ai_pos_y - 75.f)) {
        ball_v_x *= -scale;
        ball_v_y = 0.5f * ai_v_y;
    }

    if (ball_pos_y + 6.f > (render_state.h / 2.f) - border_thickness) {
        ball_v_y *= -1.f;
    }
    else if (ball_pos_y - 6.f < -(render_state.h / 2.f) + border_thickness) {
        ball_v_y *= -1.f;
    }

    if (ball_pos_x + 6.f > (render_state.w / 2.f) - border_thickness){
        ai_score++;
        ball_pos_x = 0.f;
        ball_pos_y = 0.f;
    }
    else if (ball_pos_x - 6.f < -(render_state.w / 2.f) + border_thickness) {
        player_score++;
        ball_pos_x = 0.f;
        ball_pos_y = 0.f;
    }

    // render updated states
        // player
    draw_rect(player_pos_x, player_pos_y, 15.0, 75.0, 0x00db02);
        
        // ai
    draw_rect(ai_pos_x, ai_pos_y, 15.0, 75.0, 0xff00ff);
        
        // ball
    draw_rect(ball_pos_x, ball_pos_y, 6.f, 6.f, 0x000000);

        // scoreboard
    update_score(player_score, 0, 0.2 * (render_state.w / 2.f), 0.85 * (render_state.h / 2.f), 0.01*(render_state.w / 2.f), 0.005 * (render_state.w /2.f), 0x5130f2);
    update_score(ai_score, 0, -0.2 * (render_state.w / 2.f), 0.85 * (render_state.h / 2.f), 0.01 * (render_state.w / 2.f), 0.005 * (render_state.w / 2.f), 0x5130f2);
}