
#include <stdio.c>
#include <network.c>

void main(void) {
    int outv;
    int count;
    int inp;
    int ready;

    count = 0;
    inp = 0;
    outv = 0;
    ready = 0;

    while (count == count) {
        // Reads a value
        inp = input();

        // There should be no data available to read at this point
        ready = isDataReadyOverUART(1);
        if (ready != 0){
            // ERROR E005
            // Some data is ready over UART,
            // however at this point, there
            // should be no data
            output(57349);
            inp = input();
            output(ready);
            inp = input();
            output(57349);
            inp = input();
        }

        // Writes on UART kind A
        sendOverUART(0, inp);

        // There should be some data available to read at this point
        ready = isDataReadyOverUART(1);
        if (ready != 1){
            // ERROR E006
            // There is no data at all
            // over UART, however, in this
            // context, this was added
            // couple instructions before
            output(57350);
            inp = input();
            output(ready);
            inp = input();
            output(57350);
            inp = input();
        }

        // Reads the data entered before
        // ERROR E007 (just for location)
        output(57351);
        inp = input();
        // outv = readOverUART(1);
        {{
            ALU_XOR eax eax eax
            UART_BDEBUG1 eax
            DMA_STOREi eax &outv
        }};
        output(outv);
        inp = input();
        // ERROR E008 (just for location)
        output(57352);
        inp = input();
        {{
            ALU_XOR eax eax eax
            UART_BDEBUG2 eax
            DMA_STOREi eax &outv
        }};
        output(outv);
        inp = input();
        // ERROR E009 (just for location)
        output(57353);
        inp = input();
        outv = readOverUART(1);
        output(outv);
        
    }
}
