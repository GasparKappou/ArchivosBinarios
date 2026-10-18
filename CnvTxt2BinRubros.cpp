// CnvTxt2BinRubros.cpp
// HAC

#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

typedef char str5 [ 6],
             str30[31];

struct sRub {
  str5  codRub;
  str30 descrip;
};

// Formato de cada linea de Rubros.Txt: x(5) x(30)
bool LeerRubros(ifstream &Rub, sRub &rRub) {
  Rub.get(rRub.codRub, 6);
  Rub.ignore();
  Rub.get(rRub.descrip, 31);
  bool ok = !Rub.fail();   // antes del ignore: la ultima linea puede no tener '\n'
  Rub.ignore(80, '\n');
  return ok;
} // LeerRubros

int main() {
  ifstream RubrosTxt("Rubros.Txt");
  fstream  Rubros("Rubros.Dat",
                  ios::binary | ios::in | ios::out | ios::trunc);
  sRub rRub;

  while (LeerRubros(RubrosTxt, rRub))
    Rubros.write((char *) &rRub, sizeof rRub);
  // Comprobar si se guardo correctamente.
  Rubros.clear();
  Rubros.seekg(0);
  while (Rubros.read((char *) &rRub, sizeof rRub))
    cout << right << setw(5) << rRub.codRub << ' '
         << left << setw(30) << rRub.descrip << endl;
  RubrosTxt.close();
  Rubros.close();
  return 0;
}
