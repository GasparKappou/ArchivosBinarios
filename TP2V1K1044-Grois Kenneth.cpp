  /*=============================================================================
    TRABAJO PRÁCTICO N° 2
    ALGORITMOS Y ESTRUCTURAS DE DATOS
    UTN - FRBA
    CURSO: K1044

    INTEGRANTES:
      - Kenneth Grois               (Legajo: 260.205-2)
      - Gaspar Kappou               (Legajo: 260.238-6)

=============================================================================*/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
using namespace std;
 
#define main CnvRubros_main
#include "CnvTxt2BinRubros.cpp"
#undef main
#define main CnvArticulos_main
#include "CnvTxt2BinArticulos.cpp"
#undef main
 
const short MAX_ARTICULOS = 30000;
const short ANCHO_LINEA   = 65;
 
typedef char str5[6], str7[8], str30[31];
 
struct tInfoRub;
struct tInfoArt;
 
typedef struct sNodoRub *tListaRub;
typedef struct sNodoArt *tSubListaArt;
 
struct tInfoRub {
  str5         codRub;
  tSubListaArt subListaArt;
};
 
struct tInfoArt {
  str30 descrip;
  short refArt;
};
 
struct sNodoRub {
  tInfoRub  info;
  tListaRub sig;
};
 
struct sNodoArt {
  tInfoArt     info;
  tSubListaArt sig;
};
 
void InsertaInicio(tSubListaArt &Lista, tInfoArt valor) {
  tSubListaArt nuevo = new sNodoArt;
  nuevo->info = valor;
  nuevo->sig  = Lista;
  Lista = nuevo;
}
 
void InsertaEnLugar(tSubListaArt &pNodo, tInfoArt valor) {
 
  tSubListaArt nuevo = new sNodoArt;
  nuevo->info  = valor;
  nuevo->sig   = pNodo->sig;
  pNodo->sig   = nuevo;
}
 
void InsertaEnMedio(tSubListaArt &Lista, tInfoArt valor) {
 
  tSubListaArt ant = Lista;
  while (ant->sig != NULL && strcmp(ant->sig->info.descrip, valor.descrip) <= 0)
    ant = ant->sig;
  InsertaEnLugar(ant, valor);
}
 
void InsertaNodo(tSubListaArt &Lista, tInfoArt valor) {
  if (Lista == NULL || strcmp(valor.descrip, Lista->info.descrip) < 0)
    InsertaInicio(Lista, valor);
  else
    InsertaEnMedio(Lista, valor);
}
 
void SacarPrimerNodoArt(tSubListaArt &Lista, tInfoArt &valor) {
  tSubListaArt p = Lista;
  valor = p->info;
  Lista = p->sig;
  delete p;
}
 
void AgregarUltimoNodo(tListaRub &Lista, tListaRub &ListaFin, tInfoRub valor) {
  tListaRub nuevo = new sNodoRub;
  nuevo->info = valor;
  nuevo->sig  = NULL;
  if (Lista == NULL)
    Lista = nuevo;
  else
    ListaFin->sig = nuevo;
  ListaFin = nuevo;
}
 
tListaRub iif(bool exprLog, tListaRub ptr1, tListaRub ptr2) {
  if (exprLog)
    return ptr1;
  else
    return ptr2;
}
 
tListaRub BuscarNodoClvOrd(tListaRub &Lista, str5 codRub) {
 
  tListaRub p = Lista;
  while (p != NULL && strcmp(p->info.codRub, codRub) < 0)
    p = p->sig;
  return iif(p != NULL && strcmp(p->info.codRub, codRub) == 0, p, NULL);
}
 
void SacarPrimerNodoRub(tListaRub &Lista, tInfoRub &valor) {
  tListaRub p = Lista;
  valor = p->info;
  Lista = p->sig;
  delete p;
}
 
string replicate(char car, short n) {
  return string(n, car);
}
 
long GetDate(int &year, int &mes, int &dia, int &ds) {
  time_t t = time(NULL);
  tm *f = localtime(&t);
  year = f->tm_year + 1900;
  mes  = f->tm_mon + 1;
  dia  = f->tm_mday;
  ds   = f->tm_wday;
  return year * 10000L + mes * 100 + dia;
}
 
void ConvertirTxt2Bin() {
#if defined(_WIN32) || defined(__WIN32__)
  freopen("nul", "w", stdout);
#else
  freopen("/dev/null", "w", stdout);
#endif
  CnvRubros_main();
  CnvArticulos_main();
}
 
