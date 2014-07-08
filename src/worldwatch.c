#include "pebble.h"

Window *window;
TextLayer *dub_time_label_layer;
TextLayer *dub_time_layer;

TextLayer *edt_time_label_layer;
TextLayer *edt_time_layer;

TextLayer *cest_time_label_layer;
TextLayer *cest_time_layer;

TextLayer *msk_time_label_layer;
TextLayer *msk_time_layer;

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
	// Need to be static because they're used by the system later.
	static char dub_time_text[] = "00:00";
	static char edt_time_text[] = "00:00";
	static char cest_time_text[] = "00:00";
	static char msk_time_text[] = "00:00";

	char *time_format;
	
	if (clock_is_24h_style()) 
		time_format = "%R";
	else 
		time_format = "%I:%M";

	time_t now = time(NULL);

	tick_time = localtime(&now);
	strftime(dub_time_text, sizeof(dub_time_text), time_format, tick_time);
	text_layer_set_text(dub_time_layer, dub_time_text);
	
	now -= 5 * 3600; // now it is EDT

	tick_time = localtime(&now);
	strftime(edt_time_text, sizeof(edt_time_text), time_format, tick_time);
	text_layer_set_text(edt_time_layer, edt_time_text);

	now += 5 * 3600 + 1 * 3600;

	tick_time = localtime(&now);
	strftime(cest_time_text, sizeof(cest_time_text), time_format, tick_time);
	text_layer_set_text(cest_time_layer, cest_time_text);

	now += 2 * 3600 + (tick_time->tm_isdst ? 0 : 3600); // if there is a DST in Ireland, we are further from Moscow

	tick_time = localtime(&now);
	strftime(msk_time_text, sizeof(msk_time_text), time_format, tick_time);
	text_layer_set_text(msk_time_layer, msk_time_text);
}

void handle_deinit(void)
{
	tick_timer_service_unsubscribe();
}

TextLayer* create_layer(Layer *window_layer, int x, int y, int w, int h, const char *font)
{
	TextLayer *layer = text_layer_create(GRect(x, y, w, h));
	
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_font(layer, fonts_get_system_font (font));

	layer_add_child(window_layer, text_layer_get_layer(layer));

	return layer;
}

void handle_init(void)
{
	window = window_create();
	window_stack_push(window, true /* Animated */ );
	window_set_background_color(window, GColorBlack);

	Layer *window_layer = window_get_root_layer(window);

	// Time layer DUB
	dub_time_label_layer= create_layer(window_layer, 7, 0, 144 - 7, 14, FONT_KEY_GOTHIC_14);
	text_layer_set_text(dub_time_layer, "Dublin");
	dub_time_layer= create_layer(window_layer, 7, 14, 144 - 7, 18, FONT_KEY_GOTHIC_18);

	edt_time_label_layer= create_layer(window_layer, 7, 0+36, 144 - 7, 14, FONT_KEY_GOTHIC_14);
	text_layer_set_text(edt_time_layer, "Bala Cynwyd");
	edt_time_layer= create_layer(window_layer, 7, 14+36, 144 - 7, 18, FONT_KEY_GOTHIC_18);

	cest_time_label_layer= create_layer(window_layer, 7, 0+72, 144 - 7, 14, FONT_KEY_GOTHIC_14);
	text_layer_set_text(cest_time_layer, "Frankfurt");
	cest_time_layer= create_layer(window_layer, 7, 14+72, 144 - 7, 18, FONT_KEY_GOTHIC_18);

	msk_time_label_layer= create_layer(window_layer, 7, 0+108, 144 - 7, 14, FONT_KEY_GOTHIC_14);
	text_layer_set_text(msk_time_layer, "Moscow");
	msk_time_layer= create_layer(window_layer, 7, 14+108, 144 - 7, 18, FONT_KEY_GOTHIC_18);

	// Time callback setup
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	handle_minute_tick(NULL, MINUTE_UNIT);
}

int main(void)
{
	handle_init();

	app_event_loop();

	handle_deinit();
}
