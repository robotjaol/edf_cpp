#include <Arduino.h>

typedef struct {
    void (*taskFunction)(void *);
    uint32_t period;
    uint32_t deadline;
    uint32_t nextExecutionTime;
} EDFTask;

void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);

EDFTask edfTasks[] = {
    {task1, 1000, 1000, 0},
    {task2, 1500, 1500, 0},
    {task3, 2000, 2000, 0},
};

const int numTasks = sizeof(edfTasks) / sizeof(EDFTask);

void setup() {
    Serial.begin(115200);

    for (int i = 0; i < numTasks; i++) {
        xTaskCreatePinnedToCore(
            edfTasks[i].taskFunction,
            String("Task" + String(i + 1)).c_str(),
            2048,
            (void *)&edfTasks[i],
            1,
            NULL,
            1
        );
    }
}

void loop() {
    uint32_t currentTime = millis();
    EDFTask *nextTask = NULL;
    uint32_t earliestDeadline = UINT32_MAX;

    for (int i = 0; i < numTasks; i++) {
        if (currentTime >= edfTasks[i].nextExecutionTime &&
            edfTasks[i].deadline < earliestDeadline) {
            nextTask = &edfTasks[i];
            earliestDeadline = edfTasks[i].deadline;
        }
    }

    if (nextTask != NULL) {
        nextTask->nextExecutionTime = currentTime + nextTask->period;
        nextTask->taskFunction((void *)nextTask);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}

void task1(void *pvParameters) {
    Serial.println("Task 1 executed");
    vTaskDelay(pdMS_TO_TICKS(100));
}

void task2(void *pvParameters) {
    Serial.println("Task 2 executed");
    vTaskDelay(pdMS_TO_TICKS(150));
}

void task3(void *pvParameters) {
    Serial.println("Task 3 executed");
    vTaskDelay(pdMS_TO_TICKS(200));
}
