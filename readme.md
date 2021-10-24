# Protocolo de Ligação de Dados

## QUICK START

#### Prerequisites
 
- socat: 
```
$ sudo apt update
$ sudo apt install socat
```

#### Usage:

``` $ ./RC_LAB1_APP <file_name> <T|R> <port_number> [options]```

T - Transmitter, R - Receiver

*Options:*
```
  -timeout=<seconds>            Seconds until a frame is timed out
  -baudrate=<rate>              Serial port rate
  -max_retries=<retries>        Times a frame transmission can be retried
  -frag_size=<size>             Max size for data fragments
```

```
$ sudo socat -d  -d  PTY,link=/dev/ttyS10,mode=777   PTY,link=/dev/ttyS11,mode=777
$ ./RC_LAB1_APP static/pinguim.gif T 10
$ ./RC_LAB1_APP static/pinguim.gif R 11
```
