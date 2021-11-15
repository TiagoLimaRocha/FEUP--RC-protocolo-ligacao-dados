# Redes de Computadores

![feuplogo](./assets/feup_logo.jpg)
## Protocolo de Ligação de Dados
*25 de Novembro de 2021*

#### Contributors
- [Tiago Lima Rocha](mailto:up201406679@up.pt)  up201406679
- [Tiago Azevedo](mailto:upxxxxxxxx@up.pt) upxxxxxxxxx

## Table of Contents
- [Redes de Computadores](#redes-de-computadores)
  - [Protocolo de Ligação de Dados](#protocolo-de-ligação-de-dados)
      - [Contributors](#contributors)
  - [Table of Contents](#table-of-contents)
  - [Abstract](#abstract)
  - [Architecture](#architecture)
  - [Code Structure](#code-structure)
  - [Main Use Cases](#main-use-cases)
  - [Link Layer Protocol](#link-layer-protocol)
  - [Application Layer Protocol](#application-layer-protocol)
  - [Validation](#validation)
  - [Efficiency](#efficiency)
      - [Connection capacity](#connection-capacity)
      - [Data frame size](#data-frame-size)
      - [FER - Frame Error Ratio](#fer---frame-error-ratio)
  - [Conclusions](#conclusions)
  - [Code](#code)

## Abstract

In this report we explain our approach and results for the very first lab project in the course Computer Networks. The project consists in the transmission of files using the serial port. 

The final goal of this project was to develop and test a low level application for file transfering through a serial port making use of 2 independent layers: the link layer and the application layer.

**Keywords:** *functional blocks and interfaces used in the architecture*, *API, data structures and methods*, *project use cases and function call sequences*, *functional aspects and strategies implemented in the link layer*, *functional aspects and strategies implemented in the link layer*, *validations that were done*, *measurement and analysis of the protocol's efficiency* as well as a *summary of the conclusions we've drawn about the project*
## Architecture

There are 2 fundamental and independent functional blocks in this project: the **data link** block and the **application** block.

The **data link** block has as a goal open up and close down the connection, as well as assuring a correct transmission of all the frames, and recover them when transmission errors occur. This block has a very low level and communicates directly with the serial port.

The **application** block has as a goal to read and write files, manage data frame sizes and assure that all received data is valid. This block has a higher level and makes use of the middleware API defined in the data link block.

Memory management in both these blocks happens with the use of a **dynamic array**.

The program's **CLI** allows the use of the same executable for both reception and transmission of files by specifying the option in the comand line. Besides that it is necessary to specify the name or path of the file we want to transmit/receive as well as the name of the serial port we want to use for either operation. Moreover, **baudrate**, **max frame size**, **max transmission tries** and **timeout in seconds** are optional parameters in this interface. When the program finishes executing, transmission statistics are shown in the standard output.

## Code Structure

## Main Use Cases

The **link layer** will have as main use cases to allow for an application to connect a serial port (through **ll_open**), send or receive data framents (**ll_read** and **ll_write**), and shut down the connection (**ll_close**). Moreover it showcases the statistics about the connection itself, in this case total frames sent and frames lost. This layer can be configured with the use of **ll_setup**, making it possible to change the *baudrate*, *max number of tries* and *timout* limit for the transmission.

The **application layer** has  two main use cases: send a file (**al_send_file**, which also uses **ll_write** in its implementation) or receive a file (**al_receive_file**, which uses **ll_read** in its implementaion), and both these methods make use of the functions **ll_open**, **ll_close** form the link layer. This layer can be configured using **al_setup** in which it's possible to change the *max data fragment size*. It also displays the inforation about *average bits per second*, *number of data packets sent*, *transmission duration* and *efficiency*.   
## Link Layer Protocol

The data link protocol begins its flow with the **ll_open** method which starts by opening a connection with the serial port using the supplied configurations. After establishing a connection, the receptor end waits for the transmitter to send a control frame **SET**, and this makes the latter wait for a **UA** control frame response to acknowledge the connection.

After this communication occurs it is then possible to transmit I frames using **ll_write** and **ll_read**. In the **ll_write** function hte header of the data frame is firstly created, and then the data fragments are introduced in the frame on a per byte basis. Simultaneously, the **BCC2** security byte is calculated which gives place for the **byte stuffing** mechanism to take place. Finally, the program verifies if **BCC2** needs **byte stufffing** and introduces it in the frame. When the frame is complete, it is then sent through the serial port, and a response containing the **RR** control frame is awaited. If that response takes longer than the stipulated limit timeframe, or the **RR** frame is a double or if the response is actually a **REJ** frame, a re-transmission must now take place. If the re-transmission limit is surpassed, the operation aborts.

In the **ll_read** method we await for a data frame. When said frame arrives, the header must be validated and the data is read byte by byte, using a technique called **byte destuffing** when necessary and calculating the expected **BCC2**. After this process, the **BCC2** is read and compared with the expected one previously calculated. If they match an **RR** control frame is sent. Otherwise, if a problem occurs a control framewith control field **REJ** is sent instead.

Last but not least, in **ll_close** the transmitter sends a control frame with control field **DISC** and the receptor must respond with **DISC** as well. At this point the transmitter sends a control frame with control field **UA**, shuts down the connection and resets the serial port configuration. The receptor does the same on its end after receiving the **UA** message.

## Application Layer Protocol

The application protocol has a higher level and uses the middleware API from the data link to make file transfers. This layer can be represented by two fundamental methods: **al_receive_file** and **al_send_file**. Both these methods initialize and close the connection by the end of their flow.

The **al_send_file** method reads the file and sends a control packet to commence the transmission. Afterwards, for each data fragment, it is created a data package which includes the file name or path, package sequence number, segment size and the actual segment data using the function **ll_write** from the link layer. When all fragments are sent another control package is sent to sinalize the end of transmission.

The **al_receive_file** function makes use of **ll_read** to read these packets. While it receives the packets, this method saves the data fragments in a file until it receives the control packet that symbolizes the end of the operation. This function validates the packets sequence numbers and, at the end, if it received the right quantity of bytes.

This layer also outputs transmission information.
## Validation

The following test scenarios were implemented in this project:

- Transmission of files with different sizes
- Interruption of the connection at random moments during transmission
- Introduction of random bits during transmission
- Simulation of erros in the BCC2 field of the data packets
- Variation of multiple application parameters such as *baudrate*, *max number of retries*, *timeout* and *frame size*.

## Efficiency

#### Connection capacity

#### Data frame size

#### FER - Frame Error Ratio

## Conclusions

## Code
[main](../app/src/main.c)
[types](../app/src/lib/types.h)
[macros](../app/src/lib/macros.h)
[utils](../app/src/lib/utils/utils.c)
[Hash Map](../app/src/lib/HashMap/HashMap.c)
[Linked List](../app/src/lib/LinkedList/LinkedList.c)
[Byte Buffer](../app/src/lib/ByteBuffer/ByteBuffer.c)
[Applicaton Layer](../app/src/lib/api/ApplicationLayer/ApplicationLayer.c)
[Link Layer](../app/src/lib/api/LinkLayer/LinkLayer.c)