#include <pebble.h>
  
#define KEY_TEMPERATURE 1
#define KEY_ICON 2
#define KEY_SCALE 3
#define KEY_TEST 4
  
static Window *window;
static TextLayer *s_time_layer;
TextLayer *text_date_layer;

time_t auto_hide;

static bool initiate_watchface = true;
static bool refresh_weather = false;

static TextLayer *s_weather_layer;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

const int BT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_BLUETOOTH
};

static BitmapLayer *s_icon_layer;
static GBitmap *s_icon_bitmap = NULL;

static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMAGE_SUN, //0
  RESOURCE_ID_IMAGE_CLOUD, //1
  RESOURCE_ID_IMAGE_RAIN, //2
  RESOURCE_ID_IMAGE_SNOW, //3
  RESOURCE_ID_IMAGE_STORM //4 
};

const int BATTERY_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_BATTERY_20,
  RESOURCE_ID_BATTERY_40,
  RESOURCE_ID_BATTERY_60,
  RESOURCE_ID_BATTERY_80,
  RESOURCE_ID_BATTERY_100
};

//Random Number Setup
int NUM_MUG_SHOTS = 15;
int seed_mugs2;
int start_number_mug, random_mug;

const int MUG_SHOTS_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_DOG_ONE, //0
  RESOURCE_ID_DOG_TWO, //1
  RESOURCE_ID_DOG_THREE, //2
  RESOURCE_ID_DOG_FOUR, //3
  RESOURCE_ID_DOG_FIVE, //4
  RESOURCE_ID_DOG_SIX, //5 
  RESOURCE_ID_DUCK_ONE, //6
  RESOURCE_ID_DUCK_TWO, //7
  RESOURCE_ID_DUCK_THREE, //8
  RESOURCE_ID_DUCK_FOUR, //9
  RESOURCE_ID_DUCK_FIVE, //10
  RESOURCE_ID_DUCK_SIX, //11
  RESOURCE_ID_DUCK_SEVEN, //12
  RESOURCE_ID_DUCK_EIGHT, //13
  RESOURCE_ID_DUCK_NINE //14
};

#define TOTAL_BATTERY 5
static GBitmap *battery_images[TOTAL_BATTERY];
static BitmapLayer *battery_layers[TOTAL_BATTERY];

#define TOTAL_BT_DIGITS 1
static GBitmap *bt_digits_images[TOTAL_BT_DIGITS];
static BitmapLayer *bt_digits_layers[TOTAL_BT_DIGITS];

#define TOTAL_mug_amount 2
static GBitmap *mug_amount_images[TOTAL_mug_amount];
static BitmapLayer *mug_amount_layers[TOTAL_mug_amount];

//Bitmap Container
static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
GBitmap *old_image = *bmp_image;

 	*bmp_image = gbitmap_create_with_resource(resource_id);
 	GRect frame = (GRect) {
   	.origin = origin,
    
#ifdef PBL_COLOR
    .size = gbitmap_get_bounds(*bmp_image).size 
#else
     .size = (*bmp_image)->bounds.size 
#endif
    
      
};
 	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
 	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

 	if (old_image != NULL) {
 	gbitmap_destroy(old_image);
}
}

// Bluetooth Connectivity
static void handle_bluetooth(bool connected) {
if (connected) {
// Normal Mode 
if (initiate_watchface) {
   	layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), true);
}
// On disconnection vibrate twice
else {
    layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), true);
vibes_double_pulse();
}
}
else {
  
// If started in disconnection display image, no vibration
if (initiate_watchface) {
  layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), false);
set_container_image(&bt_digits_images[0], bt_digits_layers[0], BT_IMAGE_RESOURCE_IDS[0], GPoint(36, 60));
}
  
// On disconnection display image and vibrate three times
else {
layer_set_hidden(bitmap_layer_get_layer(bt_digits_layers[0]), false);
set_container_image(&bt_digits_images[0], bt_digits_layers[0], BT_IMAGE_RESOURCE_IDS[0], GPoint(36, 60));


vibes_enqueue_custom_pattern( (VibePattern) {
   	.durations = (uint32_t []) {100, 100, 100, 100, 100},
   	.num_segments = 5
} );
}}}

 static void update_display(struct tm *current_time) {
 	if( ((current_time->tm_min == 0) && (current_time->tm_sec == 0)) || (initiate_watchface == true) ){ 

  if (initiate_watchface){
  start_number_mug = (current_time->tm_sec) + (current_time->tm_min) + NUM_MUG_SHOTS;
}
    
  static long seed_mugs = 100;
  seed_mugs  = (((seed_mugs * 214013L + 2531011L) >> 16) & 32767);
  seed_mugs2 = seed_mugs + start_number_mug;
  random_mug = (seed_mugs2 % NUM_MUG_SHOTS);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "random character generated [#%d].", random_mug);

set_container_image(&mug_amount_images[1], mug_amount_layers[1], MUG_SHOTS_IMAGE_RESOURCE_IDS[random_mug], GPoint(0, 0));
    
}
}


