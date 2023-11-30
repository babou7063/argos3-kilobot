#include <kilolib.h>

// Definition des etats
#define ALONE 0
#define N_FRIEND 1
#define N_ENENY 2
#define N_FE 3

// Initalisation des variables globales
message_t send_msg;
message_t rcv_msg;
int new_message;
int current_state;
int previous_state;
int kilo_team;


void setup(){
    // Récupération equipe 
    //GetNodeAttributeOrDefault(t_node, "team_kilo", m_nTeamKilo, m_nTeamKilo);
    kilo_team = 1;

    // Assignation des état 
    current_state = ALONE;
    previous_state = ALONE;

    // Assigniation de la valeur du message et calcule de son CRC
    send_msg.type = NORMAL;
    send_msg.data[0] = kilo_team; // comment récupérer la valeur qui se trouve dans le fichier XML
    send_msg.crc = message_crc(&send_msg);
}


void flashing_LED(int red, int green, int blue){
    set_color(RGB(red, green, blue));
    delay(500);
    set_color(RGB(0, 0, 0));
    delay(500);
}

void SetStateSurrondingRobots(message_t msg){
    // robot dans la même team
    if (msg.data[0] == kilo_team){
        // s'il était ALONE, N_FRIEND, N_FE => devient N_FRIEND
        if (current_state == ALONE || current_state == N_FRIEND || current_state == N_FE){
            previous_state = current_state;
            current_state = N_FRIEND;
        }
        // s'il etait N_ENENY => devient N_FE
        else{
            previous_state = current_state;
            current_state = N_FE;
        }
    }
    // robot dans une autre team
    else{
        // s'il était ALONE, N_ENENY alors devient => N_ENENY
        if (current_state == ALONE || current_state == N_ENENY){
            previous_state = current_state;
            current_state = N_ENENY;
        }
        // s'il était N_FRIEND, N_FE devient => N_FE
        else{
            previous_state = current_state;
            current_state = N_FE;
        }
    }
}

uint8_t genertaor_left_value(){
    return kilo_straight_left;
}

uint8_t genertaor_right_value(){
    return kilo_straight_right;
}

void RW_Alone(){
    // il va en avant durant 10cm
    spinup_motors();
    set_motors(genertaor_left_value(), genertaor_right_value());
    delay(1000);
    set_motors(0, 0);
}

void RW_NF(){
    spinup_motors();
    set_motors(genertaor_left_value(), genertaor_right_value());
    delay(250);
    set_motors(0, 0);
}

void RW_NE(){
    spinup_motors();
    set_motors(genertaor_left_value(), genertaor_right_value());
    delay(20000);
    set_motors(0, 0);
}

void RW_NFE(){
    spinup_motors();
    set_motors(genertaor_left_value(), genertaor_right_value());
    delay(500);
    set_motors(0, 0);
}

void loop(){
    
    // check si on a reçut un message
    if (new_message){
        // Changer le current_state en fct des mess reçut
        SetStateSurrondingRobots(rcv_msg);
    }
    else {
        // si pas de message on est seul
        previous_state = current_state;
        current_state = ALONE;
    }

    // Action de random walk
    switch (current_state){
            
            case ALONE:
                RW_Alone();
                flashing_LED(39, 255, 6);
                break;
            
            case N_FRIEND:
                RW_NF();
                flashing_LED(39, 255, 6);
                break;

            case N_ENENY:
                RW_NE();
                flashing_LED(39, 255, 6);
                break;

            case N_FE:
                RW_NFE();
                flashing_LED(39, 255, 6);
                break;

            // gestion erreur 
            default:
                flashing_LED(255, 0, 0);
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