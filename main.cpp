#include <iostream>
using namespace std;
void redArr(int*& arr, int& capacidad){

    int nuevaCap = capacidad * 2;
    int* nuevoArr = new int[nuevaCap];
    copy(arr,arr+capacidad,nuevoArr);

    delete[] arr;
    arr = nuevoArr;

    capacidad = nuevaCap;
}


int main()
{
    int analogPin = A0; // Pin de entrada analógica
    int val = 0;
    int maxVal = 0;
    int minVal = 1023; // Inicializamos al valor máximo posible del ADC
    unsigned long startTime;
    const unsigned long samplingTime = 1000; // 1 segundo de muestreo

    void setup() {
        Serial.begin(9600);
    }

    void loop() {
        startTime = millis();
        while (millis() - startTime < samplingTime) {
            val = analogRead(analogPin);

            // Actualizar el valor máximo y mínimo
            if (val > maxVal) {
                maxVal = val;
            }
            if (val < minVal) {
                minVal = val;
            }

            delay(1); // Pequeña pausa para no saturar el muestreo
        }

        // Calcular la amplitud pico a pico
        int amplitude = maxVal - minVal;

        // Imprimir resultados
        Serial.print("Valor máximo: ");
        Serial.println(maxVal);
        Serial.print("Valor mínimo: ");
        Serial.println(minVal);
        Serial.print("Amplitud pico a pico: ");
        Serial.println(amplitude);

        // Resetear para la próxima lectura
        maxVal = 0;
        minVal = 1023;

        delay(2000); // Esperar 2 segundos antes de la próxima medición
    }

/*
    int capacidad = 2;
    int* arr = new int[capacidad];
    int cantElementos=0;
    int nroIngresado=0;
    char continuar;

    do{

        if(cantElementos == capacidad){
            redArr(arr,capacidad);
            int nuevaCap = capacidad * 2;
            int* nuevoArr = new int[nuevaCap];
            std::copy(arr,arr+capacidad,nuevoArr);

            delete[] arr;
            arr = nuevoArr;

            capacidad = nuevaCap;
        }


        cout << "Ingresa un numero: ";
        cin >> nroIngresado;
        arr[cantElementos] = nroIngresado;
        cantElementos++;

        cout << "Si desea continuar digite s, si desea parar digite n :";
        cin >> continuar;

    }while(continuar == 's' || continuar == 'S');

    for(int i = 0; i < cantElementos; i++){
        cout << arr[i] << endl;
    }   
    delete[] arr;
    return 0;
*/
}



