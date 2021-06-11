<p align="center">
  <img align="center" src="/assets/snap.svg" alt="Oh, Snap!" width="250">
</p>

<h1 align="center">Oh, Snap!</h1>
<p align="center">A Fabrication Pipeline to Magnetically Connect Conventional and 3D-Printed Electronics.</p>

<div align="center">
<img src="/assets/schnapp.gif" width="80%">
  <br />
    <!--<a href="https://github.com/othneildrew/Best-README-Template"><strong>Explore the docs »</strong></a>
    <br />
    <br />-->
  <h4 align="center">
    <a href="https://www.youtube.com/watch?v=JX3ZwKnnJVs">Watch Teaser</a>
    ·
    <a href="/assets/Schmitz2021-OhSnap.pdf">View PDF</a>
    ·
    <a href="https://www.youtube.com/watch?v=0AUrrtwaPVQ">Watch Talk</a>
    ·
    <a href="https://doi.org/10.1145/3411764.3445641">Follow DOI</a>
    </h4>
</div>



## Paper Abstract
3D printing has revolutionized rapid prototyping by speeding up the creation of custom-shaped objects. With the rise of multi-material 3D printers, these custom-shaped objects can now be made interactive in a single pass through passive conductive structures. However, connecting conventional electronics to these conductive structures often still requires time-consuming manual assembly involving many wires, soldering or gluing.

To alleviate these shortcomings, we propose <i>Oh, Snap!</i>: a fabrication pipeline and interfacing concept to magnetically connect a 3D-printed object equipped with passive sensing structures to conventional sensing electronics. To this end, <i>Oh, Snap!</i> utilizes ferromagnetic and conductive 3D-printed structures, printable in a single pass on standard printers. We further present a proof-of-concept capacitive sensing board that enables easy and robust magnetic assembly to quickly create interactive 3D-printed objects. We evaluate <i>Oh, Snap!</i> by assessing the robustness and quality of the connection and demonstrate its broad applicability by a series of example applications.

## How to use
We provide the schematics and firmware for the Oh, Snap board for capacitive sensing, as well as sample code and 3D models used for the example applications in the paper.

### Schematics
The [schematics](/schematics/) folder contains all schematics to clone the presented Oh, Snap board or use them as a basis for another kind of board. The schematics are provided in Eagle format.

### Firmware
The [firmware](/firmware/) folder contains the firmware required to operate the Oh, Snap board. Please see [firmware/README.md](/firmware/README.md) for more details on the flashing procedure.

### Example Applications
![Apps](/assets/apps.jpg)

The [examples](/examples/) folder contains code for some example applications:
- [Car Gesture](examples/gesture) (A)
- [Flowerpot](examples/flowerpot) (B)
- [Smarthome Control](examples/smarthome) (C)
- [Piano](examples/piano)

In addition, it also contains some simple monitoring demos.

### 3D Models
For the 3D models used for the example applications and the evaluation see [here](models/).


## How to cite
Please cite this work like this:
<pre>
  Martin Schmitz, Jan Riemann, Florian Müller, Steffen Kreis, and Max Mühlhäuser. 2021. 
  Oh, Snap! A Fabrication Pipeline to Magnetically Connect Conventional and 3D-Printed Electronics. 
  In Proceedings of the 2021 CHI Conference on Human Factors in Computing Systems (CHI ’21), 1–11. 
  https://doi.org/10.1145/3411764.3445641
  </pre>

or use this

<pre>
@inbook{10.1145/3411764.3445641,
author = {Schmitz, Martin and Riemann, Jan and M\"{u}ller, Florian and Kreis, Steffen and M\"{u}hlh\"{a}user, Max},
title = {Oh, Snap! A Fabrication Pipeline to Magnetically Connect Conventional and 3D-Printed Electronics},
year = {2021},
isbn = {9781450380966},
publisher = {Association for Computing Machinery},
address = {New York, NY, USA},
url = {https://doi.org/10.1145/3411764.3445641},
booktitle = {Proceedings of the 2021 CHI Conference on Human Factors in Computing Systems},
articleno = {420},
numpages = {11}
}
</pre>


## Credits
This project uses the following sources:
- [Protocentral FDC1004](https://github.com/Protocentral/ProtoCentral_fdc1004_breakout)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [MQTT PubSubClient](https://github.com/knolleary/pubsubclient)
- [ArduinoJSON]( https://github.com/bblanchon/ArduinoJson)
- [FreeRTOS]( https://github.com/FreeRTOS/FreeRTOS)
- [Tone.js]( https://github.com/Tonejs/Tone.js)
- [Plotly.js](https://github.com/plotly/plotly.js)
- [Arduino-ESP32](https://github.com/espressif/arduino-esp32)
- [Logo](https://www.pngaaa.com/detail/1826967)
