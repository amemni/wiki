#include "stdio.h"
#include "stdlib.h"

// IKE STATES
#define NO_CHANGE 4
#define IKE_START_STATE 0
#define IKE_INIT_STATE 1
#define IKE_AUTH_STATE 2
#define IKE_ESTAB_STATE 3

#define INIT_EVENT 0
#define TIMEOUT_EVENT 1
#define DATA_EVENT 2
#define REDIRECT_EVENT 3

#define FSM_Q_SIZE 1000

int ikeFsmQHead = 0;
int ikeFsmQTail = 0;

typedef struct {
	int     curState;
} ikeStruct;

typedef struct
{
    int ikeEvent; 
    void *ike;
    unsigned char *fsmBuff;
} fsmParam;

fsmParam *ikeFsmQ[FSM_Q_SIZE];

// FSM 
typedef struct 
{
    int (*funcPtr)(ikeStruct *ike, int ikeEvent, unsigned char *fsmBuff);
    int nextState;
} fsmStruct;

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

static fsmStruct ikeFsm[4][4] = {
    { /* IKE_START */
        {ikeStart, IKE_INIT_STATE},
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

fsmRoutine() {
    ikeStruct *ike;
    fsmParam *fp;
    int event;
    int count = 0;
    unsigned char *fsmBuff;

    printf("\n IKE FSM started ..");
    while (1) {
        do {
            if (ikeFsmQ[ikeFsmQHead] != 0) {
                fp = ikeFsmQ[ikeFsmQHead];
                ike = fp -> ike;
                event = fp -> ikeEvent;
                fsmBuff = fp -> fsmBuff;
                ikeFsm[ike -> curState][event].funcPtr(ike, event, fsmBuff);

                if (ikeFsm[ike -> curState][event].nextState != NO_CHANGE) {
                    ike -> curState = ikeFsm[ike -> curState][event].nextState;
                }
                free(ikeFsmQ[ikeFsmQHead]);
                ikeFsmQ[ikeFsmQHead] = 0;
                if (ikeFsmQHead == FSM_Q_SIZE - 1) {
                    printf("\n ikeFsmQHead wraps up to 0 ..");
                    ikeFsmQHead = 0;
                } else {
                    ikeFsmQHead++;
                }
                if ((++count % 50) == 0) {
                    printf("\n FSM routine sleeping ..");
                    sleep(3);
                }
            } else {
                sleep(1);
                continue;
            }
        } while (ikeFsmQHead != ikeFsmQTail);
    }
}

ikeFsmExecute() {}

main() {
    printf("\n IPSec simulator started ..");
    return 1;
}
