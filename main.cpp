int analogPin = A0;        // Pin de entrada analógica
int val = 0;               // Valor leído del pin analógico
int maxVal = 0;            // Valor máximo de la señal
int minVal = 1023;         // Valor mínimo de la señal
unsigned long startTime;
const unsigned long samplingTime = 1000; // Tiempo de muestreo (1 segundo)

// Configuración de botones
const int botonPin = 2;    // Botón para iniciar la adquisición
const int botonpin2 = 4;   // Botón para detener la adquisición
int contador = 0;          // Contador para el estado del botón principal
int contador2 = 0;         // Contador para el estado del segundo botón

int capacidad = 10;        // Capacidad inicial del arreglo
int** arr = new int*[2];   // Arreglo dinámico bidimensional (2 filas)
unsigned long* tiempos = new unsigned long[capacidad]; // Arreglo para almacenar tiempos
int index = 0;             // Índice para almacenar los valores en el arreglo
bool capturandoDatos = false;  // Bandera para determinar si se está capturando

// Función para redimensionar los arreglos manualmente
void redArr(int*& arrDatos, unsigned long*& tiempos, int& capacidad) {
    int nuevaCap = capacidad * 2;

    // Redimensionar arreglo de datos (primera fila)
    int* nuevoArrFila1 = new int[nuevaCap];
    for (int i = 0; i < capacidad; i++) {
        nuevoArrFila1[i] = arrDatos[i]; // Copiar manualmente los datos
    }
    delete[] arrDatos;
    arrDatos = nuevoArrFila1;

    // Redimensionar arreglo de tiempos
    unsigned long* nuevoTiempos = new unsigned long[nuevaCap];
    for (int i = 0; i < capacidad; i++) {
        nuevoTiempos[i] = tiempos[i]; // Copiar manualmente los tiempos
    }
    delete[] tiempos;
    tiempos = nuevoTiempos;

    capacidad = nuevaCap;
}

void setup() {
    Serial.begin(9600);   // Inicializar comunicación serial
    pinMode(botonPin, INPUT);
    pinMode(botonpin2, INPUT);

    // Inicializar el arreglo bidimensional con dos filas
    arr[0] = new int[capacidad]; // Primera fila: datos de adquisición
    arr[1] = new int[capacidad]; // Segunda fila: tiempos de adquisición
}

void loop() {
    int estadoBoton = digitalRead(botonPin);   // Leer el estado del botón principal
    int estadoBoton2 = digitalRead(botonpin2); // Leer el estado del segundo botón

    if (estadoBoton == HIGH && !capturandoDatos) {
        Serial.println("Iniciando adquisición de datos...");
        capturandoDatos = true;
        contador = 1;
        startTime = millis();
    }

    if (estadoBoton2 == HIGH && capturandoDatos) {
        Serial.println("Deteniendo adquisición de datos...");
        capturandoDatos = false;
        contador2 = 1;
    }

    if (capturandoDatos) {
        // Adquisición de datos
        if (millis() - startTime < samplingTime) {
            val = analogRead(analogPin);

            // Guardar el valor en la primera fila (datos)
            if (index >= capacidad) {
                redArr(arr[0], tiempos, capacidad);  // Redimensionar el arreglo si es necesario
            }
            arr[0][index] = val;

            // Guardar el tiempo en la segunda fila
            arr[1][index] = millis() - startTime;  // Guardar el tiempo transcurrido

            index++;  // Incrementar índice para la siguiente entrada

            // Actualizar el valor máximo y mínimo
            if (val > maxVal) {
                maxVal = val;
            }
            if (val < minVal) {
                minVal = val;
            }

            delay(1);  // Pequeña pausa
        } else {
            // Calcular la amplitud pico a pico
            int amplitude = maxVal - minVal;
            Serial.print("Valor máximo: ");
            Serial.println(maxVal);
            Serial.print("Valor mínimo: ");
            Serial.println(minVal);
            Serial.print("Amplitud pico a pico: ");
            Serial.println(amplitude);

            // Imprimir los datos almacenados y sus tiempos
            for (int i = 0; i < index; i++) {
                Serial.print("Dato: ");
                Serial.print(arr[0][i]);
                Serial.print(" Tiempo: ");
                Serial.println(arr[1][i]);
            }

            // Resetear para la próxima adquisición
            maxVal = 0;
            minVal = 1023;
            startTime = millis();  // Reiniciar el temporizador
        }
    }

    delay(500);  // Pausa entre lecturas de los botones
}

