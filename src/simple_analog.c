#include "simple_analog.h"

#include "pebble.h"

static Window *s_window;
static Layer *s_minute_bg_layer, *s_second_bg_layer, *s_date_layer, *s_hands_layer, *seconds_layer;

static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_num_buffer[4], s_day_buffer[6];
static int s_digits[12];
static int batt;
static GColor background_color;
static GColor time_midground_color;
static GColor date_midground_color;
static GColor foreground_color;
static GColor batt_color;
static bool update_seconds = 0;
static bool always_update_seconds = false;
static int minute;
static int second;
static bool bt_connected;

typedef enum {
  AppKeyBackgroundColor = 0,
  AppKeyTimeMidgroundColor = 1,
  AppKeyDateMidgroundColor = 2,
  AppKeyMainColor = 3,
  AppKeySeconds = 4
} AppKey;

static void bg_sec_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, background_color);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_antialiased(ctx, false);
  
  graphics_context_set_stroke_color(ctx, time_midground_color);
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
  
  if (charge_state.charge_percent > 69) {
    batt_color = GColorGreen;
  } else if (charge_state.charge_percent > 39){
    batt_color = GColorYellow;
  } else {
    batt_color = GColorRed;
  }
  
  batt = charge_state.charge_percent/10;
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int mins = t->tm_min;
 
  graphics_context_set_fill_color(ctx, background_color);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_antialiased(ctx, false);
  
  graphics_context_set_stroke_color(ctx, time_midground_color);
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
  graphics_context_set_stroke_color(ctx, date_midground_color);
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
  graphics_context_set_antialiased(ctx, true);
  
  if (!clock_is_24h_style() ){
    if (hours > 12){
      hours = hours - 12;
    }
  }
  
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

  graphics_context_set_stroke_color(ctx, foreground_color);

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

}

static void seconds_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int secs = t->tm_sec;

  graphics_context_set_antialiased(ctx, false);
  second = secs;

  if(secs < 10) {
    s_digits[8] = 0;
  } else {
    s_digits[8] = secs / 10;
  }
  s_digits[9] = secs % 10;

  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_stroke_color(ctx, foreground_color);

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
  
  GColor center_color = batt_color;
  if (!bt_connected) {
    center_color = GColorWhite;
  }
  
  graphics_context_set_stroke_color(ctx, center_color);
  
  for (int i = 0; i < batt; i ++ ) {
    GPoint p1 = {BATT[i][0], BATT[i][1]};
    GPoint p2 = {BATT[i+1][0], BATT[i+1][1]};
    graphics_draw_line(ctx, p1, p2);
  }
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int mins = t->tm_min;
  
  if( minute != mins || update_seconds || always_update_seconds) {
    minute = mins;
    layer_mark_dirty(seconds_layer);
    layer_mark_dirty(s_hands_layer);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect seconds_bounds = GRect(53, 53, 38, 63);

  s_minute_bg_layer = layer_create(bounds);
  //layer_set_update_proc(s_minute_bg_layer, bg_min_update_proc);
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

static void reset_update_seconds() {
  update_seconds = false;
}

static void handle_bluetooth(bool connected) {
  bt_connected = connected;
  
  update_seconds = true;
  app_timer_register(2000, reset_update_seconds, NULL);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  update_seconds = true;
  app_timer_register(8000, reset_update_seconds, NULL);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  Tuple *bg_color_t = dict_find(iter, AppKeyBackgroundColor);
  if(bg_color_t) {
    background_color = GColorFromHEX(bg_color_t->value->int32);
    persist_write_int(AppKeyBackgroundColor, bg_color_t->value->int32);
  }
  
  Tuple *time_mid_color_t = dict_find(iter, AppKeyTimeMidgroundColor);
  if(time_mid_color_t) {
    time_midground_color = GColorFromHEX(time_mid_color_t->value->int32);
    persist_write_int(AppKeyTimeMidgroundColor, time_mid_color_t->value->int32);
  }
  
  Tuple *date_mid_color_t = dict_find(iter, AppKeyDateMidgroundColor);
  if(date_mid_color_t) {
    date_midground_color = GColorFromHEX(date_mid_color_t->value->int32);
    persist_write_int(AppKeyDateMidgroundColor, date_mid_color_t->value->int32);
  }
  
  Tuple *fore_color_t = dict_find(iter, AppKeyMainColor);
  if(fore_color_t) {
    foreground_color = GColorFromHEX(fore_color_t->value->int32);
    persist_write_int(AppKeyMainColor, fore_color_t->value->int32);
  }
  Tuple *second_tick_t = dict_find(iter, AppKeySeconds);
  if(second_tick_t) {
    always_update_seconds = second_tick_t->value->int32 == 1;
    persist_write_bool(AppKeySeconds, second_tick_t->value->int32 == 1);
  }
    
  update_seconds = true;
  app_timer_register(2000, reset_update_seconds, NULL);
}

static void init() {
  batt_color = GColorGreen;
  if (persist_exists(AppKeyBackgroundColor)) {
    background_color = GColorFromHEX(persist_read_int(AppKeyBackgroundColor));    
  } else {
    background_color = GColorBlack;
  }
  
  if (persist_exists(AppKeyTimeMidgroundColor)) {
    time_midground_color = GColorFromHEX(persist_read_int(AppKeyTimeMidgroundColor));
  } else {
    time_midground_color = GColorDarkGreen;
  }
  
  if (persist_exists(AppKeyDateMidgroundColor)) {
    date_midground_color = GColorFromHEX(persist_read_int(AppKeyDateMidgroundColor));    
  } else {
    date_midground_color = GColorDarkGreen;
  }

  if (persist_exists(AppKeyMainColor)) {
    foreground_color = GColorFromHEX(persist_read_int(AppKeyMainColor));    
  } else {
    foreground_color = GColorWhite;
  }
  
  if ( persist_exists(AppKeySeconds)) {
    always_update_seconds = persist_read_bool(AppKeySeconds);
  } else {
    always_update_seconds = false;
  }
  
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
    
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

  bt_connected = connection_service_peek_pebble_app_connection();

  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bluetooth
  });
  
  battery_state_service_subscribe(handle_battery);
  handle_battery(battery_state_service_peek());
  accel_tap_service_subscribe(tap_handler);
  
  app_message_register_inbox_received(inbox_received_handler);

  app_message_open(64, 64);
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
