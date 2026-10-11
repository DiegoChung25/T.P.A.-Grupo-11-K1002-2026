#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

// Patrón de Apareo clásico de dos archivos ordenados
bool apareo(const char* nomA, const char* nomB, const char* nomC) {
    FILE* a = fopen(nomA, "rb");
    FILE* b = fopen(nomB, "rb");
    FILE* c = fopen(nomC, "wb");

    if (!a || !b || !c) {
        if (a) fclose(a);
        if (b) fclose(b);
        if (c) fclose(c);
        return false;
    }

    Comanda ca, cb;
    int la = fread(&ca, sizeof(Comanda), 1, a);
    int lb = fread(&cb, sizeof(Comanda), 1, b);

    while (la == 1 && lb == 1) {
        if (ca.idMozo <= cb.idMozo) {
            fwrite(&ca, sizeof(Comanda), 1, c);
            la = fread(&ca, sizeof(Comanda), 1, a);
        } else {
            fwrite(&cb, sizeof(Comanda), 1, c);
            lb = fread(&cb, sizeof(Comanda), 1, b);
        }
    }
    while (la == 1) {
        fwrite(&ca, sizeof(Comanda), 1, c);
        la = fread(&ca, sizeof(Comanda), 1, a);
    }
    while (lb == 1) {
        fwrite(&cb, sizeof(Comanda), 1, c);
        lb = fread(&cb, sizeof(Comanda), 1, b);
    }

    fclose(a);
    fclose(b);
    fclose(c);
    return true;
}

// Función principal que coordina el cierre semanal
bool generarCierreSemanal(int cantidadDias, const char* archivoSemanal) {
    if (cantidadDias <= 0) return false;

    char diaActual[100];
    const char* tempAcumulado = "temp_acum.dat";
    const char* tempNuevo = "temp_nuevo.dat";

    cout << "Ingrese el archivo del dia 1: ";
    cin >> diaActual;

    // Copiamos el primer día al archivo temporal de acumulación
    FILE* fOrigen = fopen(diaActual, "rb");
    FILE* fDestino = fopen(tempAcumulado, "wb");
    if (!fOrigen || !fDestino) {
        if (fOrigen) fclose(fOrigen);
        if (fDestino) fclose(fDestino);
        return false;
    }

    Comanda r;
    while (fread(&r, sizeof(Comanda), 1, fOrigen) == 1) {
        fwrite(&r, sizeof(Comanda), 1, fDestino);
    }
    fclose(fOrigen);
    fclose(fDestino);

    // Procesamos el resto de los días mediante apareos sucesivos
    for (int i = 2; i <= cantidadDias; i++) {
        cout << "Ingrese el archivo del dia " << i << ": ";
        cin >> diaActual;

        if (!apareo(tempAcumulado, diaActual, tempNuevo)) return false;

        // Copiamos el resultado de tempNuevo de vuelta a tempAcumulado
        fOrigen = fopen(tempNuevo, "rb");
        fDestino = fopen(tempAcumulado, "wb");
        while (fread(&r, sizeof(Comanda), 1, fOrigen) == 1) {
            fwrite(&r, sizeof(Comanda), 1, fDestino);
        }
        fclose(fOrigen);
        fclose(fDestino);
    }

    // Copiamos el resultado acumulado final al archivo semanal definitivo
    fOrigen = fopen(tempAcumulado, "rb");
    fDestino = fopen(archivoSemanal, "wb");
    if (!fOrigen || !fDestino) {
        if (fOrigen) fclose(fOrigen);
        if (fDestino) fclose(fDestino);
        return false;
    }

    while (fread(&r, sizeof(Comanda), 1, fOrigen) == 1) {
        fwrite(&r, sizeof(Comanda), 1, fDestino);
    }
    fclose(fOrigen);
    fclose(fDestino);

    return true;
}

int main() {
    int dias;
    char salida[100];

    cout << "=== CIERRE DE LA SEMANA ===" << endl;
    cout << "Cantidad de dias: ";
    cin >> dias;
    cout << "Nombre del archivo semanal de salida: ";
    cin >> salida;

    if (generarCierreSemanal(dias, salida)) {
        cout << "\nCierre completado con exito. Archivo generado: " << salida << endl;
        return 0;
    } else {
        cout << "\nError durante el cierre." << endl;
        return 1;
    }
}