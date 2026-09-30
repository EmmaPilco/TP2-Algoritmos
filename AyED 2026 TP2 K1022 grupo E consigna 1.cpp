#include <iostream>
#include <cstring>
using namespace std;

struct RegCorredores{
    int numero;
    char nombreApellido[50];
    char categoria[50];
    char genero;
    char localidad[40];
    char llegada[11];
};

struct CorredorProcesado{
    RegCorredores datos;

    int tiempoSegundos;
    bool termino;
    int posGeneral;
    int posGenero;
    int posCat;
    int difPrimero;
    int difAnterior;
};

int convertirASegundos(const char llegada[]);
void convertirAStringTiempo(int decSeg, char destino[]);
void procesarCarrera(const char* tituloCarrera, CorredorProcesado lista[], int cant);

int main(){
    char carpetaRuta[]= "C:/Users/emmanuelp148/Documents/Emmanuel Pilco1/UTN/Algortimos y Estructura de datos/Ejercicios practicos/";
    char nombreDelArchivo[]= "Archivo corredores 4Refugios.bin";
    char ruta[300];
    strcpy(ruta, carpetaRuta);
    strcat(ruta, nombreDelArchivo);
    FILE *f = fopen(ruta, "rb+");

    if (!f) {
        cout << "No se pudo abrir el archivo principal en la ruta: " << ruta << endl;
        return 1;
    }

    RegCorredores aux;
    while(fread(&aux, sizeof(RegCorredores), 1, f)){
        if(strcmp(aux.llegada, "DNF") == 0 || strncmp(aux.llegada, "DNF", 3) == 0 || strncmp(aux.llegada, "DSQ", 3) == 0){
            strcpy(aux.llegada, "No termino");
            fseek(f, -sizeof(RegCorredores), SEEK_CUR);
            fwrite(&aux, sizeof(RegCorredores), 1, f);
            fseek(f, 0, SEEK_CUR);
        }
    }

    CorredorProcesado *clasica = new CorredorProcesado[1000];
    CorredorProcesado *nonstop = new CorredorProcesado[1000];
    int cantClasica = 0, cantNonstop = 0;

    rewind(f);
    while(fread(&aux, sizeof(RegCorredores), 1, f)){
        CorredorProcesado cp;
        cp.datos= aux;
        cp.tiempoSegundos= convertirASegundos(aux.llegada);
        cp.termino= (cp.tiempoSegundos >= 0);

        if(strstr(aux.categoria, "Clasica") != NULL){
            clasica[cantClasica++] = cp;
        } else if(strstr(aux.categoria, "NonStop") != NULL){
            nonstop[cantNonstop++] = cp;
        }
    }
    fclose(f);

    for (int i = 0; i < cantClasica - 1; i++) {
        for (int j = i + 1; j < cantClasica; j++) {
            bool cambiar = false;
            if (!clasica[i].termino && clasica[j].termino) {
                cambiar = true;
            } else if (clasica[i].termino && clasica[j].termino) {
                if (clasica[j].tiempoSegundos < clasica[i].tiempoSegundos) cambiar = true;
            }
            if (cambiar) {
                CorredorProcesado temp = clasica[i];
                clasica[i] = clasica[j];
                clasica[j] = temp;
            }
        }
    }

    for(int i = 0; i < cantNonstop - 1; i++){
        for(int j = i + 1; j < cantNonstop; j++){
            bool cambiar = false;
            if(!nonstop[i].termino && nonstop[j].termino){
                cambiar = true;
            } else if(nonstop[i].termino && nonstop[j].termino){
                if(nonstop[j].tiempoSegundos < nonstop[i].tiempoSegundos){
                    cambiar = true;
                }
            }
            if(cambiar){
                CorredorProcesado temp = nonstop[i];
                nonstop[i] = nonstop[j];
                nonstop[j] = temp;
            }
        }
    }

    int primerTiempoClasico = -1;
    int posGralValidaC = 1;
    for(int i = 0; i < cantClasica; i++){
        if(clasica[i].termino){
            clasica[i].posGeneral = posGralValidaC++;
            if(primerTiempoClasico == -1){
                primerTiempoClasico = clasica[i].tiempoSegundos;
            }
            clasica[i].difPrimero = clasica[i].tiempoSegundos - primerTiempoClasico;

            if(posGralValidaC == 2){
                clasica[i].difAnterior = 0;
            } else{
                int tiempoAnt = 0;
                for(int k = 0; k < i; k++){
                    if(clasica[k].posGeneral == clasica[i].posGeneral - 1){
                        tiempoAnt = clasica[k].tiempoSegundos;
                    }
                }
                clasica[i].difAnterior = clasica[i].tiempoSegundos - tiempoAnt;
            }
        } else {
            clasica[i].posGeneral = 0;
            clasica[i].difPrimero = -1;
            clasica[i].difAnterior = -1;
        }

        int pGen = 1, pCat = 1;
        for (int k = 0; k < i; k++) {
            if (clasica[k].termino && clasica[k].datos.genero == clasica[i].datos.genero){
                pGen++;
            }
            if (clasica[k].termino && strcmp(clasica[k].datos.categoria, clasica[i].datos.categoria) == 0){
                pCat++;
            }
        }
        clasica[i].posGenero = clasica[i].termino ? pGen : 0;
        clasica[i].posCat = clasica[i].termino ? pCat : 0;
    }

    int primerTiempoNonStop = -1;
    int posGralValidaN = 1;
    for(int i = 0; i < cantNonstop; i++){
        if (nonstop[i].termino) {
            nonstop[i].posGeneral = posGralValidaN++;
            if (primerTiempoNonStop == -1) primerTiempoNonStop = nonstop[i].tiempoSegundos;
            nonstop[i].difPrimero = nonstop[i].tiempoSegundos - primerTiempoNonStop;
            
            if (posGralValidaN == 2) {
                nonstop[i].difAnterior = 0;
            } else {
                int tiempoAnt = 0;
                for(int k = 0; k < i; k++) {
                    if(nonstop[k].posGeneral == nonstop[i].posGeneral - 1) tiempoAnt = nonstop[k].tiempoSegundos;
                }
                nonstop[i].difAnterior = nonstop[i].tiempoSegundos - tiempoAnt;
            }
        } else {
            nonstop[i].posGeneral = 0;
            nonstop[i].difPrimero = -1;
            nonstop[i].difAnterior = -1;
        }

        int pGen = 1, pCat = 1;
        for(int k = 0; k < i; k++){
            if(nonstop[k].termino && nonstop[k].datos.genero == nonstop[i].datos.genero){
                pGen++;
            }
            if(nonstop[k].termino && strcmp(nonstop[k].datos.categoria, nonstop[i].datos.categoria) == 0){
                pCat++;
            }
        }
        nonstop[i].posGenero = nonstop[i].termino ? pGen : 0;
        nonstop[i].posCat = nonstop[i].termino ? pCat : 0;
    }

    procesarCarrera("4 Refugios Clasica", clasica, cantClasica);
    procesarCarrera("4 Refugios NonStop", nonstop, cantNonstop);

    FILE *fClasica = fopen("clasica_procesada.bin", "wb");
    for(int i = 0; i < cantClasica; i++) {
        fwrite(&clasica[i], sizeof(CorredorProcesado), 1, fClasica);
    }
    fclose(fClasica);

    FILE *fNonstop = fopen("nonstop_procesada.bin", "wb");
    for(int i = 0; i < cantNonstop; i++) {
        fwrite(&nonstop[i], sizeof(CorredorProcesado), 1, fNonstop);
    }
    fclose(fNonstop);

    delete[] clasica;
    delete[] nonstop;

    return 0;
}

