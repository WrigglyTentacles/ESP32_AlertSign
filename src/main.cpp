#include <led_control.h>
#include <ota_control.h>

TaskHandle_t Task1;
TaskHandle_t Task2;
QueueHandle_t signal_queue;

void setup(){
  Serial.begin(9600);
  int txBuffer[5];
  signal_queue = xQueueCreate( 2, sizeof(int) ); // create a queue
  if (signal_queue == 0){
    Serial.println("Failed to create queue");
  }
  ledsetup(signal_queue);
  ota_setup(signal_queue);
    //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    anim_loop,   /* Task function. */
                    "Led Control Loop",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    ota_loop,   /* Task function. */
                    "Wifi Control Loop",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

void loop (){

} 