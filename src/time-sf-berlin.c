#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#define TITLE "Time SF Berlin"
#define AUTHOR "Stefan Thoss"
	
#define CITY1 "San Francisco"
#define CITY2 "Berlin"
	
#define TIME_DIFFERENCE 9

#define MY_UUID {0xf2, 0x09, 0xc2, 0x9a, 0x14, 0x04, 0x83, 0x19, 0xbb, 0xfb, 0xb1, 0x8c, 0x9a, 0x9a, 0x19, 0x0f}

PBL_APP_INFO(MY_UUID, TITLE, AUTHOR, 1, 0, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_city1_layer;
TextLayer text_city2_layer;
TextLayer text_day_change_layer;
TextLayer text_time1_layer;
TextLayer text_time2_layer;

Layer line_layer;


void line_layer_update_callback(Layer *me, GContext* ctx) {

  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_draw_line(ctx, GPoint(8, 34), GPoint(144-16, 34));

  graphics_draw_line(ctx, GPoint(8, 118), GPoint(144-16, 118));
}


void handle_init(AppContextRef ctx) {

  window_init(&window, TITLE);
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);
	
  // 1st city
  text_layer_init(&text_city1_layer, window.layer.frame);
  text_layer_set_text_color(&text_city1_layer, GColorWhite);
  text_layer_set_background_color(&text_city1_layer, GColorClear);
  layer_set_frame(&text_city1_layer.layer, GRect(10, 4, 144-10, 168-4));
  text_layer_set_font(&text_city1_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_city1_layer.layer);
  text_layer_set_text(&text_city1_layer, CITY1);

  // 1st time
  text_layer_init(&text_time1_layer, window.layer.frame);
  text_layer_set_text_color(&text_time1_layer, GColorWhite);
  text_layer_set_background_color(&text_time1_layer, GColorClear);
  layer_set_frame(&text_time1_layer.layer, GRect(8, 26, 144-8, 168-26));
  text_layer_set_font(&text_time1_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time1_layer.layer);


  // 2nd city (smaller label)
  text_layer_init(&text_city2_layer, window.layer.frame);
  text_layer_set_text_color(&text_city2_layer, GColorWhite);
  text_layer_set_background_color(&text_city2_layer, GColorClear);
  layer_set_frame(&text_city2_layer.layer, GRect(10, 88, 144-34, 168-88));
  text_layer_set_font(&text_city2_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_city2_layer.layer);
  text_layer_set_text(&text_city2_layer, CITY2);

  // day change indicator
  text_layer_init(&text_day_change_layer, window.layer.frame);
  text_layer_set_text_color(&text_day_change_layer, GColorWhite);
  text_layer_set_background_color(&text_day_change_layer, GColorClear);
  layer_set_frame(&text_day_change_layer.layer, GRect(144-34, 88, 34-10, 168-88));
  text_layer_set_font(&text_day_change_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &text_day_change_layer.layer);

  // 2nd time
  text_layer_init(&text_time2_layer, window.layer.frame);
  text_layer_set_text_color(&text_time2_layer, GColorWhite);
  text_layer_set_background_color(&text_time2_layer, GColorClear);
  layer_set_frame(&text_time2_layer.layer, GRect(8, 110, 144-8, 168-110));
  text_layer_set_font(&text_time2_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time2_layer.layer);


  layer_init(&line_layer, window.layer.frame);
  line_layer.update_proc = &line_layer_update_callback;
  layer_add_child(&window.layer, &line_layer);


  // TODO: Update display here to avoid blank display on launch?
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  // Need to be static because they're used by the system later.
  static char time1_text[] = "00:00";
  static char time2_text[] = "00:00";
  static char day_change_text[] = "+1";

  char *time_format;
	
  PblTm tick_time1 = *t->tick_time;
  PblTm tick_time2 = *t->tick_time;
  tick_time2.tm_hour += TIME_DIFFERENCE;

  // Handle 2nd city's date is already tomorrow
  if(tick_time2.tm_hour > 24) {
	tick_time2.tm_hour -= 24;

    // Add day change indicator to city
    text_layer_set_text(&text_day_change_layer, day_change_text);
  }

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time1_text, sizeof(time1_text), time_format, (const PblTm* const) &tick_time1);
  string_format_time(time2_text, sizeof(time2_text), time_format, (const PblTm* const) &tick_time2);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time1_text[0] == '0')) {
    memmove(time1_text, &time1_text[1], sizeof(time1_text) - 1);
  }
  if (!clock_is_24h_style() && (time2_text[0] == '0')) {
    memmove(time2_text, &time2_text[1], sizeof(time2_text) - 1);
  }

  text_layer_set_text(&text_time1_layer, time1_text);
  text_layer_set_text(&text_time2_layer, time2_text);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
