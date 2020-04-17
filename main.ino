#include <arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "Vector.h"
#include "virtuabotixRTC.h"


// --- Mapeamento de Hardware ---
#define   clk   6
#define   dat   7
#define   rst   8


// ========================================================================================================
// --- Constantes Auxiliares ---
#define   segL       15
#define   minL       32
#define   horL       11
#define   d_semL      4
#define   d_mesL     18
#define   mesL        4
#define   anoL     2025


// ========================================================================================================
// --- Declaração de Objetos ---
virtuabotixRTC myRTC(clk, dat, rst);         //declara objeto para o RTC
using namespace std;

void sleep() {    // disable ADC
// watchdog interrupt
    ISR(WDT_vect)
    {
        wdt_disable();  // disable watchdog

    }  // end of WDT_vect

    ADCSRA = 0;

// clear various "reset" flags
    MCUSR = 0;
// allow changes, disable reset
    WDTCSR = bit(WDCE) | bit(WDE);
// set interrupt mode and an interval
    WDTCSR = bit(WDIE) | bit(WDP3) | bit(WDP0);    // set WDIE, and 8 seconds delay
    wdt_reset();  // pat the dog

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    noInterrupts();           // timed sequence follows
    sleep_enable();

// turn off brown-out enable in software
    MCUCR = bit(BODS) | bit(BODSE);
    MCUCR = bit(BODS);
    interrupts();             // guarantees next instruction executed
    sleep_cpu();

// cancel sleep as a precaution
    sleep_disable();
}


void light(int led) { //TODO ver como funciona o ci e programar

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);

}

//Remedios
struct medicine {
    char name[40];
    int place;
    int stock;
    int dose[2];
};

int nextDose;
int led;
//TODO decidir se struct ou objeto melhor
int daily;
int takenTilNow;

Vector<int> scheduleMinutes; //TODO ver se o vetor ta funcionando
Vector<int> scheduleMedicine;


//--------------------Auxiliares-----------------
int currentTime() {
    myRTC.updateTime();
    int time = myRTC.hours * 60 + myRTC.minutes

    return time; //converte pra minutos
}


int getNextDose() { //TODO arrumar essa funcao
    int nowMinutes = currentTime() - 1;
    if (takenTilNow < daily) { //calcula quantos rests o atmega deve fazer ate proximo remedio
        int difference = scheduleMinutes[takenTilNow] - nowMinutes;
        nextDose = 60 * difference / 8;
    } else {
        nextDose = 0;
    }
    return nextDose;
}

void setup() {
    daily = scheduleMinutes.size();
    int nowMinute = currentTime();
    for (int i = 0; scheduleMinutes[i] < nowMinute; i++) { //ve quantos remedios ja deveriam ter sido tomados ate agr
        takenTilNow = i;
    }

    nextDose = getNextDose();
}

void loop() {
    int nextDoseSteps = nextDoseSteps - 1;
    if (takenTilNow < daily) {
        if (nextDoseSteps == 0) {
            led = true; //TODO melhorar logicA
            bool botaoApertado = false;
            while (led) { //verficar se vale como boolean
                //setar led high
                if (!botaoApertado) { //botao apertado
                    nextDoseSteps = getNextDose();
                }
            }
        }
    }

    sleep(); //Dorme esperando proximo remedio
}
