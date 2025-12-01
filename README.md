# RR BLE33 MOUSEBOUT

Software for running mousebot on Arduino BLE33

## Hardware Layout

USB Connection from Raspberry Pi to arduino Nano BLE33

## Communication Protocol

### Supported Commands

#### Commands

| ID      | CONSTANT        |  SENSOR   | DESCRIPTION              |
| ------  | --------------  | --------- | ------------------------ |
| 200     | MSP_SET_RAW_RC. | Motors.   | Sets motors              |


#### Monitor Commands

| ID.     | CONSTANT        |  SENSOR   | DESCRIPTION              |
| ------  | --------------  | --------- | ------------------------ |
| 102     | MSP_RAW_IMU.    | IMU       | Monitor IMU details      |
| 104     | MSP_MOTOR       | MOTORS    | Set, or monitor motors.  |
| 105     | MSP_RAW_SENSORS | Range     | Range sensors            |

#### Error Codes

The following codes are not supported by MSP protocol, MSP protocol uses message prefix, specifically 'M!' to signify errors:
refer to (Reefwing-MSP)[https://github.com/Reefwing-Software/Reefwing-MSP].

| ID.     | CONSTANT        |  SENSOR   | DESCRIPTION                                       |
| ------  | --------------  | --------- | ------------------------------------------------- |
| 400     | BAD_REQUEST     | NA        | will not process the request due to an apparent client error (e.g., malformed request syntax, size too large, invalid request message framing, or deceptive request routing). |


#### DataFrame

Dataframe used is simple, it contains a termination character that indicates the end of the payload. 

### Termination Character

Termination character used in 0x1E, tranditionally RECORD SEPARATOR in ASCII file systems.

### Maximum Length

The maximum length of a request 1200 bytes, internally the code will retreive chunks using a 240 byte buffer, which 
forums regard as a good limit, all though I was unable to find any official documenation confirming this.

## References

(platform IO reference)[https://docs.platformio.org/en/latest/boards/nordicnrf52/nano33ble.html]
(Nano 33. Pin layout)[https://docs.arduino.cc/resources/pinouts/ABX00030-full-pinout.pdf]
(nanopb/Nanopb)[https://registry.platformio.org/libraries/nanopb/Nanopb/examples/network_server]