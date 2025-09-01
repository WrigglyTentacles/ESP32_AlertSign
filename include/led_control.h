#include "FastLED.h"

#define NUM_LEDS 35
#define DATA_PIN 13
CRGB leds[NUM_LEDS];
uint8_t max_brightness = 50;
QueueHandle_t rxqueue;


void set_all_leds(uint8_t g,uint8_t b,uint8_t r){
    for (int i=0;i<NUM_LEDS;i++){
      leds[i] = CRGB(g,b,r);
    }
    FastLED.show();
}

void blink_green(uint delayt){
    set_all_leds(100,0,0);
    delay(delayt);
    set_all_leds(0,0,0);
    delay(delayt);
}

void blink_red(uint delayt){
    set_all_leds(0,40,0);
    delay(delayt);
    set_all_leds(0,0,0);
    delay(delayt);
}

void setup_animation(){
    for (int i=0;i<NUM_LEDS;i++){
      leds[i] = CRGB(30,0,0);
      FastLED.show();
      delay(50);
      //leds[i] = CRGB(0,0,0);
      FastLED.show();
    }
    blink_green(200);
    blink_green(200);
    set_all_leds(100,0,0);
    delay(1000);
}


uint8_t color_phaser(uint8_t color, bool up){
  if (up == true){
    switch (color){
      case 30:
        return 15;
      case 0:
        return 15;
      case 15:
        return 30;
    }
  }
  else {
    switch (color){
      case 30:
        return 15;
      case 15:
        return 0;
      case 0:
        return 15;
    }
  }
  return 0;
}

void rainbow_animation(){
  uint8_t thisSpeed=3;
  uint8_t deltaHue=2;
  uint8_t thisHue = beat8(thisSpeed,25);                     // A simple rainbow march.
  fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);            // Use FastLED's fill_rainbow routine.
  FastLED.show();
}

void increment_animation(int loop){
    uint8_t loop_value_to_animation_valueg=((loop+0)%3)*15;
    uint8_t loop_value_to_animation_valuer=((loop+1)%3)*15;
    uint8_t loop_value_to_animation_valueb=((loop+2)%3)*15;
    for (int i=0;i<NUM_LEDS;i++){
      leds[i] = CRGB(color_phaser(loop_value_to_animation_valueg,true),color_phaser(loop_value_to_animation_valuer,true),color_phaser(loop_value_to_animation_valueb,true));
      delay(50);
      FastLED.show();
    }

}

void decrement_animation(int loop){
    uint8_t loop_value_to_animation_valueg=((loop+0)%3)*15;
    uint8_t loop_value_to_animation_valuer=((loop+1)%3)*15;
    uint8_t loop_value_to_animation_valueb=((loop+2)%3)*15;
    for (int i=NUM_LEDS;i>0;i--){
      leds[i] = CRGB(color_phaser(loop_value_to_animation_valueb,false),color_phaser(loop_value_to_animation_valueg,false),color_phaser(loop_value_to_animation_valuer,false));
      FastLED.show();
      delay(200);
    }

}

bool is_collision(uint8_t index1, uint8_t index2){
  if(index1==index2){return true;}
  return false;
}

void decay_led(uint8_t index,uint8_t rate){
  (leds[index].r-rate > 0)?leds[index].r-=rate:leds[index].r=0;
  (leds[index].g-rate > 0)?leds[index].g-=rate:leds[index].g=0;
  (leds[index].b-rate > 0)?leds[index].b-=rate:leds[index].b=0;
}

void decay(uint8_t rate){
    for (int i=0;i<NUM_LEDS;i++){
      decay_led(i,rate);
    }
    FastLED.show();
}

void dots_at_play(){
    uint8_t led1_index = 0;
    uint8_t led2_index = 0;
    for (int i=0;i<NUM_LEDS*2;i++){
      leds[led1_index] = CRGB(0,0,0);
      leds[led2_index] = CRGB(0,0,0);
      led1_index = (led1_index+1)%NUM_LEDS;
      led2_index = (led1_index+(uint8_t(NUM_LEDS)/2))%NUM_LEDS;
      leds[led1_index] = CRGB(0,100,100);
      leds[led2_index] = CRGB(10,10,100);
      FastLED.show();
      delay(20);
    }
}

void dots_at_play_faded(){
    uint8_t led1_index = 0;
    uint8_t led2_index = 0;
    for (int i=0;i<NUM_LEDS*2;i++){
      decay(10);
      led1_index = (led1_index+1)%NUM_LEDS;
      led2_index = (led1_index+(uint8_t(NUM_LEDS)/2))%NUM_LEDS;
      leds[led1_index] = CRGB(0,100,100);
      leds[led2_index] = CRGB(10,10,100);
      FastLED.show();
      delay(20);
    }
}

void chase_the_dot(){
    uint8_t led_index = 0;
    for (int i=0;i<NUM_LEDS*2;i++){
      decay(3);
      led_index = (led_index+1)%NUM_LEDS;
      leds[led_index] = CRGB(0,70,120);
      FastLED.show();
      delay(50);
    }
}

void urgent_status(CRGB led1, CRGB led2){
    uint8_t led1_index = 0;
    uint8_t led2_index = 0;
    for (int i=0;i<NUM_LEDS*2;i++){
      decay(10);
      led1_index = (led1_index+1)%NUM_LEDS;
      led2_index = (led1_index+(uint8_t(NUM_LEDS)/2))%NUM_LEDS;
      leds[led1_index] = led1;
      leds[led2_index] = led2;
      FastLED.show();
      delay(40);
    }
}

void urgent_status_reverse(CRGB led1, CRGB led2){
    int led1_index = NUM_LEDS;
    int led2_index = NUM_LEDS/2;
    for (int i=NUM_LEDS;i>0;i--){
      led1_index = (led1_index-1);
      led2_index = (led2_index-1);
      (led2_index<0)?led2_index=NUM_LEDS:led2_index=led2_index;

      leds[led1_index] = led1;
      leds[led2_index] = led2;

      decay(10);
      FastLED.show();
      delay(40);
    }
}

char rxBuffer[1];
int isGood=0;
void normal_animation(int loop){
  if( xQueueReceive(rxqueue, &(rxBuffer), (TickType_t)5)){
    isGood=rxBuffer[0];
    set_all_leds(255,0,0);//signal received, I really only want this when debugging
    vTaskDelay(500/ portTICK_RATE_MS);
  }
  if ( isGood==0 ) {
    rainbow_animation();
  }
  else if (isGood==1){
    urgent_status(CRGB(50,100,0),CRGB(50,100,0));
  }
  else if (isGood==2){
    urgent_status_reverse(CRGB(0,60,100),CRGB(100,0,100));
  }
  else if (isGood==3){
    chase_the_dot();
  }
  else if (isGood==4){
    dots_at_play();
  }
  else{
    rainbow_animation();
  }
  //rainbow_animation();
  //chase_the_dot();
  //dots_at_play();
}

void ledsetup(QueueHandle_t signal_queue) {
    rxqueue = signal_queue;
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);
    FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

    FastLED.setBrightness(max_brightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); // FastLED power management set at 5V, 500mA
    setup_animation();  //Signal is blink green twice
}

int loop_count = 0;
void anim_loop(void * pvParameters) {
  for(;;){
    normal_animation(loop_count++);
  }
}