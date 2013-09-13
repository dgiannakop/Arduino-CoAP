#include <CoapSensor.h>

class analogSensor : public CoapSensor {
public:
    int pin, status;

    analogSensor(String name, int pin) : CoapSensor(name) {
        this->pin = pin;
        this->status = analogRead(this->pin); // read the value from the sensor
    }

    void get_value(uint8_t* output_data, size_t* output_data_len) {
        *output_data_len = sprintf((char*) output_data, "%d", this->status);
    }

    void check(void) {
        static unsigned long timestamp = 0;
        if (millis() - timestamp > 500) {
            int newStatus = analogRead(this->pin); // read the value from the sensor
            if (newStatus > this->status + 10 || newStatus < this->status - 10) {
                this->changed = true;
            }
            status = newStatus;
            timestamp = millis();
        }
    }
};
