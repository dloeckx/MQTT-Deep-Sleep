# MQTT-Deep-Sleep
Sample program to wake-up from deep sleep, send and receive a message over MQTT to Home Assistant, and go back to sleep. See also https://github.com/dawidchyrzynski/arduino-home-assistant

Tested on a ESP32 dev board (https://www.ezsbc.com/product/esp32-breakout-and-development-board/).

## Example

The below messages keep repeating, with a pause of 5 seconds after each _Goodnight!_.

```
Welcome. bootCount =  0
Connected to WiFi: 192.168.###.###
Connected to mqtt
New message on topic: aha/sleep_esp32/bootCount/stat_t: 0at_t
Goodnight!
!�
ޘ̀q�␑�!�)))����!���!�

Welcome. bootCount =  1
Connected to WiFi: 192.168.###.###
Connected to mqtt
New message on topic: aha/sleep_esp32/bootCount/stat_t: 1at_t
Goodnight!
!�
ޘ̀q�␑�!�)))����!���!�

Welcome. bootCount =  2
Connected to WiFi: 192.168.###.###
Connected to mqtt
New message on topic: aha/sleep_esp32/bootCount/stat_t: 2at_t
Goodnight!
!�
ޘɀq�␑�!�)))����!���!�

```

Note that sometimes the wifi or mqtt connections don't initialise, or the messages are not send or received. So for a reliable solution, these exceptions should be caught and handled.