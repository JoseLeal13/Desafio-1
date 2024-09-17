int determinarForma(int* arrDatos, int index) {
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

    if (esSenoidal) return 0; // Senoidal
    if (esTriangular) return 1; // Triangular
    if (esCuadrada) return 2; // Cuadrada
    return 3; // Desconocida
}

void calcularResultados(int* arrDatos, unsigned long* tiempos, int index) {
    // Calcular amplitud pico a pico
    int maxVal = 0;
    int minVal = 1023;
    for (int i = 0; i < index; i++) {
        if (arrDatos[i] > maxVal) {
            maxVal = arrDatos[i];
        }
        if (arrDatos[i] < minVal) {
            minVal = arrDatos[i];
        }
    }
    int amplitud = maxVal - minVal;
    Serial.print("Amplitud pico a pico: ");
    Serial.println(amplitud);

    // Calcular la frecuencia comparando picos
    Serial.print("Frecuencia estimada: ");
    Serial.println(Cfrecuencia(arrDatos, tiempos, index, amplitud, Umbralerror));

    // Determinar forma de la señal
    int forma = determinarForma(arrDatos, index);
    Serial.print("Forma de la señal: ");
    switch (forma) {
    case 0: Serial.println("Senoidal"); break;
    case 1: Serial.println("Triangular"); break;
    case 2: Serial.println("Cuadrada"); break;
    case 3: Serial.println("Desconocida"); break;
    }
}
