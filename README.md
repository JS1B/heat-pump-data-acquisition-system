Heat Pump Data Acquisition System
=================================

The project uses a DFRobot Nano v4 board to read data from multiple temperature and humidity sensors. The data is then sent to a logger csv file via uart where it can be extracted from with a computer.

> [!NOTE]
> The project was developed using DFRobot Nano v4 board. For that board the baudrate is 57600. If you are using a different board, you may need to change the baudrate in the code.
> Same goes for the pins and the number of sensors connected to the board via the multiplexer.