#include <kilolib.h>

void setup(){

}

void loop(){
    set_motors(0, 0);
}

int  main(){
    kilo_init();
    kilo_start(setup, loop);

    return 0;
}