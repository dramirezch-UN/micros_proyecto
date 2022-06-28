# Proyecto Microcontroladores

## Specs

El dispositivo puede rearlizar las siguientes funciones:

- Medir la cantidad de agua (en ml) que pasa por la tubería cada vez que se abre el flujo del agua. Mostrar esta cantidad en un LCD. ✅
- Si el flujo de agua dura abierto más de 30 segundos, enviar un mensaje de advertencia por Bluetooth. ✅
- Encender y apagar la bomba de agua con un botón. Si el flujo de agua dura abierto más de 60 segundos, se apaga la bomba. ✅

## Pines

(A menos que se especifique, el dispositivo tiene conecciones directas a 5V y ground.)

- El sensor de flujo se conecta a RB5.
- El HC05 (bluetooth) se conecta a los pines Tx y Rx (25 y 26).
- El botón que controla la bomba de agua se conecta a RA0. El otro pin del botón va a 5V. A RA0 tambien se debe conectar una resistencia a ground.
- La bomba de agua se conecta a RA1.
