// CnvTxt2BinArticulos.cpp
// HAC

#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

typedef char str5 [ 6],
             str7 [ 8],
             str30[31];

struct sArt {
  str7  codArt;
  str30 descrip;
  short stkAct;
  str5  codRub;
  float preUni;
};

// Formato de cada linea de Articulos.Txt:
// x(7) x(30) 9(4) x(5) 9(7).99   (columnas separadas por un espacio)
bool LeerArticulos(ifstream &Art, sArt &rArt) {
  Art.get(rArt.codArt, 8);
  Art.ignore();
  Art.get(rArt.descrip, 31);
  Art.ignore();
  Art >> rArt.stkAct;
  Art.ignore();
  Art.get(rArt.codRub, 6);
  Art.ignore();
  Art >> rArt.preUni;
  Art.ignore(80, '\n');
  return !Art.fail();
} // LeerArticulos

int main() {
  ifstream ArticulosTxt("Articulos.Txt");
  fstream  Articulos("Articulos.Dat",
                     ios::binary | ios::in | ios::out | ios::trunc);
  sArt rArt;

  while (LeerArticulos(ArticulosTxt, rArt))
    Articulos.write((char *) &rArt, sizeof rArt);
  // Comprobar si se guardo correctamente.
  Articulos.clear();
  Articulos.seekg(0);
  while (Articulos.read((char *) &rArt, sizeof rArt))
    cout << left << setw(7) << rArt.codArt << ' '
         << setw(30) << rArt.descrip << ' '
         << right << setw(4) << rArt.stkAct << ' '
         << rArt.codRub << ' '
         << fixed << setprecision(2) << setw(10) << rArt.preUni << endl;
  ArticulosTxt.close();
  Articulos.close();
  return 0;
}