int convertirASegundos(const char llegada[]){
    if(strcmp(llegada, "No termino") == 0 || strcmp(llegada, "No Termino") == 0 || strcmp(llegada, "DNF") == 0 || strncmp(llegada, "DNF", 3) == 0 || strncmp(llegada, "DSQ", 3) == 0){
        return -1;
    }
    int h = (llegada[0] - '0') * 10 + (llegada[1] - '0');
    int m = (llegada[3] - '0') * 10 + (llegada[4] - '0');
    int s = (llegada[6] - '0') * 10 + (llegada[7] - '0');
    int d = (llegada[9] - '0');
    return (h * 3600 + m * 60 + s) * 10 + d;
}

void convertirAStringTiempo(int decSeg, char destino[]) {
    if (decSeg < 0) {
        strcpy(destino, "No Termino");
        return;
    }
    int sTotales = decSeg / 10;
    int d = decSeg % 10;
    int h = sTotales / 3600;
    int m = (sTotales % 3600) / 60;
    int s = sTotales % 60;

    sprintf(destino, "%02d:%02d:%02d.%d", h, m, s, d);
}

void procesarCarrera(const char* tituloCarrera, CorredorProcesado lista[], int cant){
    cout << "---------------------------------------------------------------------------------------------------------" << endl;
    cout << " REPORTE: " << tituloCarrera << endl;
    cout << "---------------------------------------------------------------------------------------------------------" << endl;
    cout << "Pos.Gral | Pos.Gen | Pos.Cat | N  | Nombre              | Categoria                  | Gen | Localidad | Total      | Dif. 1ro   | Dif. Ant." << endl;
    cout << "---------------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < cant; i++) {
        char sTotal[15], sDif1[15], sDifAnt[15];
        convertirAStringTiempo(lista[i].tiempoSegundos, sTotal);
        convertirAStringTiempo(lista[i].difPrimero, sDif1);
        convertirAStringTiempo(lista[i].difAnterior, sDifAnt);

        if (lista[i].tiempoSegundos < 0) {
            strcpy(sTotal, "No Termino");
            strcpy(sDif1, "");
            strcpy(sDifAnt, "");
        }

        cout << lista[i].posGeneral << "\t | " 
             << lista[i].posGenero << "\t | " 
             << lista[i].posCat << "\t | " 
             << lista[i].datos.numero << " | " 
             << lista[i].datos.nombreApellido << " | " 
             << lista[i].datos.categoria << " | " 
             << lista[i].datos.genero << " | " 
             << lista[i].datos.localidad << " | " 
             << sTotal << " | " 
             << sDif1 << " | " 
             << sDifAnt << endl;
    }
    cout << endl;
}