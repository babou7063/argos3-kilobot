#include <kilolib.h>

// Initalisation des variables globales
message_t send_msg;
message_t rcv_msg;
int new_message = 0;
int current_state = 1;
int previous_state = 1;


void setup(){
    // Assigniation de la valeur du message et calcule de son CRC
    send_msg.type = NORMAL;
    send_msg.data[0] = 1; // comment récupérer la valeur qui se trouve dans le fichier XML
    send_msg.crc = message_crc(&send_msg);
}

void loop(){
    
    // Action de random walk ssi on a reçut un nouveau message
    if (new_message == 1){
        
        // Changer le current_state en fct des mess reçut
        SetStateSurrondingRobots(rcv_msg);

        // encore une fois il faut y associer les états ici 
        switch (current_state)
        {
        case 0:
            
            break;
        
        case 1:
            
            break;

        case 2:
        
            break;

        case 3:
            
            break;

        // gestion erreur 
        default:

            break;
        }
    }
    else {
        // si pas de message on continue comme avant
        current_state = previous_state;
    }

}


void SetStateSurrondingRobots(message_t msg){
    
}

RW_Alone(){

}

RW_NF(){
    
}

RW_NE(){
    
}

RW_NFE(){
    
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