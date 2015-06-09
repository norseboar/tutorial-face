#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

// INITIALIZE WINDOW AND LAYERS =================================================
static void main_window_load(Window *window) {
  // create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // add time layer as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}
static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}

// INITIALIZE SERVICE CALLS ===================================================
static void update_time() {
  // get a tm struct
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // create long-lived buffer to contain time
  static char buffer[] = "00:00";
  
  // write the current hours and minutes into the buffer
  char *time_format;
  if(clock_is_24h_style() == true) {
    time_format = "%H:%M";
  } else {
    time_format = "%I:%M";
  }
  strftime(buffer, sizeof("00:00"), time_format, tick_time);
  
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();  
}

// INIT =======================================================================
static void init() {
  s_main_window = window_create();
  
  // set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // show Window on the watch, with animated set to true
  window_stack_push(s_main_window, true);
  // update time so that it's immediately accurate
  // (otherwise, we need to wait a clock tick)
  update_time();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}