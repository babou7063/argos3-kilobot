#include <kilolib.h>

// Initialisation variable globale
message_t msg;
uint8_t message_sent;

message_t *message_tx(){
    return &msg;
}

void tx_message_success(){
    message_sent = 1;
}

void setup(){
    msg.type = NORMAL;
    msg.data[0] = 1;
    msg.data[1] = 1;
    msg.data[2]  = 1;
    msg.crc = message_crc(&msg);
}

void loop(){
    if (message_sent){
        message_sent = 0;
        set_color(RGB(1, 0, 0));
        delay(100);
        set_color(RGB(0, 0, 0));
    }
}

int main(){
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_tx_success = tx_message_success;

    kilo_start(setup, loop);

    return 0;
}