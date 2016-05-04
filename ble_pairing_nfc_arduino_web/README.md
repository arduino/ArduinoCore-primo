#BLE pairing with NFC and launch Arduino.org website
This example is based on the Heart Rate Application with BLE pairing using NFC example. It implements the pairing from a BLE Central device using the NFC interface, it uses the NFC tag module that is one of the nRF52 peripherals and the NFC antenna that is delivered with the Arduino Primo.
When the pairing is successful, new NFC tag data is configured to launch Arduino org website. When the connection is lost, advertising does not restart automatically. 
To pair with a different device, remove bonding information by pressing Button USER2 on application startup.
Touch the NFC antenna with the smartphone or tablet and observe that the BSP_INDICATE_ADVERTISING state is indicated. LED USER2 is lit when the NFC tag detects the NFC field, LED L13 is lit when the BLE service discovery.

#Known Issue
The NFC pairing functionality can be a bit flaky on some Android phones (including Samsung models).
Testing ASUS Zenfone2, HTC A9 and samsumg s7 with NFC pairing functionality, it does not work well.
