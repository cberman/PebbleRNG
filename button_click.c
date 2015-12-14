#include <pebble.h>
  
#define TEXT_LENGTH 8

#define COIN 0
#define DIE 1
#define BYTE 2
#define DEC 3
#define HEX 4
  
static Window *window;
static TextLayer *text_layer;
static int format;
static char *text;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  unsigned int r = rand();
  if(rand() % 2 == 1) {
    r |= (unsigned int)RAND_MAX + 1;
  }
  int i;
  switch(format) {
  case COIN:
    if((r & 1) == 0) {
      strncpy(text, "Heads", TEXT_LENGTH);
    } else {
      strncpy(text, "Tails", TEXT_LENGTH);
    }
    break;
  case DIE:
    snprintf(text, 2, "%d", (r % 6) + 1);
    break;
  case BYTE:
    for(i = 0; i < TEXT_LENGTH; i++) {
      text[i] = '0' | (r & 1);
      r >>= 1;
    }
    text[i] = 0;
    break;
  case DEC:
    snprintf(text, TEXT_LENGTH+1, "%u", r);
    break;
  case HEX:
    i = snprintf(text, TEXT_LENGTH+1, "%X", r);
    memset(text, '0', TEXT_LENGTH);
    snprintf(text + TEXT_LENGTH - i, i+1, "%X", r);
    break;
  default:
    text = "NOPE";
  }
  text[TEXT_LENGTH] = 0;
  text_layer_set_text(text_layer, text);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  format++;
  format %= HEX + 1;
  select_click_handler(recognizer, context);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  format += HEX;
  format %= HEX + 1;
  select_click_handler(recognizer, context);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  text = (char*)malloc(TEXT_LENGTH + 1);
  memset(text, 0, TEXT_LENGTH + 1);
  format = HEX;
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 60 }, .size = { bounds.size.w, 40 } });
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(text_layer, text);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  select_click_handler(NULL, NULL);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  free(text);
}

static void init(void) {
  srand(time(NULL));
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
