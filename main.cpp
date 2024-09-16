int analogPin = A0;        // Pin de entrada analógica
int val = 0;               // Valor leído del pin analógico
int maxVal = 0;            // Valor máximo de la señal
int minVal = 1023;         // Valor mínimo de la señal
unsigned long startTime;
const unsigned long samplingTime = 1000; // Tiempo de muestreo (1 segundo)

// Configuración de botones
const int botonPin = 2;    // Botón para iniciar la adquisición
const int botonpin2 = 4;   // Botón para detener la adquisición

int capacidad = 10;        // Capacidad inicial del arreglo
int** arr = new int*[2];   // Arreglo dinámico bidimensional (2 filas)
unsigned long* tiempos = newint analogPin = A0;        // Pin de entrada analógica
int val = 0;               // Valor leído del pin analógico
int maxVal = 0;            // Valor máximo de la señal
int minVal = 1023;         // Valor mínimo de la señal
unsigned long startTime;
const unsigned long samplingTime = 1000; // Tiempo de muestreo (1 segundo)

// Configuración de botones
const int botonPin = 2;    // Botón para iniciar la adquisición
const int botonpin2 = 4;   // Botón para detener la adquisición

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
    // Verificar estado del botón para iniciar adquisición
    if (digitalRead(botonPin) == HIGH && !capturandoDatos) {
        Serial.println("Iniciando adquisición de datos...");
        capturandoDatos = true;  // Activar adquisición de datos
        startTime = millis();    // Iniciar temporizador
        delay(500);              // Debounce
    }

    // Verificar estado del segundo botón para detener la adquisición
    if (digitalRead(botonpin2) == HIGH && capturandoDatos) {
        Serial.println("Deteniendo adquisición de datos...");
        capturandoDatos = false;  // Detener adquisición de datos
        delay(500);               // Debounce

        // Calcular amplitud, frecuencia y forma
        calcularResultados();
    }

    // Si la bandera de adquisición está activa, continuar adquiriendo datos
    if (capturandoDatos) {
        val = analogRead(analogPin);

        // Guardar el valor en la primera fila (datos)
        if (index >= capacidad) {
            redArr(arr[0], tiempos, capacidad);  // Redimensionar el arreglo si es necesario
        }
        arr[0][index] = val;

        // Guardar el tiempo en la segunda fila
        arr[1][index] = millis() - startTime;

        index++;  // Incrementar índice para la siguiente entrada

        // Actualizar el valor máximo y mínimo
        if (val > maxVal) {
            maxVal = val;
        }
        if (val < minVal) {
            minVal = val;
        }

        delay(1);  // Pausa corta
    }

    delay(100);  // Pausa para evitar múltiples lecturas de los botones
}

void calcularResultados() {
    // Calcular amplitud pico a pico
    int amplitud = maxVal - minVal;
    Serial.print("Amplitud pico a pico: ");
    Serial.println(amplitud);

    // Calcular la frecuencia
    unsigned long tInicio = tiempos[0];
    unsigned long tFin = tiempos[index - 1];
    float periodo = (tFin - tInicio) / (float)(index - 1); // Tiempo medio entre picos
    float frecuencia = 1 / (periodo / 1000.0); // Convertir a segundos
    Serial.print("Frecuencia estimada: ");
    Serial.println(frecuencia);

    // Determinar forma de la señal
    String forma = determinarForma();
    Serial.print("Forma de la señal: ");
    Serial.println(forma);
}

