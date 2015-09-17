# udp4all

udp4all reads udp datagrams and forwards them to multiple IP addresses and ports.
In between some dataprocessing like sentence conversion could be applied.

Usually udp4all comes with a GUI, but could also be build as a non GUI daemon. 
Daemon is only available on Unix systems.

## Building

* For GUI application simply execute qmake and make.
* For daemon release execute qmake "config += console" and make. 
  The binary is named udp4alld

