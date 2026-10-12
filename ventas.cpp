#include <cstring> 
#include <iostream>
#include <cstdio>

using namespace std;

const int CORRIMIENTO = 5;
const float TASA_COMISION = 0.10f;

struct ComandaHistorica
{
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
};

struct Mozo
{
    int idMozo; 
    char nombre[50]; 
    char password[20]; 
    float totalComision;
};

struct Comanda
{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};


float calcularComision(float precio, int cantidad)
{
    return precio * cantidad * TASA_COMISION;
}

bool encriptarYValidar(char clave[], int len, Mozo mozo)
{

    char claveEncriptada[len];

    int i = 0;
    while (i < len)
    {
       claveEncriptada[i] = clave[i] + CORRIMIENTO;
       i++;
    }

    return strcmp(claveEncriptada, mozo.password) == 0; 
       
}

void crearNombreArchivo(char fecha[], char nombreArchivo[])
{
    strcpy(nombreArchivo, "comandas_");
    strcat(nombreArchivo, fecha);
    strcat(nombreArchivo, ".dat");   
}

long buscarMozo(const char nombreArchivo[], int idBuscada, Mozo &m)
{
    FILE* f = fopen(nombreArchivo, "rb");

    if(f == NULL)
    {
        cout << "No se pudo abrir el archivo de mozos." << endl;
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Mozo);
    long primero = 0, ultimo = n - 1, pos = -1;

    while(primero <= ultimo && pos == -1)
    {
        long medio = (primero + ultimo) / 2;
        fseek(f, medio * sizeof(Mozo), SEEK_SET);
        fread(&m, sizeof(Mozo), 1, f);
        if(m.idMozo == idBuscada) pos = medio;
        else if(idBuscada > m.idMozo) primero = medio + 1;
        else ultimo = medio - 1; 
    }

    fclose(f);
    return pos;

}

long buscarProducto(const char nombreArchivo[], int codigo, Producto &p)
{
    FILE* f = fopen(nombreArchivo, "rb");

    if(f == NULL)
    {
        cout << "No se pudo abrir el archivo de productos." << endl;
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Producto);
    long primero = 0, ultimo = n - 1, pos = -1;

    while(primero <= ultimo && pos == -1)
    {
        long medio = (primero + ultimo) / 2;
        fseek(f, medio * sizeof(Producto), SEEK_SET);
        fread(&p, sizeof(Producto), 1, f);
        if(p.codigo == codigo) pos = medio;
        else if(codigo > p.codigo) primero = medio + 1;
        else ultimo = medio - 1;
    }
    fclose(f);
    return pos;
}

bool hayStock(const char nombreArchivo[], long pos, Producto &p, int cantidad)
{
    if(cantidad <= 0) return false;

    FILE* f = fopen(nombreArchivo, "rb");

    if(f == NULL)
    {
        cout << "No se pudo abrir el inventario" << endl;
        return false;
    }

    fseek(f, pos * sizeof(Producto), SEEK_SET);
    fread(&p, sizeof(Producto), 1, f);

    fclose(f);

    return cantidad <= p.stockActual;
}

void ordenarComandas(const char nombreArchivo[])
{

    FILE* f = fopen(nombreArchivo, "rb+");

    if(f == NULL)
    {
        cout << "No se pudo abrir el archivo de comandas" << endl;
        return;
    }

    fseek(f, 0, SEEK_END);
    long cantidadComandas = ftell(f) / sizeof(Comanda);

    for(long i = 0; i < cantidadComandas - 1; i++)
    {
        for(long j = 0; j < cantidadComandas - 1 - i; j++)
        {
            Comanda c1, c2;

            fseek(f, j * sizeof(Comanda), SEEK_SET);
            fread(&c1, sizeof(Comanda), 1, f);

            fseek(f, (j+1) * sizeof(Comanda), SEEK_SET);
            fread(&c2, sizeof(Comanda), 1, f);

            if(c1.idMozo > c2.idMozo)
            {
                fseek(f, j * sizeof(Comanda), SEEK_SET);
                fwrite(&c2, sizeof(Comanda), 1, f);

                fseek(f, (j+1)*sizeof(Comanda), SEEK_SET);
                fwrite(&c1, sizeof(Comanda), 1, f);
            }   
        }
    }

    fclose(f);

}


