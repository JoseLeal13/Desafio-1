#include <iostream>
using namespace std;

void reDimArr(int &cap, int*& arr){

    int nuevaCap = cap*2;
    int* nuevoArr = new int[nuevaCap];
    copy(arr,arr+cap,nuevoArr);
    delete[] arr;
    arr = nuevoArr;
    cap = nuevoArr;

}
int main()
{
    int cap = 2;
    int* arr = new int[cap];
    int cantElementos = 0;
    char opcion;
    do
    {
        if(canElementos == cap){
            reDimArr(cap, arr);
        }
    }while (true);

    cout << "Ingrese un valor: "
    cin >> arr[cantElementos];
    cantElementos++;
}
while (opcion =="s"){
    cout<< "desea continuar escriba 's', si no escriba 'n': " << endl;
    cin >> opcion << endl;
    }
cout << arr; << endl;

