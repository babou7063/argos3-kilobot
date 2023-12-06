#include <kilolib.h>

uint8_t recvd_message;
uint8_t message_sent;
message_t msg;
message_t recvd_msg;

message_t *message_tx(){
    return &recvd_msg;
}

void rx_message(message_t *msg, distance_measurement_t *d){
    recvd_msg = *msg;
    recvd_message = 1;
}

void setup(){
    recvd_message = 0;

    msg.type = NORMAL;
    msg.data[0] = recvd_msg.data[0];
    msg.data[1] = recvd_msg.data[1];
    msg.data[2]  = recvd_msg.data[2];
    msg.crc = message_crc(&msg);
}

void loop(){
    if(recvd_message){
        recvd_message = 0;

        // on fait clignoter la led de la couleur du message
        set_color(RGB(recvd_msg.data[0], recvd_msg.data[1], recvd_msg.data[2]));
        delay(100);
        set_color(RGB(0, 0, 0));
    }
}

int main(){
    kilo_init();

    kilo_message_rx = rx_message;
    kilo_message_tx = message_tx;

    kilo_start(setup, loop);

    return 0;
}