//--BATTERY
static uint8_t battery_level;
static Layer *battery_layer;

//--BATTERY LAYER
void battery_layer_update_callback(Layer *layer, GContext *ctx) {
  
    if (battery_level > 80) {
    set_container_image(&battery_images[0], battery_layers[0], BATTERY_IMAGE_RESOURCE_IDS[4], GPoint(98, 148));

      }
  	else if (battery_level > 60 && battery_level <= 80) {
    set_container_image(&battery_images[0], battery_layers[0], BATTERY_IMAGE_RESOURCE_IDS[3], GPoint(98, 148));

      }
      else if (battery_level > 40 && battery_level <= 60 ){
    set_container_image(&battery_images[0], battery_layers[0], BATTERY_IMAGE_RESOURCE_IDS[2], GPoint(98, 148));
      
    }
      else if (battery_level > 20 && battery_level <= 40 ){
    set_container_image(&battery_images[0], battery_layers[0], BATTERY_IMAGE_RESOURCE_IDS[1], GPoint(98, 148));
      
    }
      else if (battery_level <= 20){
    set_container_image(&battery_images[0], battery_layers[0], BATTERY_IMAGE_RESOURCE_IDS[0], GPoint(98, 148));
      
    }
  	else {	
 	}
  
}

//--HANDLE BATTERY
void battery_state_handler(BatteryChargeState charge) {
	battery_level = charge.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char date_text[] = "00.00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  strftime(date_text, sizeof(date_text), "%m.%d", tick_time);
  text_layer_set_text(text_date_layer, date_text);
  
  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void update_bg_color(struct tm *current_time) {
  if (current_time->tm_hour >= 12 && current_time->tm_hour < 17){
    //Day
  window_set_background_color(window, GColorFromRGB(0,170,255));
  }
  else if (current_time->tm_hour >= 5 && current_time->tm_hour < 12){
    //Morning
    window_set_background_color(window, GColorFromRGB(255,255,0));
  } 
  else if (current_time->tm_hour >= 17 && current_time->tm_hour < 21){
    //Evening
    window_set_background_color(window, GColorFromRGB(255,170,0));
  } 
  else if (current_time->tm_hour >= 21 || current_time->tm_hour < 5){
    //Night
    window_set_background_color(window, GColorFromRGB(0,0,85));
  } 
}

static void update_bg_image(struct tm *current_time){
  
  if(current_time->tm_mon >=12 && current_time->tm_mon < 3){
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Winter);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }
  else if(current_time->tm_mon >=3 && current_time->tm_mon < 6){
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Spring);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }
   else if(current_time->tm_mon >=6 && current_time->tm_mon < 9){
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Summer);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }
   else if(current_time->tm_mon >=9 && current_time->tm_mon < 12){
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_Fall);
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  }
  
}


static void handle_tap(AccelAxisType axis, int32_t direction)
{
  
   refresh_weather = true;
  
   //Show date for 5 seconds, then hide
   auto_hide = time(NULL) + 5;
  
  //Hide
   layer_set_hidden(bitmap_layer_get_layer(battery_layers[0]), true);

 //Show
    layer_set_hidden(text_layer_get_layer(text_date_layer), false);
  
}

