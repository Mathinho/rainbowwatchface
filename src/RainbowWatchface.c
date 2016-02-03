#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_wochentag_layer;
static int s_battery_level;
static Layer *s_battery_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_location_layer;
static TextLayer *s_condition_layer;

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_LOCATION 2

static void update_time() {
  setlocale(LC_TIME, ""); //stellt Zeit auf lokale Einstellung um, inklusive Sprache
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);    
}

static void Wochentag() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "Wochentag, 00.00.00";
  
  strftime(buffer, sizeof("Wochentag, 00.00.00"), "%a, %d.%m.%y", tick_time);
  text_layer_set_text(s_wochentag_layer, buffer);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level) * 1.45);
  
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, bounds,2, GCornerNone);
  


  // Draw the background
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, GRect(0, 0, 145, 10), 0, GCornerNone);

  // Draw the bar
  graphics_context_set_fill_color(ctx, GColorIslamicGreen);
  graphics_fill_rect(ctx, GRect(0, 0, width, 10), 0, GCornerNone);
  

}




static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 20, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorDukeBlue);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  
  s_wochentag_layer = text_layer_create(GRect(0, 70, 144, 45));
  text_layer_set_background_color(s_wochentag_layer, GColorYellow);
  text_layer_set_text_color(s_wochentag_layer, GColorDarkGray);
  text_layer_set_font(s_wochentag_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_wochentag_layer, GTextAlignmentCenter);
  
  s_location_layer = text_layer_create(GRect(0, 90, 144, 25));
  text_layer_set_background_color(s_location_layer, GColorGreen);
  text_layer_set_text_color(s_location_layer, GColorWhite);
  text_layer_set_font(s_location_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
  
  // Create battery meter Layer
  s_battery_layer = layer_create(GRect(0, 0, 145, 8));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(0, 115, 144, 30));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorDarkGray);
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "lädt...");
  
  s_condition_layer = text_layer_create(GRect(0, 145, 144, 30));
  text_layer_set_background_color(s_condition_layer, GColorClear);
  text_layer_set_text_color(s_condition_layer, GColorDarkGray);
  text_layer_set_font(s_condition_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_condition_layer, GTextAlignmentCenter);



  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wochentag_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_location_layer));
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_condition_layer));
  

}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_wochentag_layer);
    text_layer_destroy(s_weather_layer);
    text_layer_destroy(s_location_layer);
  

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
// Register with TickTimerService
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();
  Wochentag();
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}

}

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  // Update meter
  layer_mark_dirty(s_battery_layer);

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
static char temperature_buffer[8];
static char conditions_buffer[32];
static char weather_layer_buffer[32];
static char location_buffer[32];
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
  snprintf(temperature_buffer, sizeof(temperature_buffer), "%d °C", (int)t->value->int32);
  break;
case KEY_CONDITIONS:
  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
  break;
case KEY_LOCATION:
      snprintf(location_buffer, sizeof(location_buffer), "%s", t->value->cstring);
      break;
default:
  APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
  break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
// Assemble full string and display
snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
text_layer_set_text(s_weather_layer, weather_layer_buffer);
text_layer_set_text(s_condition_layer, conditions_buffer);
text_layer_set_text(s_location_layer, location_buffer);

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}




static void init() {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  update_time();
  battery_state_service_subscribe(battery_callback);
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  // Register callbacks
app_message_register_inbox_received(inbox_received_callback);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  }

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
    layer_destroy(s_battery_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}