String determinarForma() {
    // Análisis simple para determinar la forma de la señal
    bool esSenoidal = true;
    bool esTriangular = true;
    bool esCuadrada = true;

    for (int i = 1; i < index - 1; i++) {
        int diferencia1 = abs(arr[0][i] - arr[0][i - 1]);
        int diferencia2 = abs(arr[0][i + 1] - arr[0][i]);

        // Si los cambios no son suaves, no es senoidal
        if (diferencia1 > 10 || diferencia2 > 10) {
            esSenoidal = false;
        }

        // Si los cambios no son lineales, no es triangular
        if (diferencia1 != diferencia2) {
            esTriangular = false;
        }

        // Si los cambios no son abruptos, no es cuadrada
        if (diferencia1 < 100 && diferencia2 < 100) {
            esCuadrada = false;
        }
    }

    if (esSenoidal) return "Senoidal";
    if (esTriangular) return "Triangular";
    if (esCuadrada) return "Cuadrada";
    return "Desconocida";  // Si combina varias formas
}
 unsigned long[capacidad]; // Arreglo para almacenar tiempos
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
    // Verificar estado del botón para iniciar adquisición
    if (digitalRead(botonPin) == HIGH && !capturandoDatos) {
        Serial.println("Iniciando adquisición de datos...");
        capturandoDatos = true;  // Activar adquisición de datos
        startTime = millis();    // Iniciar temporizador
        delay(500);              // Debounce
    }

    // Verificar estado del segundo botón para detener la adquisición
    if (digitalRead(botonpin2) == HIGH && capturandoDatos) {
        Serial.println("Deteniendo adquisición de datos...");
        capturandoDatos = false;  // Detener adquisición de datos
        delay(500);               // Debounce

        // Calcular amplitud, frecuencia y forma
        calcularResultados();
    }

    // Si la bandera de adquisición está activa, continuar adquiriendo datos
    if (capturandoDatos) {
        val = analogRead(analogPin);

        // Guardar el valor en la primera fila (datos)
        if (index >= capacidad) {
            redArr(arr[0], tiempos, capacidad);  // Redimensionar el arreglo si es necesario
        }
        arr[0][index] = val;

        // Guardar el tiempo en la segunda fila
        arr[1][index] = millis() - startTime;

        index++;  // Incrementar índice para la siguiente entrada

        // Actualizar el valor máximo y mínimo
        if (val > maxVal) {
            maxVal = val;
        }
        if (val < minVal) {
            minVal = val;
        }

        delay(1);  // Pausa corta
    }

    delay(100);  // Pausa para evitar múltiples lecturas de los botones
}

void calcularResultados() {
    // Calcular amplitud pico a pico
    int amplitud = maxVal - minVal;
    Serial.print("Amplitud pico a pico: ");
    Serial.println(amplitud);

    // Calcular la frecuencia
    unsigned long tInicio = tiempos[0];
    unsigned long tFin = tiempos[index - 1];
    float periodo = (tFin - tInicio) / (float)(index - 1); // Tiempo medio entre picos
    float frecuencia = 1 / (periodo / 1000.0); // Convertir a segundos
    Serial.print("Frecuencia estimada: ");
    Serial.println(frecuencia);

    // Determinar forma de la señal
    String forma = determinarForma();
    Serial.print("Forma de la señal: ");
    Serial.println(forma);
}

String determinarForma() {
    // Análisis simple para determinar la forma de la señal
    bool esSenoidal = true;
    bool esTriangular = true;
    bool esCuadrada = true;

    for (int i = 1; i < index - 1; i++) {
        int diferencia1 = abs(arr[0][i] - arr[0][i - 1]);
        int diferencia2 = abs(arr[0][i + 1] - arr[0][i]);

        // Si los cambios no son suaves, no es senoidal
        if (diferencia1 > 10 || diferencia2 > 10) {
            esSenoidal = false;
        }

        // Si los cambios no son lineales, no es triangular
        if (diferencia1 != diferencia2) {
            esTriangular = false;
        }

        // Si los cambios no son abruptos, no es cuadrada
        if (diferencia1 < 100 && diferencia2 < 100) {
            esCuadrada = false;
        }
    }

    if (esSenoidal) return "Senoidal";
    if (esTriangular) return "Triangular";
    if (esCuadrada) return "Cuadrada";
    return "Desconocida";  // Si combina varias formas
}
