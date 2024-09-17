#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

int analogPin = A0;        // Pin de entrada analógica
int val = 0;               // Valor leído del pin analógico
int maxVal = 0;            // Valor máximo de la señal
int minVal = 1023;         // Valor mínimo de la señal
unsigned long startTime;
const unsigned long samplingTime = 1000; // Tiempo de muestreo (1 segundo)
unsigned int Umbralerror = 50;

// Configuración de botones
const int botonPin = 2;    // Botón para iniciar la adquisición
const int botonpin2 = 4;   // Botón para detener la adquisición

const int capacidadInicial = 100; // Tamaño inicial estimado
int capacidad = capacidadInicial;
int* arrDatos;             // Arreglo dinámico para datos
unsigned long* tiempos;    // Arreglo para almacenar tiempos
unsigned int index = 0;    // Índice para almacenar los valores en el arreglo
bool capturandoDatos = false;  // Bandera para determinar si se está capturando

void setup() {
    Serial.begin(9600);   // Inicializar comunicación serial
    pinMode(botonPin, INPUT);
    pinMode(botonpin2, INPUT);
    lcd.begin(16, 2);
    inicializarMemoria(); // Reservar memoria al iniciar el programa
}

void loop() {
    if (digitalRead(botonPin) == HIGH && !capturandoDatos) {
        Serial.println("Iniciando adquisición de datos...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Adquiriendo datos...");
        capturandoDatos = true;
        startTime = millis();
        index = 0;
        maxVal = 0;
        minVal = 1023;
        delay(500); // Debounce

        // Volver a inicializar la memoria antes de recolectar nuevos datos
        inicializarMemoria();
    }

    if (digitalRead(botonpin2) == HIGH && capturandoDatos) {
        Serial.println("Deteniendo adquisición de datos...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Deteniendo datos...");
        capturandoDatos = false;
        delay(500); // Debounce

        // Calcular resultados
        calcularResultados();

        // Liberar memoria después de usar los arreglos
        liberarMemoria();
    }

    if (capturandoDatos) {
        val = analogRead(analogPin);

        // Guardar el valor en el arreglo
        if (index >= capacidad) {
            redArr(arrDatos, tiempos, capacidad);
        }
        arrDatos[index] = val;
        tiempos[index] = millis();

        if (val > maxVal) maxVal = val;
        if (val < minVal) minVal = val;

        index++;
        delay(100);  // Pausa corta
    }

    delay(100); // Pausa para evitar múltiples lecturas de los botones
}

void redArr(int*& arrDatos, unsigned long*& tiempos, int& capacidad) {
    int nuevaCap = capacidad * 2;
    int* nuevoArrDatos = new int[nuevaCap];
    for (unsigned int i = 0; i < capacidad; i++) {
        nuevoArrDatos[i] = arrDatos[i];
    }
    delete[] arrDatos;
    arrDatos = nuevoArrDatos;

    unsigned long* nuevoTiempos = new unsigned long[nuevaCap];
    for (unsigned int i = 0; i < capacidad; i++) {
        nuevoTiempos[i] = tiempos[i];
    }
    delete[] tiempos;
    tiempos = nuevoTiempos;

    capacidad = nuevaCap;
}

void calcularResultados() {
    int amplitud = maxVal - minVal;
    lcd.setCursor(0, 0);
    lcd.print("Amplitud: ");
    lcd.print(amplitud);

    float frecuencia = Cfrecuencia(arrDatos, tiempos, capacidad, maxVal, Umbralerror);
    lcd.setCursor(0, 1);
    lcd.print("Frecuencia: ");
    lcd.print(frecuencia);

    delay(2000);  // Pausa para mostrar resultados
    lcd.clear();

    int forma = determinarForma();
    lcd.setCursor(0, 0);
    lcd.print("Forma Señal: ");
    lcd.setCursor(0, 1);
    lcd.print(forma);  // Mostrar el tipo de señal en formato numérico
    delay(2000);  // Pausa para mostrar la forma
    lcd.clear();
}

int determinarForma() {
    const int UMBRAL_SENOIDAL = 10;  // Umbral para cambios suaves
    const int UMBRAL_CUADRADA = 100; // Umbral para cambios abruptos

    bool esSenoidal = true;
    bool esTriangular = true;
    bool esCuadrada = true;

    for (unsigned int i = 1; i < index - 1; i++) {
        int diferencia1 = abs(arrDatos[i] - arrDatos[i - 1]);
        int diferencia2 = abs(arrDatos[i + 1] - arrDatos[i]);

        // Verificar si la señal es cuadrada
        if (diferencia1 < UMBRAL_CUADRADA && diferencia2 < UMBRAL_CUADRADA) {
            esCuadrada = false;
            break;  // Si se detecta que no es cuadrada, no es necesario seguir verificando
        }

        // Verificar si la señal es triangular
        if (abs(diferencia1 - diferencia2) > UMBRAL_SENOIDAL) {
            esTriangular = false;
        }

        // Verificar si la señal es senoidal
        if (diferencia1 > UMBRAL_SENOIDAL || diferencia2 > UMBRAL_SENOIDAL) {
            esSenoidal = false;
        }
    }

    if (esCuadrada) return 2;  // Cuadrada
    if (esTriangular) return 1; // Triangular
    if (esSenoidal) return 0;  // Senoidal
    return 3; // Desconocida
}

float Cfrecuencia(int* arrDatos, unsigned long* tiempos, int capacidad, int amplitud, unsigned int Umbralerror) {
    unsigned int count = 0;
    unsigned long time1 = 0, time2 = 0;
    float frecuencia = 0.0;

    for (unsigned int i = 0; i < capacidad; i++) {
        if (arrDatos[i] >= (amplitud - Umbralerror) && arrDatos[i] <= (amplitud + Umbralerror)) {
            if (count == 0) {
                time1 = tiempos[i];
                count++;
            } else if (count == 1) {
                time2 = tiempos[i];
                break;
            }
        }
    }

    if (time2 > time1) {
        unsigned long periodo = time2 - time1;
        frecuencia = 1000.0 / periodo;
    } else {
        Serial.println("No se encontraron dos picos para calcular la frecuencia.");
    }

    return frecuencia;
}

void liberarMemoria() {
    delete[] arrDatos;
    delete[] tiempos;
    arrDatos = nullptr;
    tiempos = nullptr;
}

void inicializarMemoria() {
    // Reservar nueva memoria para los arreglos
    arrDatos = new int[capacidad];
    tiempos = new unsigned long[capacidad];
}

