#include <iostream>
#include "Arbin.h"
#include "Lista.h"

using namespace std;

enum Direccion { DIRECCION_DERECHA , DIRECCION_IZQUIERDA };

int main()
{
	Arbin<int> arbin;
	int goal;

	// Parseando el arbol...

	checksum(arbin,0,goal);
}

bool checksum(const Arbin<int> &tree, int sum,const int goal)
{
	if(tree.raiz() + sum == goal)
		return true;
	else
	{
		if(!tree.hijoIz().esVacio() && !tree.hijoDr().esVacio())
			return checksum(tree.hijoIz(),sum + tree.raiz(),goal) || checksum(tree.hijoDr(),sum + tree.raiz(),goal);
		else if(!tree.hijoIz().esVacio())
			return checksum(tree.hijoIz(),sum + tree.raiz(),goal);
		else
			return checksum(tree.hijoDr(),sum + tree.raiz(),goal);
	}
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