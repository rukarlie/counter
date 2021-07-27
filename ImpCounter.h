/*
 * ImpCounter.h
 *
 *  Created on: 29 янв. 2021 г.
 *      Author: Karlin Egor
 */

#ifndef PORTS_IMPCOUNTER_H_
#define PORTS_IMPCOUNTER_H_

#include <stdint.h>
#include <stddef.h>
#include <xdc/std.h>
#include "ti_drivers_config.h"
#include    <ti/sysbios/knl/Clock.h>

#include <ti/drivers/timer/GPTimerCC26XX.h>
//#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/sysbios/knl/Event.h>

#define EVENT_MATCHED Event_Id_04

class ImpCounter
{
    typedef struct fMeter {
        uint32_t count_old;
        uint32_t count_new;
        uint32_t time_old;
        uint32_t time_new;
    }fMeter;

    typedef struct IC_GATE_Params{
        Event_Handle    hEvent;
        uint32_t        eventMask;
        uint16_t        matchValue;
    };

public:
    ImpCounter(uint32_t pin,  uint32_t tmr_ind);
    virtual ~ImpCounter();
    uint32_t count;
    float w;            //weight coefficient
    float volume;
    float freq;
    void clear(void);
    void start(void);
    void stop(void);
    float getFreq(void);
    uint16_t getCounts(void);
    PIN_Handle getHandle(void);
    void setGate(uint16_t mtchVal, Event_Handle hEvent);


private:
    static void frontCb(PIN_Handle handle, PIN_Id pinId);
    static void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask);
    PIN_State timerpinState;
    PIN_Handle timerPinHandle;
    PIN_Config timerPinTable[2]= {0|PIN_INPUT_EN|PIN_PULLDOWN, PIN_TERMINATE};
    Event_Handle cbEventHndl;
    fMeter fm;
    GPTimerCC26XX_Handle hTimer;

};

#endif /* PORTS_IMPCOUNTER_H_ */
