#include <iostream>
#include "Arbin.h"
#include "Lista.h"

using namespace std;

enum Direccion { DIRECCION_DERECHA , DIRECCION_IZQUIERDA };

int main()
{
	Arbin<int> arbin;
	Lista<int> list;
	int goal;

	// Parseando el arbol...

	autumn(arbin,list,list.principio(),DIRECCION_IQUIERDA);
}


void autumn(const Arbin<int> &tree, Lista<int> &list,Lista<int>::Iterador,Direccion direcion)
{
	if(list.esVacia())
		list.insertar(tree.raiz(),list.principio());
	else
	{
		if(direccion == DIRECCION_IZQUIERDA)

	}

}