int main(int argc, char const *argv[])
{
    char fecha[20];
    char nombreArchivo[50];
    cout << "Ingrese la fecha (DD-MM-AAAA): " << endl;
    cin >> fecha;

    crearNombreArchivo(fecha, nombreArchivo);

    FILE* archivo = fopen(nombreArchivo, "ab+");

    if(archivo == NULL)
    {
        cout << "No se pudo crear o abrir el archivo del dia." << endl;
        return 1; 
    }
    
    bool continuar = true;

    while(continuar){   

        int idMozo;
        cout << "Ingrese id del Mozo: " << endl;

        while(!(cin >> idMozo)){
            cin.clear();
            cin.ignore(10000,'\n');
            cout << "Entrada invalida. Ingrese un ID numerico valido: " << endl;
        }

        Mozo m;

        long posicionMozo = buscarMozo("mozos.dat", idMozo, m);

        while(posicionMozo == -1)
        {
            cout << "No se encontró un mozo con esa ID, intente nuevamente: " << endl;
            while(!(cin >> idMozo)){
                cin.clear();
                cin.ignore(10000,'\n');
                cout << "Entrada invalida. Ingrese un ID numerico valido: " << endl;
            }

            posicionMozo = buscarMozo("mozos.dat", idMozo, m);
        }

        char clave[20];
        cout << "Ingrese la clave: " << endl;
        cin >> clave;

        if(!encriptarYValidar(clave, 20, m))
        {
            cout << "Clave incorrecta" << endl;
        }
        else
        {
            Producto p;
            int codigoProducto;

            cout << "Ingrese codigo de producto" << endl;
            while(!(cin >> codigoProducto))
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Entrada invalida. Ingrese un codigo numerico valido" << endl;
            }
            long posicionProducto = buscarProducto("inventario.dat", codigoProducto, p);
            while(posicionProducto == -1)
            {
                cout << "No se encontró el producto, intente nuevamente: " << endl;
                while(!(cin >> codigoProducto)){
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Entrada invalida. Ingrese un codigo numerico valido" << endl;
                }
                posicionProducto = buscarProducto("inventario.dat", codigoProducto, p); 
            }
            int cantidad;
            cout << "Ingrese cantidad: " << endl;
            while(!(cin>>cantidad) || cantidad <= 0)
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Cantidad invalida. Ingrese un numero mayor a 0" << endl;
            }
            if(hayStock("inventario.dat", posicionProducto, p, cantidad))
            {
                
                float comision = calcularComision(p.precio, cantidad);
                Comanda c;
                c.idMozo = idMozo;
                c.codigoProducto = codigoProducto;
                c.cantidad = cantidad;
                c.comision = comision;

                //Guarda la comanda al final del archivo del día
                fseek(archivo, 0, SEEK_END);
                fwrite(&c, sizeof(Comanda), 1, archivo);

                //Actualiza el stock del producto
                p.stockActual -= cantidad;

                FILE* inventario = fopen("inventario.dat", "rb+");

                if(inventario == NULL)
                {
                    cout << "No se pudo abrir el inventario" << endl;
                }
                else
                {
                fseek(inventario, posicionProducto * sizeof(Producto), SEEK_SET);
                fwrite(&p, sizeof(Producto), 1, inventario);
                fclose(inventario);
                }
                cout << "Venta registrada con exito" << endl;
            } else
            {
                cout << "Cantidad invalida o stock insuficiente" << endl;
            }

        }
        char input;
        cout << "Ingrese '1' para cargar otra venta, presione cualquier otra tecla para terminar la operación" << endl;
        cin >> input;

        continuar = (input == '1');
    }   

    fclose(archivo);

    ordenarComandas(nombreArchivo);

    cout << "Proceso finalizado con exito" << endl;

    return 0;
    
}
