/*
 * ImpCounter.cpp
 *
 *  Created on: 29 янв. 2021 г.
 *      Author: runechuv
 */

#include <Ports/ImpCounter.h>

ImpCounter::ImpCounter(uint32_t pin, uint32_t tmr_ind )
{
    timerPinTable[0]=pin|PIN_INPUT_EN|PIN_PULLDOWN;

    timerPinHandle=PIN_open(&timerpinState, timerPinTable);

    //PIN_registerIntCb(PinHandle, (PIN_IntCb)&frontCb);
    //PIN_setUserArg(PinHandle, (UArg)&count);
    count=0;
    w=1;
    fm.count_old=count;
    fm.time_old = Clock_getTicks();

    GPTimerCC26XX_Params GPT_params;
    GPTimerCC26XX_Params_init(&GPT_params);
    GPT_params.width          = GPT_CONFIG_16BIT;
    GPT_params.mode           = GPT_MODE_EDGE_COUNT;
    GPT_params.direction      = GPTimerCC26XX_DIRECTION_UP;
    GPT_params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
    hTimer = GPTimerCC26XX_open(tmr_ind, &GPT_params);

    GPTimerCC26XX_setCaptureEdge(hTimer, GPTimerCC26XX_POS_EDGE);
    GPTimerCC26XX_registerInterrupt(hTimer, timerCallback, GPT_INT_MATCH);

    GPTimerCC26XX_PinMux pinMux = GPTimerCC26XX_getPinMux(hTimer);
    //Fan output is mapped to pin
    PINCC26XX_setMux(timerPinHandle, pin, pinMux);
    setGate(1000, 0);
}

ImpCounter::~ImpCounter()
{
    PIN_close(timerPinHandle);
    GPTimerCC26XX_close(hTimer);
    // TODO Auto-generated destructor stub
}

void ImpCounter::start(){

    //PIN_setInterrupt(PinHandle, PinTable[0]|edge);
    count = GPTimerCC26XX_getValue(hTimer);
    GPTimerCC26XX_start(hTimer);
    fm.count_old=count;
    fm.time_old = Clock_getTicks();


}

void ImpCounter::setGate(uint16_t mtchVal, Event_Handle hEvent){
    cbEventHndl = hEvent;
    if(mtchVal!=0){
        GPTimerCC26XX_setMatchValue(hTimer, mtchVal);
        GPTimerCC26XX_enableInterrupt(hTimer, GPT_INT_MATCH);
        GPTimerCC26XX_setArg(hTimer, cbEventHndl);
    } else {
        GPTimerCC26XX_disableInterrupt(hTimer, GPT_INT_MATCH);
    }
}

void ImpCounter::stop(){

     //PIN_setInterrupt(PinHandle, PinTable[0]|PIN_IRQ_DIS);
    GPTimerCC26XX_stop(hTimer);
    count = GPTimerCC26XX_getValue(hTimer);



 }

void ImpCounter::clear(){
    count=0;
    volume=0;
    //GPTimerCC26XX_setLoadValue(hTimer, 0);
    uint32_t offset = hTimer->timerPart * 4;

    HWREG(hTimer->hwAttrs->baseAddr+ offset+ GPT_O_TAV ) = 0 ;
    fm.count_old=count;
    fm.time_old = Clock_getTicks();

}

PIN_Handle ImpCounter::getHandle(){
    return timerPinHandle;
}

float ImpCounter::getFreq(){
    count = GPTimerCC26XX_getValue(hTimer);
    fm.count_new=count;
    fm.time_new=Clock_getTicks();
    freq= (float)(fm.count_new-fm.count_old)*1000000 / (float)((fm.time_new-fm.count_old)*Clock_tickPeriod);
    fm.count_old=fm.count_new;
    fm.time_old = fm.time_new;
    return freq;
}

uint16_t ImpCounter::getCounts(){
    count = GPTimerCC26XX_getValue(hTimer);
    return count;
}

void ImpCounter::frontCb(PIN_Handle handle, PIN_Id pinId){
    uint32_t* pCount = (uint32_t*)handle->userArg;
    *pCount= *pCount +1;
}

void ImpCounter::timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask){
    //GPTimerCC26XX_setLoadValue(handle, 0);
    Event_Handle hEvent;
    hEvent = (Event_Handle) GPTimerCC26XX_getArg(handle);
    if(hEvent!=NULL){
     Event_post(hEvent, EVENT_MATCHED);
    }
}

