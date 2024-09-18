#include <LiquidCrystal.h>

// Inicialización del LCD (usa los pines correctos según tu conexión)
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

int analogPin = A0;  // Pin de entrada analógica
int val = 0;         // Valor leído del pin analógico
unsigned long startTime;
const unsigned long samplingTime = 1000;  // Tiempo de muestreo (1 segundo)

// Configuración de botones
const int botonPin = 2;  // Botón para iniciar la adquisición
const int botonpin2 = 4; // Botón para detener la adquisición

const int capacidadInicial = 100; // Tamaño inicial estimado
int capacidad = capacidadInicial;
int* arrDatos;            // Arreglo dinámico para datos
unsigned long* tiempos;   // Arreglo para almacenar tiempos
unsigned int index = 0;   // Índice para almacenar los valores en el arreglo
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
        delay(500);  // Debounce

        inicializarMemoria(); // Volver a inicializar la memoria
    }

    if (digitalRead(botonpin2) == HIGH && capturandoDatos) {
        Serial.println("Deteniendo adquisición de datos...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Deteniendo datos...");
        capturandoDatos = false;
        delay(500);  // Debounce

        // Calcular los resultados
        calcularResultados();

        liberarMemoria();  // Liberar memoria después de usar los arreglos
    }

    if (capturandoDatos) {
        val = analogRead(analogPin);

        // Guardar el valor en el arreglo
        if (index >= capacidad) {
            redimensionarMemoria(arrDatos, tiempos, capacidad);
        }
        arrDatos[index] = val;
        tiempos[index] = millis();
        index++;

        delay(50);  // Ajuste del tiempo de muestreo para mayor precisión
    }

    delay(100);  // Pausa para evitar múltiples lecturas de los botones
}

// Redimensionar el arreglo dinámico si es necesario
void redimensionarMemoria(int*& arrDatos, unsigned long*& tiempos, int& capacidad) {
    int nuevaCapacidad = capacidad * 2;
    int* nuevoArrDatos = new int[nuevaCapacidad];
    for (unsigned int i = 0; i < capacidad; i++) {
        nuevoArrDatos[i] = arrDatos[i];
    }
    delete[] arrDatos;
    arrDatos = nuevoArrDatos;

    unsigned long* nuevoTiempos = new unsigned long[nuevaCapacidad];
    for (unsigned int i = 0; i < capacidad; i++) {
        nuevoTiempos[i] = tiempos[i];
    }
    delete[] tiempos;
    tiempos = nuevoTiempos;

    capacidad = nuevaCapacidad;
}

// Calcular los resultados
void calcularResultados() {
    // Calcular la amplitud
    float amplitud = calcularAmplitud(arrDatos, index);
    lcd.setCursor(0, 0);
    lcd.print("Amplitud: ");
    lcd.print(amplitud);

    // Calcular la frecuencia
    float frecuencia = calcularFrecuencia(arrDatos, tiempos, index);
    lcd.setCursor(0, 1);
    lcd.print("Frecuencia: ");
    lcd.print(frecuencia);

    delay(2000);  // Pausa para mostrar resultados
    lcd.clear();

    // Determinar la forma de la señal
    int forma = determinarForma(arrDatos, index);
    lcd.setCursor(0, 0);
    lcd.print("Forma Señal: ");
    lcd.setCursor(0, 1);
    switch (forma) {
    case 0:
        lcd.print("Senoideal");
        break;
    case 1:
        lcd.print("Triangular");
        break;
    case 2:
        lcd.print("Cuadrada");
        break;
    default:
        lcd.print("Desconocida");
        break;
    }
    delay(2000);  // Pausa para mostrar la forma
    lcd.clear();
}

// Calcular la amplitud de la señal (pico a pico)
float calcularAmplitud(int* arrDatos, int capacidad) {
    int maxVal = arrDatos[0];
    int minVal = arrDatos[0];
    for (int i = 1; i < capacidad; i++) {
        if (arrDatos[i] > maxVal) maxVal = arrDatos[i];
        if (arrDatos[i] < minVal) minVal = arrDatos[i];
    }
    return (maxVal - minVal) / 2.0;  // Amplitud es la mitad del valor pico a pico
}

// Calcular la frecuencia de la señal en Hz
float calcularFrecuencia(int* arrDatos, unsigned long* tiempos, int capacidad) {
    unsigned long tiempoTotal = tiempos[capacidad - 1] - tiempos[0];
    int conteoCiclos = 0;
    for (int i = 1; i < capacidad; i++) {
        if (arrDatos[i] > arrDatos[i - 1] && arrDatos[i] > (arrDatos[i - 1] + 10)) {
            conteoCiclos++;
        }
    }
    return conteoCiclos / (tiempoTotal / 1000.0);  // Frecuencia en Hz
}

// Determinar la forma de la señal
int determinarForma(int* arrDatos, int capacidad) {
    int conteoCiclos = 0;
    int conteoCambios = 0;
    for (int i = 1; i < capacidad; i++) {
        if (arrDatos[i] > arrDatos[i - 1] && arrDatos[i] > (arrDatos[i - 1] + 10)) {
            conteoCiclos++;
        }
        if (abs(arrDatos[i] - arrDatos[i - 1]) > 50) {
            conteoCambios++;
        }
    }
    if (conteoCiclos > 2 && conteoCambios < 2) return 0; // Senoidal
    else if (conteoCiclos > 2 && conteoCambios > 2) return 1; // Triangular
    else if (conteoCiclos < 2 && conteoCambios > 2) return 2; // Cuadrada
    else return 3; // Desconocida
}

// Liberar la memoria después de su uso
void liberarMemoria() {
    delete[] arrDatos;
    delete[] tiempos;
    arrDatos = nullptr;
    tiempos = nullptr;
}

// Inicializar la memoria para los arreglos dinámicos
void inicializarMemoria() {
    arrDatos = new int[capacidad];
    tiempos = new unsigned long[capacidad];
}
