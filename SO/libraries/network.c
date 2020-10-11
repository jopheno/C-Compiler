void setBaudRate(int kind, int baud) {
    if (kind == 0) {
        {{
            DMA_LOADi eax &baud
            UART_ABAUDSET eax
        }};
    } else {
        {{
            DMA_LOADi eax &baud
            UART_BBAUDSET eax
        }};
    }
}

void sendOverUART(int kind, int valu) {

    if (kind == 0) {
        {{
            DMA_LOADi eax &valu
            UART_AWRITE eax
        }};
    } else {
        {{
            DMA_LOADi eax &valu
            UART_BWRITE eax
        }};
    }

}

int isDataReadyOverUART(int kind) {
    int isready;

    if (kind == 0) {
        {{
            UART_ATELL eax
            DMA_STOREi eax &isready
        }};
    } else {
        {{
            UART_BTELL eax
            DMA_STOREi eax &isready
        }};
    }

    return isready;
}

int readOverUART(int kind) {
    int result;
    int ready;

    result = 0;
    ready = isDataReadyOverUART(kind);

    while(ready <= 0) {
        ready = isDataReadyOverUART(kind);
    }

    if (kind == 0) {
        {{
            UART_AREAD eax
            DMA_STOREi eax &result
        }};
    } else {
        {{
            UART_BREAD eax
            DMA_STOREi eax &result
        }};
    }

    return result;
}
