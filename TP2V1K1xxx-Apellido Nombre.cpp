// TP2V1K1xxx-Apellido Nombre.cpp
// Programa  : ArticulosxRubros
// Fecha Ent.: 09-11-2026                                    Version: 1
// Objetivo  : Listar los Articulos agrupados por Rubro. Se arma una lista
//             ordenada de Rubros (por codRub) y, colgando de cada rubro, una
//             sublista de Articulos ordenada por descripcion. Cada nodo de
//             articulo guarda la posicion (referencia) del registro en
//             Articulos.Dat, para recuperar luego codigo y precio al azar.
// Curso     : K1xxx            Dia: xxxxxxxx            Turno: xxxxxxxx
// Grupo Nro.: x
// Integrantes (Apellido, Nombre):
//             1) xxxxxxxx, xxxxxxxx
//             2) xxxxxxxx, xxxxxxxx
// Compilador: Borland C++ V.5.5
// HAC

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdio>
#include <ctime>
using namespace std;

const short MAX_ARTICULOS = 30000;
const short ANCHO_LINEA   = 65;

typedef char str5[6], str7[8], str30[31];

struct tInfoRub;
struct tInfoArt;

struct sRub {
  str5  codRub;
  str30 descrip;
};

struct sArt {
  str7  codArt;
  str30 descrip;
  short stkAct;
  str5  codRub;
  float preUni;
};

typedef struct sNodoRub *tListaRub;
typedef struct sNodoArt *tSubListaArt;

struct tInfoRub {
  str5         codRub;
  tSubListaArt subListaArt;
};

struct tInfoArt {
  str30 descrip;
  short refArt;      // Nro. de registro del articulo en Articulos.Dat
};

struct sNodoRub {
  tInfoRub  info;
  tListaRub sig;
};

struct sNodoArt {
  tInfoArt     info;
  tSubListaArt sig;
};

// ---------------------------------------------------------------- Sublistas
void InsertaInicio(tSubListaArt &Lista, tInfoArt valor) {
  tSubListaArt nuevo = new sNodoArt;
  nuevo->info = valor;
  nuevo->sig  = Lista;
  Lista = nuevo;
} // InsertaInicio

void InsertaEnLugar(tSubListaArt &pNodo, tInfoArt valor) {
  // Inserta un nodo nuevo a continuacion de pNodo.
  tSubListaArt nuevo = new sNodoArt;
  nuevo->info  = valor;
  nuevo->sig   = pNodo->sig;
  pNodo->sig   = nuevo;
} // InsertaEnLugar

void InsertaEnMedio(tSubListaArt &Lista, tInfoArt valor) {
  // Precondicion: Lista no vacia y valor no precede al primer nodo.
  tSubListaArt ant = Lista;
  while (ant->sig != NULL && strcmp(ant->sig->info.descrip, valor.descrip) <= 0)
    ant = ant->sig;
  InsertaEnLugar(ant, valor);
} // InsertaEnMedio

void InsertaNodo(tSubListaArt &Lista, tInfoArt valor) {
  if (Lista == NULL || strcmp(valor.descrip, Lista->info.descrip) < 0)
    InsertaInicio(Lista, valor);
  else
    InsertaEnMedio(Lista, valor);
} // InsertaNodo

void SacarPrimerNodoArt(tSubListaArt &Lista, tInfoArt &valor) {
  tSubListaArt p = Lista;
  valor = p->info;
  Lista = p->sig;
  delete p;
} // SacarPrimerNodoArt

// -------------------------------------------------------------- Lista Rubros
void AgregarUltimoNodo(tListaRub &Lista, tListaRub &ListaFin, tInfoRub valor) {
  tListaRub nuevo = new sNodoRub;
  nuevo->info = valor;
  nuevo->sig  = NULL;
  if (Lista == NULL)
    Lista = nuevo;
  else
    ListaFin->sig = nuevo;
  ListaFin = nuevo;
} // AgregarUltimoNodo

tListaRub iif(bool exprLog, tListaRub ptr1, tListaRub ptr2) {
  if (exprLog)
    return ptr1;
  else
    return ptr2;
} // iif

tListaRub BuscarNodoClvOrd(tListaRub &Lista, str5 codRub) {
  // Busqueda en lista ordenada: corta al llegar a una clave mayor o igual.
  tListaRub p = Lista;
  while (p != NULL && strcmp(p->info.codRub, codRub) < 0)
    p = p->sig;
  return iif(p != NULL && strcmp(p->info.codRub, codRub) == 0, p, NULL);
} // BuscarNodoClvOrd

void SacarPrimerNodoRub(tListaRub &Lista, tInfoRub &valor) {
  tListaRub p = Lista;
  valor = p->info;
  Lista = p->sig;
  delete p;
} // SacarPrimerNodoRub

// ---------------------------------------------------------------- Utilitarios
string replicate(char car, short n) {
  return string(n, car);
} // replicate

long GetDate(int &year, int &mes, int &dia, int &ds) {
  time_t t = time(NULL);
  tm *f = localtime(&t);
  year = f->tm_year + 1900;
  mes  = f->tm_mon + 1;
  dia  = f->tm_mday;
  ds   = f->tm_wday;
  return year * 10000L + mes * 100 + dia;
} // GetDate

// ------------------------------------------------------------------ Procesos
void Abrir(ifstream &Rubros, ifstream &Articulos) {
  Rubros.open("Rubros.Dat", ios::in | ios::binary);
  Articulos.open("Articulos.Dat", ios::in | ios::binary);
} // Abrir

void ArmarListaRubros(ifstream &Rubros, tListaRub &ListaRubros) {
  // 1er. acceso secuencial a Rubros. Como el archivo ya esta ordenado por
  // codRub, cada nodo se agrega al final de la lista.
  tListaRub ListaFin = NULL;
  tInfoRub  infoRub;
  sRub      rRub;

  ListaRubros = NULL;
  while (Rubros.read((char *) &rRub, sizeof rRub)) {
    strcpy(infoRub.codRub, rRub.codRub);
    infoRub.subListaArt = NULL;
    AgregarUltimoNodo(ListaRubros, ListaFin, infoRub);
  }
} // ArmarListaRubros

void ArmarSubListaArticulos(ifstream &Articulos, tListaRub &ListaRubros) {
  // 1er. acceso (secuencial) a Articulos. Un solo ciclo: por cada articulo
  // se ubica su rubro y se lo inserta ordenado por descripcion en su sublista.
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
} // ArmarSubListaArticulos

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

  // 2do. acceso secuencial a Rubros (solo para obtener el nombre del rubro).
  // La lista tiene los mismos rubros y en el mismo orden que el archivo.
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
      // 2do. acceso a Articulos: al azar, por la referencia guardada.
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
} // ListadoRubrosArticulos

void Cerrar(ifstream &Rubros, ifstream &Articulos) {
  Rubros.close();
  Articulos.close();
} // Cerrar

int main() {
  ifstream  Rubros, Articulos;
  tListaRub ListaRubros;

  Abrir(Rubros, Articulos);
  ArmarListaRubros(Rubros, ListaRubros);
  ArmarSubListaArticulos(Articulos, ListaRubros);
  ListadoRubrosArticulos(Rubros, Articulos, ListaRubros);
  Cerrar(Rubros, Articulos);
  return 0;
} // main
