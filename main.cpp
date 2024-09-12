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
    int capacidad = 2;
    int* arr = new int[capacidad];
    int cantElementos=0;
    int nroIngresado=0;
    char continuar;

    do{

        if(cantElementos == capacidad){
            redArr(arr,capacidad);
            /*int nuevaCap = capacidad * 2;
            int* nuevoArr = new int[nuevaCap];
            std::copy(arr,arr+capacidad,nuevoArr);

            delete[] arr;
            arr = nuevoArr;

            capacidad = nuevaCap;*/
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

}


