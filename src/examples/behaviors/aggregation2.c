#include <kilolib.h>

// Definition des etats
#define ALONE 0
#define FRIEND_MAJORITY 1
#define ENEMY_MAJORITY 2

// Initalisation des variables globales
message_t send_msg;
message_t rcv_msg;
int new_message;
int current_state;
int previous_state;
int kilo_team;


void setup(){
    // Récupération equipe
    kilo_team = 2;

    // Assignation des état 
    current_state = ALONE;
    previous_state = ALONE;

    // Assigniation de la valeur du message et calcule de son CRC
    send_msg.type = NORMAL;
    send_msg.data[0] = kilo_team; // comment récupérer la valeur qui se trouve dans le fichier XML
    send_msg.crc = message_crc(&send_msg);
}


void flashing_LED(int team){
    set_color(RGB((kilo_team*100)%255, ((kilo_team+1)*100)%255, ((kilo_team+2)*100)%255));
    delay(500);
    set_color(RGB(0, 0, 0));
    delay(500);
}

void SetStateSurrondingRobots(message_t msg){
    // en fonction du tableau qui a été remplit durant le période de réception de message

}

void RW_Alone(){
    
}

void RW_FRIEND_MAJORITY(){
    
}

void RW_ENEMY_MAJORITY(){
    
}

void loop(){
    
    // Changer le current_state en fct des mess reçut
    SetStateSurrondingRobots(rcv_msg);
    
    // Action de random walk
    switch (current_state){
            
            case ALONE:
                flashing_LED(kilo_team);
                RW_Alone();
                break;
            
            case FRIEND_MAJORITY:
                flashing_LED(kilo_team);
                RW_FRIEND_MAJORITY();
                break;

            case ENEMY_MAJORITY:
                flashing_LED(kilo_team);
                RW_ENEMY_MAJORITY();
                break;

            // gestion erreur 
            default:
                flashing_LED(kilo_team);
                break;
        }
}

message_t *message_tx(){
    return &send_msg;
}

void message_rx (message_t *message, distance_measurement_t *distance){
    // on stocke le message qu'on recoit
    rcv_msg = *message;
    // on met a jour le bool
    new_message = 1;
}

int main(){
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    kilo_start(setup, loop);

    return 0;
}