
#include <stdio.c>
#include <network.c>

void main(void) {
    int outv;
    int count;
    int inp;
    int ready;
    int kind;
    int baudrate;

    ready = 0;
    count = 0;
    inp = 0;
    outv = 0;
    kind = 0;
    baudrate = 1000;

    setBaudRate(0, baudrate);
    setBaudRate(1, baudrate);

    while (count == count) {
        // Enter kind
        // CC
        output(204);
        // Reads a value
        kind = input();

        // Enter a command
        // CD
        output(205);
        inp = input();

        // ADD
        if (inp == 1) {
            outv = 2781;
            if (kind == 1) {
                outv = outv + 4096;
            }
            // 0ADD / 1ADD
            output(outv);
            inp = input();

            sendOverUART(kind, inp);
            inp = 0;
        }

        // DEE
        if (inp == 2) {
            outv = 3566;
            if (kind == 1) {
                outv = outv + 4096;
            }
            // 0DEE / 1DEE
            output(outv);
            inp = input();

            ready = isDataReadyOverUART(kind);

            if (ready != 0) {
                outv = readOverUART(kind);
                output(outv);
            } else {
                outv = outv - 3566;
                outv = outv + 2730;
                // 0AAA / 1AAA
                output(outv);
            }
            inp = 0;
        }

        inp = input();
    }
}