void Abrir(ifstream &Rubros, ifstream &Articulos) {
  Rubros.open("Rubros.Dat", ios::in | ios::binary);
  Articulos.open("Articulos.Dat", ios::in | ios::binary);
}
 
void ArmarListaRubros(ifstream &Rubros, tListaRub &ListaRubros) {
 
  tListaRub ListaFin = NULL;
  tInfoRub  infoRub;
  sRub      rRub;
 
  ListaRubros = NULL;
  while (Rubros.read((char *) &rRub, sizeof rRub)) {
    strcpy(infoRub.codRub, rRub.codRub);
    infoRub.subListaArt = NULL;
    AgregarUltimoNodo(ListaRubros, ListaFin, infoRub);
  }
}
 
void ArmarSubListaArticulos(ifstream &Articulos, tListaRub &ListaRubros) {
 
  sArt      rArt;
  tInfoArt  infoArt;
  tListaRub pRub;
  short     nroReg = 0;
 
  while (nroReg < MAX_ARTICULOS && Articulos.read((char *) &rArt, sizeof rArt)) {
    pRub = BuscarNodoClvOrd(ListaRubros, rArt.codRub);
    if (pRub != NULL) {
      strcpy(infoArt.descrip, rArt.descrip);
      infoArt.refArt = nroReg;
      InsertaNodo(pRub->info.subListaArt, infoArt);
    }
    nroReg++;
  }
}
 
void ListadoRubrosArticulos(ifstream &Rubros, ifstream &Articulos,
                            tListaRub &ListaRubros) {
  sRub     rRub;
  sArt     rArt;
  tInfoRub infoRub;
  tInfoArt infoArt;
  short    cantItems, maxItems = 0;
  str5     codRubMax = "";
  int      anio, mes, dia, ds;
  string   linea = replicate('-', ANCHO_LINEA);
 
  freopen("ListadoRubrosArticulos.Lst", "w", stdout);
  GetDate(anio, mes, dia, ds);
  cout << "Fecha: " << setfill('0') << setw(2) << dia << '-' << setw(2) << mes
       << '-' << setw(4) << anio << setfill(' ') << endl << endl
       << replicate(' ', 17) << "Listado por Rubros y Artículos" << endl
       << linea << endl;
 
  Rubros.clear();
  Rubros.seekg(0);
  Articulos.clear();
  while (ListaRubros != NULL) {
    SacarPrimerNodoRub(ListaRubros, infoRub);
    Rubros.read((char *) &rRub, sizeof rRub);
    cout << "Cód.Rubro: " << infoRub.codRub << "     Nombre Rubro: "
         << rRub.descrip << endl << endl
         << "   Cód.Artículo   Descripción" << replicate(' ', 25)
         << "Pre.Unit." << endl;
    cantItems = 0;
    while (infoRub.subListaArt != NULL) {
      SacarPrimerNodoArt(infoRub.subListaArt, infoArt);
 
      Articulos.seekg((long) infoArt.refArt * sizeof rArt);
      Articulos.read((char *) &rArt, sizeof rArt);
      cout << "   " << left << setw(15) << rArt.codArt
           << setw(33) << infoArt.descrip << "$ " << right << setw(10)
           << fixed << setprecision(2) << rArt.preUni << endl;
      cantItems++;
    }
    cout << endl << "   Cantidad de items: " << right << setw(4) << cantItems
         << endl << linea << endl;
    if (cantItems > maxItems) {
      maxItems = cantItems;
      strcpy(codRubMax, infoRub.codRub);
    }
  }
  cout << endl << "Código de Rubro con mayor cantidad de items: " << codRubMax
       << endl;
}
 
void Cerrar(ifstream &Rubros, ifstream &Articulos) {
  Rubros.close();
  Articulos.close();
}
 
void AbrirListado() {
  fflush(stdout);
#if defined(_WIN32) || defined(__WIN32__)
  system("start notepad ListadoRubrosArticulos.Lst");
#elif defined(__APPLE__)
  system("open ListadoRubrosArticulos.Lst");
#else
  system("xdg-open ListadoRubrosArticulos.Lst");
#endif
}
 
int main() {
  ifstream  Rubros, Articulos;
  tListaRub ListaRubros;
 
  ConvertirTxt2Bin();
  Abrir(Rubros, Articulos);
  ArmarListaRubros(Rubros, ListaRubros);
  ArmarSubListaArticulos(Articulos, ListaRubros);
  ListadoRubrosArticulos(Rubros, Articulos, ListaRubros);
  Cerrar(Rubros, Articulos);
  AbrirListado();
  return 0;
}