/*
 * AD7705/AD7706 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * Initial version 1.0 3/2011
 * Updated 1.1 4/2012
 */

#include "AD770X.h"

//write communication register
//   7        6      5      4      3      2      1      0
//0/DRDY(0) RS2(0) RS1(0) RS0(0) R/W(0) STBY(0) CH1(0) CH0(0)

void AD770X::setNextOperation(byte reg, byte channel, byte readWrite) {
    byte r = 0;
    r = reg << 4 | readWrite << 3 | channel;

    digitalWrite(pinCS, LOW);
    spiTransfer(r);
    digitalWrite(pinCS, HIGH);
}

//Clock Register
//   7      6       5        4        3        2      1      0
//ZERO(0) ZERO(0) ZERO(0) CLKDIS(0) CLKDIV(0) CLK(1) FS1(0) FS0(1)
//
//CLKDIS: master clock disable bit
//CLKDIV: clock divider bit

void AD770X::writeClockRegister(byte CLKDIS, byte CLKDIV, byte outputUpdateRate) {
    byte r = CLKDIS << 4 | CLKDIV << 3 | outputUpdateRate;

    r &= ~(1 << 2); // clear CLK

    digitalWrite(pinCS, LOW);
    spiTransfer(r);
    digitalWrite(pinCS, HIGH);
}

//Setup Register
//  7     6     5     4     3      2      1      0
//MD10) MD0(0) G2(0) G1(0) G0(0) B/U(0) BUF(0) FSYNC(1)

void AD770X::writeSetupRegister(byte operationMode, byte gain, byte unipolar, byte buffered, byte fsync) {
    byte r = operationMode << 6 | gain << 3 | unipolar << 2 | buffered << 1 | fsync;

    digitalWrite(pinCS, LOW);
    spiTransfer(r);
    digitalWrite(pinCS, HIGH);
}

unsigned int AD770X::readADResult() {
    digitalWrite(pinCS, LOW);
    byte b1 = spiTransfer(0x0);
    byte b2 = spiTransfer(0x0);
    digitalWrite(pinCS, HIGH);

    unsigned int r = b1 << 8 | b2;

    return r;
}

double AD770X::readADResult(byte channel, float refOffset) {
    while (!dataReady(channel)) {
    };
    setNextOperation(REG_DATA, channel, 1);

    return readADResult() * 1.0 / 65536.0 * VRef - refOffset;
}

bool AD770X::dataReady(byte channel) {
    setNextOperation(REG_CMM, channel, 1);

    digitalWrite(pinCS, LOW);
    byte b1 = spiTransfer(0x0);
    digitalWrite(pinCS, HIGH);

    return (b1 & 0x80) == 0x0;
}

void AD770X::reset() {
    digitalWrite(pinCS, LOW);
    for (int i = 0; i < 100; i++)
        spiTransfer(0xff);
    digitalWrite(pinCS, HIGH);
}

AD770X::AD770X(double vref) {
    VRef = vref;
    pinMode(pinMOSI, OUTPUT);
    pinMode(pinMISO, INPUT);
    pinMode(pinSPIClock, OUTPUT);
    pinMode(pinCS, OUTPUT);

    digitalWrite(pinCS, HIGH);
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | _BV(SPI2X) | _BV(SPR1) | _BV(SPR0);
}

void AD770X::init(byte channel, byte clkDivider, byte polarity, byte gain, byte updRate) {
    setNextOperation(REG_CLOCK, channel, 0);
    writeClockRegister(0, clkDivider, updRate);

    setNextOperation(REG_SETUP, channel, 0);
    writeSetupRegister(MODE_SELF_CAL, gain, polarity, 0, 0);

    while (!dataReady(channel)) {
    };
}

void AD770X::init(byte channel) {
    init(channel, CLK_DIV_1, BIPOLAR, GAIN_1, UPDATE_RATE_25);
}
