int analogPin = A0;        // Pin de entrada analógica
int val = 0;               // Valor leído del pin analógico
int maxVal = 0;            // Valor máximo de la señal
int minVal = 1023;         // Valor mínimo de la señal
unsigned long startTime;
const unsigned long samplingTime = 1000; // Tiempo de muestreo (1 segundo)
unsigned int Umbralerror=50;
// Configuración de botones
const int botonPin = 2;    // Botón para iniciar la adquisición
const int botonpin2 = 4;   // Botón para detener la adquisición

int capacidad = 10;        // Capacidad inicial del arreglo
int* arrDatos;             // Arreglo dinámico para datos
unsigned long* tiempos;    // Arreglo para almacenar tiempos
int index = 0;             // Índice para almacenar los valores en el arreglo
bool capturandoDatos = false;  // Bandera para determinar si se está capturando

// Función para redimensionar el arreglo
void redArr(int*& arrDatos, unsigned long*& tiempos, int& capacidad) {
    int nuevaCap = capacidad * 2;

    // Redimensionar arreglo de datos
    int* nuevoArrDatos = new int[nuevaCap];
    for (int i = 0; i < capacidad; i++) {
        nuevoArrDatos[i] = arrDatos[i];
    }
    delete[] arrDatos;
    arrDatos = nuevoArrDatos;

    // Redimensionar arreglo de tiempos
    unsigned long* nuevoTiempos = new unsigned long[nuevaCap];
    for (int i = 0; i < capacidad; i++) {
        nuevoTiempos[i] = tiempos[i];
    }
    delete[] tiempos;
    tiempos = nuevoTiempos;

    capacidad = nuevaCap;
}

void setup() {
    Serial.begin(9600);   // Inicializar comunicación serial
    pinMode(botonPin, INPUT);
    pinMode(botonpin2, INPUT);

    // Inicializar los arreglos dinámicos
    arrDatos = new int[capacidad];
    tiempos = new unsigned long[capacidad];
}

void loop() {
    // Verificar estado del botón para iniciar adquisición
    if (digitalRead(botonPin) == HIGH && !capturandoDatos) {
        Serial.println("Iniciando adquisición de datos...");
        capturandoDatos = true;  // Activar adquisición de datos
        startTime = millis();    // Iniciar temporizador
        index = 0;              // Resetear índice
        maxVal = 0;             // Resetear valor máximo
        minVal = 1023;          // Resetear valor mínimo
        delay(500);             // Debounce
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

        // Guardar el valor en el arreglo
        if (index >= capacidad) {
            redArr(arrDatos, tiempos, capacidad);  // Redimensionar el arreglo si es necesario
        }
        arrDatos[index] = val;

        // Guardar el tiempo
        tiempos[index] = millis();

        // Actualizar el valor máximo y mínimo
        if (val > maxVal) {
            maxVal = val;
        }
        if (val < minVal) {
            minVal = val;
        }

        index++;  // Incrementar índice para la siguiente entrada

        delay(1);  // Pausa corta
    }

    delay(100);  // Pausa para evitar múltiples lecturas de los botones
}

void calcularResultados() {
    // Calcular amplitud pico a pico
    int amplitud = maxVal - minVal;
    Serial.print("Amplitud pico a pico: ");
    Serial.println(amplitud);

    // Calcular la frecuencia comparando picos
    Serial.print("la frecuencia es: ");
    Serial.println(Cfrecuencia(arrDatos, tiempos, capacidad,maxVal,Umbralerror));


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
        int diferencia1 = abs(arrDatos[i] - arrDatos[i - 1]);
        int diferencia2 = abs(arrDatos[i + 1] - arrDatos[i]);

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
}/*
float Cfrecuencia(int* arrDatos, unsigned long* tiempos, int capacidad, int amplitud, unsigned int Umbralerror) {
    int count = 0;
    unsigned long time1 = 0, time2 = 0;
    float frecuencia = 0.0;

    // Buscar dos picos máximos en los datos con un margen de error
    for (int i = 0; i < capacidad; i++) {
        if (arrDatos[i] >= (amplitud - Umbralerror) && arrDatos[i] <= (amplitud + Umbralerror)) {
            if (count == 0) {
                time1 = tiempos[i];  // Guardar el primer tiempo
                count++;
            } else if (count == 1) {
                time2 = tiempos[i];  // Guardar el segundo tiempo
                break;               // Salir del bucle cuando se encuentran dos picos
            }
        }
    }

    // Asegurarse de que se hayan encontrado dos tiempos diferentes
    if (time2 > time1) {
        unsigned long periodo = time2 - time1;  // Diferencia de tiempos en ms
        frecuencia = 1000.0 / periodo;          // Calcular la frecuencia en Hz con decimales
    }

    return frecuencia;
}*/
float Cfrecuencia(int* arrDatos, unsigned long* tiempos, int capacidad, int amplitud, unsigned int Umbralerror) {
    int count = 0;
    unsigned long time1 = 0, time2 = 0;
    float frecuencia = 0.0;

    // Buscar dos picos máximos en los datos con un margen de error
    for (int i = 0; i < capacidad; i++) {
        if (arrDatos[i] >= (amplitud - Umbralerror) && arrDatos[i] <= (amplitud + Umbralerror)) {
            if (count == 0) {
                time1 = tiempos[i];  // Guardar el primer tiempo
                /*  Serial.print("Primer pico detectado en tiempo: ");
                Serial.println(time1);  // Debug: mostrar el tiempo del primer pico */
                count++;
            } else if (count == 1) {
                time2 = tiempos[i];  // Guardar el segundo tiempo
                /*Serial.print("Segundo pico detectado en tiempo: ");
                Serial.println(time2);  // Debug: mostrar el tiempo del segundo pico
                */
                break;  // Salir del bucle cuando se encuentran dos picos
            }
        }
    }

    // Asegurarse de que se hayan encontrado dos tiempos diferentes
    if (time2 > time1) {
        unsigned long periodo = time2 - time1;  // Diferencia de tiempos en ms
        frecuencia = 1000.0 / periodo;  // Calcular la frecuencia en Hz con decimales
    } else {
        Serial.println("No se encontraron dos picos para calcular la frecuencia.");
    }

    return frecuencia;
}

