#include "mbed.h"
#include <iostream>

// Define pins for incoming customers and fire alarm button
#define ADULT p5
#define CHILD p6
#define FIRE p7

// Define pins for barbers
#define ADULT_BARBER1 p8
#define ADULT_BARBER2 p9
#define CHILD_BARBER p10

// Define pin for fire alarm
#define FIRE_ALARM p11

// Define waiting list pins
#define WAITING_SEAT1 p12
#define WAITING_SEAT2 p13
#define WAITING_SEAT3 p14
#define WAITING_SEAT4 p15
#define WAITING_SEAT5 p16
#define WAITING_SEAT6 p17
#define WAITING_SEAT7 p18
#define WAITING_SEAT8 p19

using namespace std;

InterruptIn adult_button(ADULT);
InterruptIn child_button(CHILD);
InterruptIn fire_button(FIRE);

BusOut adult_barber(ADULT_BARBER1, ADULT_BARBER2);
DigitalOut child_barber(CHILD_BARBER);

DigitalOut fire_alarm(FIRE_ALARM);

BusOut waiting_seats(WAITING_SEAT1,WAITING_SEAT2,WAITING_SEAT3,WAITING_SEAT4,WAITING_SEAT5,WAITING_SEAT6,WAITING_SEAT7,WAITING_SEAT8);

char waiting_identities[8];

bool check_space(int k){
    
    if(fire_alarm == 1){
        cout << "Fire in the building, NO ENTRY!" << endl;
        return false;
    }
    
    int cnt = 0;
    for(int i = 0; i < 8; i++){
        if(waiting_seats[i] == 0){
            cnt++;
        }
    }    
    if(k > cnt){
        cout << "No seat available now please wait 12 minutes" << endl;
        return false;
    }
    
    return true;
}

void move_forward(int k){
    for(int i = k; i < 7; i++){
        waiting_seats[i] = waiting_seats[i+1];
        waiting_identities[i] = waiting_identities[i+1];
    }
    waiting_seats[7] = 0;
    waiting_identities[7] = 'N';
}

void adult_done(){
    if(adult_barber[0] == 1){
        adult_barber[0] = 0;
        cout << "Barber 1 for adult free now" << endl;

    }
    
    else if(adult_barber[1] == 1){
        adult_barber[1] = 0;
        cout << "Barber 2 for adult free now" << endl;

    }
}

void start_adult_1(){
    if(adult_barber[0] == 0){
        for(int i = 0; i < 8; i++){
            if(waiting_identities[i] == 'A'){
                adult_barber[0] = 1;
                move_forward(i);
                break;
            }
        }
    }
    
    else{
        cout << "Barber 1 for adult is busy now" << endl;
    }
    
}

void start_adult_2(){
    if(adult_barber[1] == 0){
        for(int i = 0; i < 8; i++){
            if(waiting_identities[i] == 'A'){
                adult_barber[1] = 1;
                move_forward(i);
                break;
            }
        }
    }
    else{
        cout << "Barber 2 for adult is busy now" << endl;
    }
}

void child_done(){
    for(int i = 0; i < 8; i++){
        if(waiting_identities[i] == 'P'){
            move_forward(i);
            break;
        }
    }
    child_barber = 0;
}

void start_child(){
    if(child_barber == 0){
        for(int i = 0; i < 8; i++){
            if(waiting_identities[i] == 'C'){
                child_barber = 1;
                move_forward(i);
                break;
            }
        }
    }
    else{
        cout << "Barber for child is busy now" << endl;
    }
}



void add_adult(){
    if(check_space(1) == true){
        for(int i = 0; i < 8; i++){
            if(waiting_seats[i] == 0){
                waiting_identities[i] = 'A';
                waiting_seats[i] = 1;
                cout << "Added to seat: " << i << endl;
                break;
            }
        }
    }

}

void add_child(){
    if(check_space(2) == true){
        for(int i = 0; i < 7; i++){
            if(waiting_seats[i] == 0 && waiting_seats[i+1] == 0){
                waiting_identities[i] = 'C';
                waiting_seats[i] = 1;
                waiting_identities[i+1] = 'P';
                waiting_seats[i+1] = 1;
                
                cout << "Added child and one parent to seat: " << i << " & " << i+1 << endl;
                break;
            }
        }
    }
}

void fire(){
    for(int i = 0; i < 8; i++){
        waiting_identities[i] = 'N';
    }
    adult_barber = 0b00;
    child_barber = 0;
    waiting_seats = 0b00000000;
    fire_alarm = !fire_alarm;
}


int main() {
    adult_barber = 0b00;
    child_barber = 0;
    fire_alarm = 0;
    waiting_seats = 0b00000000;
    int timer1, timer2, timer3;
    timer1 = 12;
    timer2 = 12;
    timer3 = 12;
    
    adult_button.rise(&add_adult);
    child_button.rise(&add_child);
    fire_button.rise(&fire);
    
    while (1) {
        if(fire_alarm == 0){
            
        
            start_adult_1();
            timer1--;
            if(timer1 == 0){
                adult_done();
                timer1 = 12;
            }
            
            start_adult_2();
            timer2--;
            if(timer2 == 0){
                adult_done();
                timer2 = 12;
            }
            
            start_child();
            timer3--;
            if(timer3 == 0){
                child_done();
                timer3 = 12;
            }
        }
        wait_ms(1000);
    }
}
