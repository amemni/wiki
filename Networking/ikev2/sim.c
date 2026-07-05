#include "stdio.h"
#include "stdlib.h"

// IKE STATES
#define NO_CHANGE 4
#define IKE_START_STATE 0
#define IKE_INIT_STATE 1
#define IKE_AUTH_STATE 2
#define IKE_ESTAB_STATE 3

// IKE events
#define INIT_EVENT 0
#define TIMEOUT_EVENT 1
#define DATA_EVENT 2
#define REDIRECT_EVENT 3

#define FSM_Q_SIZE 1000

int ikeFsmQHead = 0;
int ikeFsmQTail = 0;

# IKE structure
typedef struct {
	int     curState;
} ikeStruct;

// FSM parameter
typedef struct
{
    int ikeEvent;
    void *ike;              // IKEv2 structure
    unsigned char *fsmBuff; // actual packet
} fsmParam;

fsmParam *ikeFsmQ[FSM_Q_SIZE]; // Final-State Machine parameter

// FSM structure
typedef struct
{
    int (*funcPtr)(ikeStruct *ike, int ikeEvent, unsigned char *fsmBuff);
    int nextState;
} fsmStruct;

// FSM events
int invalidEvent (ikeStruct *ike, int ikeEvent, unsigned char *buff) {
    if (buff != NULL)
        free(buff);
    return 1;
}
int timeoutEvent (ikeStruct *ike, int ikeEvent, unsigned char *buff) {
    if (buff != NULL)
        free(buff);
    return 1;
}
int dataEvent (ikeStruct *ike, int ikeEvent, unsigned char *buff) {
    if (buff != NULL)
        free(buff);
    return 1;
}
int ikeStart (ikeStruct *ike, int ikeEvent, unsigned char *buff) {
    if (buff != NULL)
        free(buff);
    return 1;
}

char* eventToString (int event) {
    switch(event) {
        case 0: return "INIT_EVENT";
        case 1: return "TIMEOUT_EVENT";
        case 2: return "DATA_EVENT";
        case 3: return "REDIRECT_EVENT";
        default: return "UNKNOWN_EVENT";
    }
}

char* stateToString (int state) {
    switch(state) {
        case 0: return "IKE_START_STATE";
        case 1: return "IKE_INIT_STATE";
        case 2: return "IKE_AUTH_STATE";
        case 3: return "IKE_ESTAB_STATE";
        default: return "UNKNOWN_STATE";
    }
}

// FSM
static fsmStruct ikeFsm[4][4] = {
    { /* IKE_START */
        {ikeStart, IKE_INIT_STATE}, // if you get an IKE_START event, you call function ikeStart() and if it returns true, you go to event IKE_INIT
        {invalidEvent, NO_CHANGE},
        {invalidEvent, NO_CHANGE},
        {invalidEvent, NO_CHANGE},
    },
    { /* IKE_INIT */
        {invalidEvent, NO_CHANGE},
        {timeoutEvent, NO_CHANGE},
        {dataEvent, IKE_AUTH_STATE},
        {invalidEvent, NO_CHANGE},
    },
    { /* IKE_AUTH */
        {invalidEvent, NO_CHANGE},
        {invalidEvent, NO_CHANGE},
        {dataEvent, IKE_ESTAB_STATE},
        {ikeStart, IKE_INIT_STATE},
    },
    { /* IKE_ESTAB */
        {invalidEvent, NO_CHANGE},
        {invalidEvent, NO_CHANGE},
		{dataEvent, NO_CHANGE},
		{invalidEvent, NO_CHANGE},
    }
};

// FSM event routine
fsmRoutine() {
    ikeStruct *ike; // peer IKE
    fsmParam *fp;
    int event;      // the event and packet will be pulled out of the FSM structure
    int count = 0;
    unsigned char *fsmBuff;

    printf("\n IKE FSM started ..");
    while (1) {
        do {
            if (ikeFsmQ[ikeFsmQHead] != 0) { // pull if there's an event
                fp = ikeFsmQ[ikeFsmQHead]; // your FSM param
                ike = fp -> ike;
                event = fp -> ikeEvent;
                fsmBuff = fp -> fsmBuff;
                ikeFsm[ike -> curState][event].funcPtr(ike, event, fsmBuff); // a function pointer to the next event

                if (ikeFsm[ike -> curState][event].nextState != NO_CHANGE) { // update IKE state
                    ike -> curState = ikeFsm[ike -> curState][event].nextState;
                }

                free(ikeFsmQ[ikeFsmQHead]);
                ikeFsmQ[ikeFsmQHead] = 0;
                if (ikeFsmQHead == FSM_Q_SIZE - 1) {
                    printf("\n ikeFsmQHead wraps up to 0 .."); // all 1000 packets processed
                    ikeFsmQHead = 0;
                } else {
                    ikeFsmQHead++;
                }
                if ((++count % 50) == 0) { // a little breather
                    printf("\n FSM routine sleeping ..");
                    sleep(3);
                }
            } else {
                sleep(1);
                continue;
            }
        } while (ikeFsmQHead != ikeFsmQTail); // exit if FSM queue is finished and loop again
    }
}

ikeFsmExecute(ikeStruct *ike, int ikeEvent, unsigned char *fsmBuff) {
    fsmParam *fp;
    char status;

    printf("\nIKE_FSM: Cur State: %s, Event: %s", stateToString(ike->curState), eventToString(ikeEvent));

    fp = (fsmParam*)malloc(sizeof(fsmParam)); // malloc a FSM param structure
    if (fp == 0) {
        printf("\nNo mem at ikeFsmExecute");
        return;
    }
    fp->ikeEvent = ikeEvent; // fill the FSM param structure
    fp->ike = ike;
    fp->fsmBuff = fsmBuff;

    if (ikeFsmQ[ikeFsmQTail] == 0) { // put the FSM param structure into the queue
        ikeFsmQ[ikeFsmQTail] = fp;
        if (ikeFsmQTail == FSM_Q_SIZE - 1) {
            printf("\n ikeFsmQTail wraps up to 0 .."); // all 1000 packets processed
            ikeFsmQTail = 0;
        } else {
            ikeFsmQTail ++
        }
    } else {
        printf("FSM event dropped...Q full.\n")
    }
}

main() {
    printf("\n IPSec simulator started ..");
    return 1;
}