static void main_window_load(Window *window) {
  
  
  //Create BitmapLayer
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  
  //Create Icon Layer
  s_icon_layer = bitmap_layer_create(GRect(80, 0, 80, 80));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_icon_layer));
  bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
  
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 139, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(-47, 139, 139, 50));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text(s_weather_layer, "...");
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  
  
   //BATTERY
 	BatteryChargeState initial = battery_state_service_peek();
 	battery_level = initial.charge_percent;
 	battery_layer = layer_create(GRect(0,0,144,168));
 	layer_set_update_proc(battery_layer, &battery_layer_update_callback);
  
  // Create DateLayer
  text_date_layer = text_layer_create(GRect(51, 139, 139, 50));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_set_hidden(text_layer_get_layer(text_date_layer), true);
  
  //Apply to TextLayer
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
	layer_add_child(window_get_root_layer(window), battery_layer);
  
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(text_date_layer);
  
  for (int i = 0; i < TOTAL_mug_amount; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(mug_amount_layers[i]));
   	gbitmap_destroy(mug_amount_images[i]);
   	bitmap_layer_destroy(mug_amount_layers[i]);
}
  
  for (int i = 0; i < TOTAL_BATTERY; i++) {
   	layer_remove_from_parent(bitmap_layer_get_layer(battery_layers[i]));
   	gbitmap_destroy(battery_images[i]);
   	bitmap_layer_destroy(battery_layers[i]);
}
  
  for (int i = 0; i < TOTAL_BT_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(bt_digits_layers[i]));
    gbitmap_destroy(bt_digits_images[i]);
    bitmap_layer_destroy(bt_digits_layers[i]);
 	}
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_bg_color(tick_time);  
  update_display(tick_time);
  

  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0 || (refresh_weather == true)) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();

  }
  
  if(time(NULL) == auto_hide - 4){
    refresh_weather = false;
  }
  
  if(time(NULL) == auto_hide){
  
  //Show
  layer_set_hidden(bitmap_layer_get_layer(battery_layers[0]), false);

 //Hide
    layer_set_hidden(text_layer_get_layer(text_date_layer), true);
  }
  
  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  int temperature;
  int Kelvin = persist_read_int(KEY_TEMPERATURE);
  int finalTemp = Kelvin;
  int test = persist_read_int(KEY_TEST);
    
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_SCALE:
      if(strcmp(t->value->cstring, "F") == 0){
        persist_write_int(KEY_TEST, 0);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      else if(strcmp(t->value->cstring, "C") == 0){
        persist_write_int(KEY_TEST, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
      }
      break;
    case KEY_TEMPERATURE:
      if(test == 0){
      temperature = (int)t->value->int32;
      persist_write_int(KEY_TEMPERATURE, temperature);
      Kelvin = persist_read_int(KEY_TEMPERATURE);
      finalTemp = (Kelvin - 273.15) * 1.8 + 32;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      else if(test == 1){
      temperature = (int)t->value->int32;
      persist_write_int(KEY_TEMPERATURE, temperature);
      Kelvin = persist_read_int(KEY_TEMPERATURE);
      finalTemp = Kelvin - 273.15;
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", finalTemp);
      }
      break;
      
      case KEY_ICON:
      if (s_icon_bitmap) {
        gbitmap_destroy(s_icon_bitmap);
      }

      s_icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[t->value->int32]);
      bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
      break;

    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
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
  // Create main Window element and assign to pointer
  window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
   Layer *window_layer = window_get_root_layer(window);
  GRect dummy_frame = { {0, 0}, {0, 0} };
  
  //Create character layer
  for (int i = 0; i < TOTAL_mug_amount; ++i) {
   	mug_amount_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(mug_amount_layers[i]));
    bitmap_layer_set_compositing_mode(mug_amount_layers[i], GCompOpSet);
}
  
   //Create Battery layer
 	for (int i = 0; i < TOTAL_BATTERY; ++i) {
   	battery_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(battery_layers[i]));
}
  
   //Create Bluetooth layer
 	for (int i = 0; i < TOTAL_BT_DIGITS; ++i) {
   	bt_digits_layers[i] = bitmap_layer_create(dummy_frame);
   	layer_add_child(window_layer, bitmap_layer_get_layer(bt_digits_layers[i]));
}
    
  // Avoids a blank screen on watch start.
 	time_t now = time(NULL);
 	struct tm *tick_time = localtime(&now);
    
  update_display(tick_time);
    
  initiate_watchface = false;
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  
  //Subscribe to Acceleromter
  accel_tap_service_subscribe(&handle_tap);
  
    // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum()); 

  handle_bluetooth(bluetooth_connection_service_peek());
  
  update_bg_color(tick_time);
  update_bg_image(tick_time);
  
}

static void deinit() {
  // Destroy Window
  window_destroy(window);
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
}

int main(void) {
  init();
  battery_state_service_subscribe (&battery_state_handler);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  app_event_loop();
  deinit();
}
