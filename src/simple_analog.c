#include "simple_analog.h"

#include "pebble.h"

static Window *s_window;
static Layer *s_minute_bg_layer, *s_second_bg_layer, *s_date_layer, *s_hands_layer, *seconds_layer;

static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];
static int s_digits[12];
static int batt;
static GColor bg_color;
static bool second_ticks = 1;
static int minute;
static int second;

typedef enum {
  AppKeyBackgroundColor = 0,
  AppKeySecondTick = 1
} AppKey;

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read color preferences
  Tuple *bg_color_t = dict_find(iter, AppKeyBackgroundColor);
  if(bg_color_t) {
    bg_color = GColorFromHEX(bg_color_t->value->int32);
  }

  // Read boolean preferences
  Tuple *second_tick_t = dict_find(iter, AppKeySecondTick);
  if(second_tick_t) {
    second_ticks = second_tick_t->value->int32 == 1;
  }

  // App should now update to take the user's preferences into account
  //reload_config();
}

static void bg_min_update_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Background Minute Update");
//   graphics_context_set_fill_color(ctx, GColorBlack);
//   graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
//   graphics_context_set_fill_color(ctx, GColorBlack);
//   graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
//   graphics_context_set_stroke_color(ctx, GColorDarkGreen);
//   graphics_context_set_stroke_width(ctx, 3);
//   graphics_context_set_antialiased(ctx, false);
  
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {HOUR_1[NUM_PATHS[8][i]][0], HOUR_1[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {HOUR_1[NUM_PATHS[8][i+1]][0], HOUR_1[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {HOUR_2[NUM_PATHS[8][i]][0], HOUR_2[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {HOUR_2[NUM_PATHS[8][i+1]][0], HOUR_2[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {MINUTE_1[NUM_PATHS[8][i]][0], MINUTE_1[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {MINUTE_1[NUM_PATHS[8][i+1]][0], MINUTE_1[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {MINUTE_2[NUM_PATHS[8][i]][0], MINUTE_2[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {MINUTE_2[NUM_PATHS[8][i+1]][0], MINUTE_2[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {MONTH_1[NUM_PATHS[8][i]][0], MONTH_1[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {MONTH_1[NUM_PATHS[8][i+1]][0], MONTH_1[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {MONTH_2[NUM_PATHS[8][i]][0], MONTH_2[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {MONTH_2[NUM_PATHS[8][i+1]][0], MONTH_2[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {DAY_1[NUM_PATHS[8][i]][0], DAY_1[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {DAY_1[NUM_PATHS[8][i+1]][0], DAY_1[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//   }
//   for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
//     GPoint p1 = {DAY_2[NUM_PATHS[8][i]][0], DAY_2[NUM_PATHS[8][i]][1]};
//     GPoint p2 = {DAY_2[NUM_PATHS[8][i+1]][0], DAY_2[NUM_PATHS[8][i+1]][1]};
//     graphics_draw_line(ctx, p1, p2);
//    }
}

static void bg_sec_update_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Background Second Update");
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_stroke_color(ctx, GColorDarkGreen);
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_antialiased(ctx, false);
  
  for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
    GPoint p1 = {SECOND_1[NUM_PATHS[8][i]][0], SECOND_1[NUM_PATHS[8][i]][1]};
    GPoint p2 = {SECOND_1[NUM_PATHS[8][i+1]][0], SECOND_1[NUM_PATHS[8][i+1]][1]};
    graphics_draw_line(ctx, p1, p2);
  }
  for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
    GPoint p1 = {SECOND_2[NUM_PATHS[8][i]][0], SECOND_2[NUM_PATHS[8][i]][1]};
    GPoint p2 = {SECOND_2[NUM_PATHS[8][i+1]][0], SECOND_2[NUM_PATHS[8][i+1]][1]};
    graphics_draw_line(ctx, p1, p2);
  }
  for (int i = 0; i < 10; i ++ ) {
    GPoint p1 = {BATT[i][0], BATT[i][1]};
    GPoint p2 = {BATT[i+1][0], BATT[i+1][1]};
    graphics_draw_line(ctx, p1, p2);
  }
}
static void handle_battery(BatteryChargeState charge_state) {
  batt = charge_state.charge_percent/10;
}
static void hands_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int mins = t->tm_min;
  
  //if (minute != mins) {
    minute = mins;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Minute Update");
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
    graphics_context_set_stroke_color(ctx, GColorDarkGreen);
    graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_antialiased(ctx, false);
    
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {HOUR_1[NUM_PATHS[8][i]][0], HOUR_1[NUM_PATHS[8][i]][1]};
      GPoint p2 = {HOUR_1[NUM_PATHS[8][i+1]][0], HOUR_1[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {HOUR_2[NUM_PATHS[8][i]][0], HOUR_2[NUM_PATHS[8][i]][1]};
      GPoint p2 = {HOUR_2[NUM_PATHS[8][i+1]][0], HOUR_2[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {MINUTE_1[NUM_PATHS[8][i]][0], MINUTE_1[NUM_PATHS[8][i]][1]};
      GPoint p2 = {MINUTE_1[NUM_PATHS[8][i+1]][0], MINUTE_1[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {MINUTE_2[NUM_PATHS[8][i]][0], MINUTE_2[NUM_PATHS[8][i]][1]};
      GPoint p2 = {MINUTE_2[NUM_PATHS[8][i+1]][0], MINUTE_2[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {MONTH_1[NUM_PATHS[8][i]][0], MONTH_1[NUM_PATHS[8][i]][1]};
      GPoint p2 = {MONTH_1[NUM_PATHS[8][i+1]][0], MONTH_1[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {MONTH_2[NUM_PATHS[8][i]][0], MONTH_2[NUM_PATHS[8][i]][1]};
      GPoint p2 = {MONTH_2[NUM_PATHS[8][i+1]][0], MONTH_2[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {DAY_1[NUM_PATHS[8][i]][0], DAY_1[NUM_PATHS[8][i]][1]};
      GPoint p2 = {DAY_1[NUM_PATHS[8][i+1]][0], DAY_1[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[8] - 1); i ++ ) {
      GPoint p1 = {DAY_2[NUM_PATHS[8][i]][0], DAY_2[NUM_PATHS[8][i]][1]};
      GPoint p2 = {DAY_2[NUM_PATHS[8][i+1]][0], DAY_2[NUM_PATHS[8][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
     }
    
    int month =  t->tm_mon + 1;
    int day = t->tm_mday;
    int hours = t->tm_hour;
  //   GRect bounds = layer_get_bounds(layer);
    graphics_context_set_antialiased(ctx, true);
  
    if(hours < 10) {
      s_digits[0] = 0;
    } else {
      s_digits[0] = hours / 10;
    }
    s_digits[1] = hours % 10;
  
    if(mins < 10) {
      s_digits[2] = 0;
    } else {
      s_digits[2] = mins / 10;
    }
    s_digits[3] = mins % 10;
  
    if(month < 10) {
      s_digits[4] = 0;
    } else {
      s_digits[4] = month / 10;
    }
    s_digits[5] = month % 10;
    
    if(day < 10) {
      s_digits[6] = 0;
    } else {
      s_digits[6] = day / 10;
    }
    s_digits[7] = day % 10;
    
//    graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    for (int i = 0; i < (NUM_POINTS[s_digits[0]] - 1); i ++ ) {
      GPoint p1 = {HOUR_1[NUM_PATHS[s_digits[0]][i]][0], HOUR_1[NUM_PATHS[s_digits[0]][i]][1]};
      GPoint p2 = {HOUR_1[NUM_PATHS[s_digits[0]][i+1]][0], HOUR_1[NUM_PATHS[s_digits[0]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[1]] - 1); i ++ ) {
      GPoint p1 = {HOUR_2[NUM_PATHS[s_digits[1]][i]][0], HOUR_2[NUM_PATHS[s_digits[1]][i]][1]};
      GPoint p2 = {HOUR_2[NUM_PATHS[s_digits[1]][i+1]][0], HOUR_2[NUM_PATHS[s_digits[1]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[2]] - 1); i ++ ) {
      GPoint p1 = {MINUTE_1[NUM_PATHS[s_digits[2]][i]][0], MINUTE_1[NUM_PATHS[s_digits[2]][i]][1]};
      GPoint p2 = {MINUTE_1[NUM_PATHS[s_digits[2]][i+1]][0], MINUTE_1[NUM_PATHS[s_digits[2]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[3]] - 1); i ++ ) {
      GPoint p1 = {MINUTE_2[NUM_PATHS[s_digits[3]][i]][0], MINUTE_2[NUM_PATHS[s_digits[3]][i]][1]};
      GPoint p2 = {MINUTE_2[NUM_PATHS[s_digits[3]][i+1]][0], MINUTE_2[NUM_PATHS[s_digits[3]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[4]] - 1); i ++ ) {
      GPoint p1 = {MONTH_1[NUM_PATHS[s_digits[4]][i]][0], MONTH_1[NUM_PATHS[s_digits[4]][i]][1]};
      GPoint p2 = {MONTH_1[NUM_PATHS[s_digits[4]][i+1]][0], MONTH_1[NUM_PATHS[s_digits[4]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[5]] - 1); i ++ ) {
      GPoint p1 = {MONTH_2[NUM_PATHS[s_digits[5]][i]][0], MONTH_2[NUM_PATHS[s_digits[5]][i]][1]};
      GPoint p2 = {MONTH_2[NUM_PATHS[s_digits[5]][i+1]][0], MONTH_2[NUM_PATHS[s_digits[5]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[6]] - 1); i ++ ) {
      GPoint p1 = {DAY_1[NUM_PATHS[s_digits[6]][i]][0], DAY_1[NUM_PATHS[s_digits[6]][i]][1]};
      GPoint p2 = {DAY_1[NUM_PATHS[s_digits[6]][i+1]][0], DAY_1[NUM_PATHS[s_digits[6]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[7]] - 1); i ++ ) {
      GPoint p1 = {DAY_2[NUM_PATHS[s_digits[7]][i]][0], DAY_2[NUM_PATHS[s_digits[7]][i]][1]};
      GPoint p2 = {DAY_2[NUM_PATHS[s_digits[7]][i+1]][0], DAY_2[NUM_PATHS[s_digits[7]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
 // }
}

static void seconds_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int secs = t->tm_sec;

//  if (true != true) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_antialiased(ctx, false);
    second = secs;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Seconds Update");
    if(secs < 10) {
      s_digits[8] = 0;
    } else {
      s_digits[8] = secs / 10;
    }
    s_digits[9] = secs % 10;
    
    graphics_context_set_stroke_width(ctx, 3);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    
    for (int i = 0; i < (NUM_POINTS[s_digits[8]] - 1); i ++ ) {
      GPoint p1 = {SECOND_1[NUM_PATHS[s_digits[8]][i]][0], SECOND_1[NUM_PATHS[s_digits[8]][i]][1]};
      GPoint p2 = {SECOND_1[NUM_PATHS[s_digits[8]][i+1]][0], SECOND_1[NUM_PATHS[s_digits[8]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < (NUM_POINTS[s_digits[9]] - 1); i ++ ) {
      GPoint p1 = {SECOND_2[NUM_PATHS[s_digits[9]][i]][0], SECOND_2[NUM_PATHS[s_digits[9]][i]][1]};
      GPoint p2 = {SECOND_2[NUM_PATHS[s_digits[9]][i+1]][0], SECOND_2[NUM_PATHS[s_digits[9]][i+1]][1]};
      graphics_draw_line(ctx, p1, p2);
    }
    for (int i = 0; i < batt; i ++ ) {
      GPoint p1 = {BATT[i][0], BATT[i][1]};
      GPoint p2 = {BATT[i+1][0], BATT[i+1][1]};
      graphics_draw_line(ctx, p1, p2);
    }
//  }
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(seconds_layer);//CHANGE THIS?
  layer_mark_dirty(s_hands_layer);
}

// static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
//   layer_mark_dirty(s_hands_layer);//CHANGE THIS?
// }

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect seconds_bounds = GRect(53, 53, 38, 63);

  s_minute_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_minute_bg_layer, bg_min_update_proc);
  layer_add_child(window_layer, s_minute_bg_layer);

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
  
  s_second_bg_layer = layer_create(seconds_bounds);
  layer_set_update_proc(s_second_bg_layer, bg_sec_update_proc);
  layer_add_child(window_layer, s_second_bg_layer);

  seconds_layer = layer_create(seconds_bounds);
  layer_set_update_proc(seconds_layer, seconds_update_proc);
  layer_add_child(window_layer, seconds_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_minute_bg_layer);
  layer_destroy(seconds_layer);
  layer_destroy(s_hands_layer);
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
    
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
 // tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  
  battery_state_service_subscribe(handle_battery);
  handle_battery(battery_state_service_peek());
  
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
