#include <kilolib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Definition des etats
#define ALONE 0
#define FRIEND_MAJORITY 1
#define ENNEMY_MAJORITY 2

// Initalisation des variables globales
message_t send_msg;
message_t rcv_msg;
int new_message;
int current_state;
int previous_state;
int kilo_team;
int neighbors_count[2]; // neighbors_count[0] = le nombre d'ami, neighbors_count[1] = le nombre d'ennemi, peu importe son équipe

void setup(){
    // Récupération equipe
    kilo_team = 1;

    // Assignation des état 
    current_state = ALONE;
    previous_state = ALONE;

    // Assigniation de la valeur du message et calcule de son CRC
    send_msg.type = NORMAL;
    send_msg.data[0] = kilo_team; 
    send_msg.crc = message_crc(&send_msg);

    // Création du tableau de nombre de voisin
    neighbors_count[0] = 0;
    neighbors_count[1] = 0;
}


void flashing_LED(int team){
    set_color(RGB((kilo_team*100)%255, ((kilo_team+1)*100)%255, ((kilo_team+2)*100)%255));
    delay(500);
    set_color(RGB(0, 0, 0));
    delay(500);
}

void SetStateSurrondingRobots(){
    // modification de l'état du kb

    // ALONE
    if (neighbors_count[0] == 0 && neighbors_count[1] == 0) {

        current_state = ALONE;
    }
    // FRIEND
    else if (neighbors_count[0] > neighbors_count[1]) {
        
        current_state = FRIEND_MAJORITY;
    }
    // ENNEMY => choix arbitraire : s'ils sont le même nombre il sera en mode exploration
    else if (neighbors_count[0] <= neighbors_count[1]) {

        current_state = ENNEMY_MAJORITY;
    }
}

void RW_Alone(){
    
}

void RW_FRIEND_MAJORITY(){
    
}

void RW_ENNEMY_MAJORITY(){
    
}

void loop(){
    
    // Changer le current_state en fct des kb croisés
    SetStateSurrondingRobots();
    
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

            case ENNEMY_MAJORITY:
                flashing_LED(kilo_team);
                RW_ENNEMY_MAJORITY();
                break;

            // gestion erreur 
            default:
                flashing_LED(RGB(1,0,0)); // le rouge représente une erreur
                break;
        }
}

message_t *message_tx(){
    return &send_msg;
}

void message_rx (message_t *message, distance_measurement_t *distance){
    // on stocke le message qu'on recoit
    rcv_msg = *message;

    // mise à jour du tableau de compte
    if (rcv_msg.data == kilo_team) {
        neighbors_count[0] ++; // on ajoute un ami
    }
    else {
        neighbors_count[1] ++; // on ajoute un ennemi
    }
    
    // on met a jour le bool
    new_message = 1;
}

/*---------------------------------------------------------------------*/
/*     Fonction d'implémentation de distribution de Cauchy et Levy     */
/*                       @author cdimidov                              */
/*---------------------------------------------------------------------*/

double uniform_distribution(double a, double b)
{

  return (rand() * (b - a) + a) / ((double)RAND_MAX + 1);
}

double wrapped_cauchy_ppf(const double c)
{
  double val, theta, u, q;
  q = 0.5;
  u = uniform_distribution(0.0, 1.0);
  val = (1.0 - c) / (1.0 + c);
  theta = 2 * atan(val * tan(M_PI * (u - q)));
  return theta;
}

double exponential_distribution(double lambda)
{
  double u, x;
  u = uniform_distribution(0.0, 1.0);
  x = (-lambda) * log(1 - u);
  return (x);
}

/* The stable Levy probability distributions have the form

   p(x) dx = (1/(2 pi)) \int dt exp(- it x - |c t|^alpha)

   with 0 < alpha <= 2. 

   For alpha = 1, we get the Cauchy distribution
   For alpha = 2, we get the Gaussian distribution with sigma = sqrt(2) c.

   */

int levy(const double c, const double alpha)
{
  double u, v, t, s;

  u = M_PI * (uniform_distribution(0.0, 1.0) - 0.5); /*vedi uniform distribution */

  if (alpha == 1) /* cauchy case */
  {
    t = tan(u);
    return (int)(c * t);
  }

  do
  {
    v = exponential_distribution(1.0); /*vedi esponential distribution */
  } while (v == 0);

  if (alpha == 2) /* gaussian case */
  {
    t = 2 * sin(u) * sqrt(v);
    return (int)(c * t);
  }

  /* general case */

  t = sin(alpha * u) / pow(cos(u), 1 / alpha);
  s = pow(cos((1 - alpha) * u) / v, (1 - alpha) / alpha);

  return (int)(c * t * s);
}

/*----------------------------------------------------------------------*/
/*                          Main function                               */
/*                        @author babou7063                             */
/*----------------------------------------------------------------------*/
int main(){
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    kilo_start(setup, loop);

    return 0;
}