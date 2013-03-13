#include <iostream>
using namespace std;

/*
  Definici�n de algunas excepciones de las distintas
  implementaciones de los TADs.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/
#ifndef __ARBOLES_H
#define __ARBOLES_H

#include <string>
#include <iosfwd>
#include <cassert>
/**
 Clase de la que heredan todas las excepciones, y
 que proporciona el atributo que almacena el
 mensaje de error.
 */
class ExcepcionTAD {
public:
	ExcepcionTAD() {}
	ExcepcionTAD(const std::string &msg) : _msg(msg) {}

	const std::string msg() const { return _msg; }

	friend std::ostream &operator<<(std::ostream &out, const ExcepcionTAD &e);

protected:
	std::string _msg;
};

inline std::ostream &operator<<(std::ostream &out, const ExcepcionTAD &e) {
	out << e._msg;
	return out;
}


// Macro para declarar las clases de tipo excepci�n
// que heredan de ExcepcionConMensaje, para ahorrar
// escribir muchas veces lo mismo...
#define DECLARA_EXCEPCION(Excepcion) \
class Excepcion : public ExcepcionTAD { \
public: \
Excepcion() {}; \
Excepcion(const std::string &msg) : ExcepcionTAD(msg) {} \
};

/**
 Excepci�n generada por algunas operaciones de las pilas.
 */
DECLARA_EXCEPCION(EPilaVacia);

/**
 Excepci�n generada por algunas de las operaciones de las colas.
 */
DECLARA_EXCEPCION(EColaVacia);

/**
 Excepci�n generada por algunas operaciones de las colas dobles.
 */
DECLARA_EXCEPCION(EDColaVacia);

/**
 Excepci�n generada por algunas operaciones de las listas.
 */
DECLARA_EXCEPCION(EListaVacia);

/**
 Excepci�n generada por accesos incorrectos a las listas
 (tanto a un n�mero de elemento incorrecto como por
 mal manejo de los iteradores).
 */
DECLARA_EXCEPCION(EAccesoInvalido);

/**
 Excepci�n generada por algunas operaciones de los
 �rboles binarios.
 */
DECLARA_EXCEPCION(EArbolVacio);

/**
 Excepci�n generada por algunas operaciones de los
 �rboles de b�squeda.
 */
DECLARA_EXCEPCION(EClaveErronea);


/**
  @file Lista.h

  Implementaci�n del TAD lista, utilizando una 
  lista doblemente enlazada.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/


/**
 Implementaci�n del TAD Pila utilizando vectores din�micos.

 Las operaciones son:

 - ListaVacia: -> Lista. Generadora implementada en el
   constructor sin par�metros.
 - Cons: Lista, Elem -> Lista. Generadora.
 - ponDr: Lista, Elem -> Lista. Modificadora.
 - primero: Lista - -> Elem. Observadora parcial
 - resto: Lista - -> Lista. Modificadora parcial
 - ultimo: Lista - -> Elem. Observadora parcial
 - inicio: Lista - -> Lista. Modificadora parcial
 - esVacia: Lista -> Bool. Observadora
 - numElems: Lista -> Elem. Obervadora.
 - elem: Lista, Entero - -> Elem. Observador parcial.

 @author Marco Antonio G�mez Mart�n
 */
template <class T>
class Lista {
private:
	/**
	 Clase nodo que almacena internamente el elemento (de tipo T),
	 y dos punteros, uno al nodo anterior y otro al nodo siguiente.
	 Ambos punteros podr�an ser NULL si el nodo es el primero
	 y/o �ltimo de la lista enlazada.
	 */
	class Nodo {
	public:
		Nodo() : _sig(NULL), _ant(NULL) {}
		Nodo(const T &elem) : _elem(elem), _sig(NULL), _ant(NULL) {}
		Nodo(Nodo *ant, const T &elem, Nodo *sig) : 
		    _elem(elem), _sig(sig), _ant(ant) {}

		T _elem;
		Nodo *_sig;
		Nodo *_ant;
	};

public:

	/** Constructor; operaci�n ListaVacia. */
	Lista() : _prim(NULL), _ult(NULL), _numElems(0) {}

	/** Destructor; elimina la lista doblemente enlazada. */
	~Lista() {
		libera();
	}

	/**
	 A�ade un nuevo elemento en la cabeza de la lista.
	 Operaci�n generadora.

	 @param elem Elemento que se a�ade en la cabecera de
	 la lista.
	*/
	void Cons(const T &elem) {
		_numElems++;
		_prim = insertaElem(elem, NULL, _prim);
		if (_ult == NULL)
			_ult = _prim;
	}

	/**
	 A�ade un nuevo elemento al final de la lista (a la 
	 "derecha"). Operaci�n modificadora.

	 ponDr(e, ListaVacia) = Cons(e, ListaVacia)
	 ponDr(e, Cons(x, xs)) = Cons(x, ponDr(e, xs))
	*/
	void ponDr(const T &elem) {
		_numElems++;
		_ult = insertaElem(elem, _ult, NULL);
		if (_prim == NULL)
			_prim = _ult;
	}

	/**
	 Devuelve el valor almacenado en la cabecera de la
	 lista. Es un error preguntar por el primero de
	 una lista vac�a.

	 primero(Cons(x, xs)) = x
	 error primero(ListaVacia)

	 @return Elemento en la cabecera de la lista.
	 */
	const T &primero() const {
		if (esVacia())
			throw EListaVacia();
		return _prim->_elem;
	}

	/**
	 Devuelve el valor almacenado en la �ltima posici�n
	 de la lista (a la derecha).
	 Es un error preguntar por el primero de una lista vac�a.

	 ultimo(Cons(x, xs)) = x           SI esVacia(xs)
	 ultimo(Cons(x, xs)) = ultimo(xs)  SI !esVacia(xs)
	 error ultimo(ListaVacia)

	 @return Elemento en la cola de la lista.
	 */
	const T &ultimo() const {
		if (esVacia())
			throw EListaVacia();

		return _ult->_elem;
	}

	/**
	 Elimina el primer elemento de la lista.
	 Es un error intentar obtener el resto de una lista vac�a.

	 resto(Cons(x, xs)) = xs
	 error resto(ListaVacia)
	*/
	void resto() {
		if (esVacia())
			throw EListaVacia();

		Nodo *aBorrar = _prim;
		_prim = _prim->_sig;
		borraElem(aBorrar);
		if (_prim == NULL)
			_ult = NULL;
		--_numElems;
	}

	/**
	 Elimina el �ltimo elemento de la lista.
	 Es un error intentar obtener el inicio de una lista vac�a.

	 inicio(Cons(x, ListaVacia)) = ListaVacia
	 inicio(Cons(x, xs)) = Cons(x, inicio(xs)) SI !esVacia(xs)
	 error inicio(ListaVacia)
	*/
	void inicio() {
		if (esVacia())
			throw EListaVacia();

		Nodo *aBorrar = _ult;
		_ult = _ult->_ant;
		borraElem(aBorrar);
		if (_ult == NULL)
			_prim = NULL;
		--_numElems;
	}

	/**
	 Operaci�n observadora para saber si una lista
	 tiene o no elementos.

	 esVacia(ListaVacia) = true
	 esVacia(Cons(x, xs)) = false

	 @return true si la lista no tiene elementos.
	 */
	bool esVacia() const {
		return _prim == NULL;
	}

	/**
	 Devuelve el n�mero de elementos que hay en la
	 lista.
	 numElems(ListaVacia) = 0
	 numElems(Cons(x, xs)) = 1 + numElems(xs)

	 @return N�mero de elementos.
	 */
	unsigned int numElems() const {
		return _numElems;
	}

	/**
	 Devuelve el elemento i-�simo de la lista, teniendo
	 en cuenta que el primer elemento (primero())
	 es el elemento 0 y el �ltimo es numElems()-1,
	 es decir idx est� en [0..numElems()-1].
	 Operaci�n observadora parcial que puede fallar
	 si se da un �ndice incorrecto. El �ndice es
	 entero sin signo, para evitar que se puedan
	 pedir elementos negativos.

	 elem(0, Cons(x, xs)) = x
	 elem(n, Cons(x, xs)) = elem(n-1, xs) si n > 0
	 error elem(n, xs) si !( 0 <= n < numElems(xs) )
	*/
	const T &elem(unsigned int idx) const {
		if (idx >= _numElems)
			throw EAccesoInvalido();

		Nodo *aux = _prim;
		for (int i = 0; i < idx; ++i)
			aux = aux->_sig;

		return aux->_elem;
	}

	/**
	 Clase interna que implementa un iterador sobre
	 la lista que permite recorrer la lista e incluso
	 alterar el valor de sus elementos.
	 */
	class Iterador {
	public:
		void avanza() {
			if (_act == NULL) throw EAccesoInvalido();
			_act = _act->_sig;
		}

		const T &elem() const {
			if (_act == NULL) throw EAccesoInvalido();
			return _act->_elem;
		}

		void pon(const T &elem) {
			if (_act == NULL) throw EAccesoInvalido();
			_act->_elem = elem;
		}

		bool operator==(const Iterador &other) const {
			return _act == other._act;
		}

		bool operator!=(const Iterador &other) const {
			return !(this->operator==(other));
		}
	protected:
		// Para que pueda construir objetos del
		// tipo iterador
		friend class Lista;

		Iterador() : _act(NULL) {}
		Iterador(Nodo *act) : _act(act) {}

		// Puntero al nodo actual del recorrido
		Nodo *_act;
	};
	
	/**
	 Devuelve el iterador al principio de la lista.
	 @return iterador al principio de la lista;
	 coincidir� con final() si la lista est� vac�a.
	 */
	Iterador principio() {
		return Iterador(_prim);
	}

	/**
	 @return Devuelve un iterador al final del recorrido
	 (fuera de �ste).
	 */
	Iterador final() const {
		return Iterador(NULL);
	}

	/**
	 Permite eliminar de la lista el elemento
	 apuntado por el iterador que se pasa como par�metro.
	 El iterador recibido DEJA DE SER V�LIDO. En su
	 lugar, deber� utilizarse el iterador devuelto, que
	 apuntar� al siguiente elemento al borrado.
	 @param it Iterador colocado en el elemento que se
	 quiere borrar.
	 @return Nuevo iterador colocado en el elemento siguiente
	 al borrado (podr�a coincidir con final() si el
	 elemento que se borr� era el �ltimo de la lista).
	 */
	Iterador borra(const Iterador &it) {
		if (it._act == NULL)
			throw EAccesoInvalido();

		// Cubrimos los casos especiales donde
		// borramos alguno de los extremos
		if (it._act == _prim) {
			resto();
			return Iterador(_prim);
		} else if (it._act == _ult) {
			inicio();
			return Iterador(NULL);
		} else {
			// El elemento a borrar es interno a la lista.
			--_numElems;
			Nodo *sig = it._act->_sig;
			borraElem(it._act);
			return Iterador(sig);
		}
	}

	/**
	 M�todo para insertar un elemento en la lista
	 en el punto marcado por el iterador. En concreto,
	 se a�ade _justo antes_ que el elemento actual. Es
	 decir, si it==l.primero(), el elemento insertado se
	 convierte en el primer elemento (y el iterador
	 apuntar� al segundo). Si it==l.final(), el elemento
	 insertado ser� el �ltimo (e it seguir� apuntando
	 fuera del recorrido).
	 @param elem Valor del elemento a insertar.
	 @param it Punto en el que insertar el elemento.
	 */
	void insertar(const T &elem, const Iterador &it) {

		// Caso especial: �a�adir al principio?
		if (_prim == it._act) {
			Cons(elem);
		} else
		// Caso especial: �a�adir al final?
		if (it._act == NULL) {
			ponDr(elem);
		}
		// Caso normal
		else {
			insertaElem(elem, it._act->_ant, it._act);
		}
	}

	// //
	// M�TODOS DE "FONTANER�A" DE C++ QUE HACEN VERS�TIL
	// A LA CLASE
	// //

	/** Constructor copia */
	Lista(const Lista<T> &other) : _prim(NULL), _ult(NULL) {
		copia(other);
	}

	/** Operador de asignaci�n */
	Lista<T> &operator=(const Lista<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

	/** Operador de comparaci�n. */
	bool operator==(const Lista<T> &rhs) const {
		if (_numElems != rhs._numElems)
			return false;
		Nodo *p1 = _prim;
		Nodo *p2 = rhs._prim;
		while ((p1 != NULL) && (p2 != NULL)) {
			if (p1->_elem != p2->_elem)
				return false;
			p1 = p1->_sig;
			p2 = p2->_sig;
		}

		return (p1 == NULL) && (p2 == NULL);
	}

	bool operator!=(const Lista<T> &rhs) const {
		return !(*this == rhs);
	}


protected:

	void libera() {
		libera(_prim);
		_prim = NULL;
		_ult = NULL;
	}

	void copia(const Lista<T> &other) {
		// En vez de trabajar con punteros en la inserci�n,
		// usamos ponDr
		_prim = 0;
		_numElems = 0;

		Nodo *act = other._prim;
		while (act != NULL) {
			ponDr(act->_elem);
			act = act->_sig;
		}
	}

private:


	/**
	 Inserta un elemento entre el nodo1 y el nodo2.
	 Devuelve el puntero al nodo creado.
	 Caso general: los dos nodos existen.
	    nodo1->_sig == nodo2
	    nodo2->_ant == nodo1
	 Casos especiales: alguno de los nodos no existe
	    nodo1 == NULL y/o nodo2 == NULL
	*/
	static Nodo *insertaElem(const T &e, Nodo *nodo1, Nodo *nodo2) {
		Nodo *nuevo = new Nodo(nodo1, e, nodo2);
		if (nodo1 != NULL)
			nodo1->_sig = nuevo;
		if (nodo2 != NULL)
			nodo2->_ant = nuevo;
		return nuevo;
	}

	/**
	 Elimina el nodo n. Si el nodo tiene nodos antes
	 o despu�s, actualiza sus punteros anterior y siguiente.
	 Caso general: hay nodos anterior y siguiente.
	 Casos especiales: algunos de los nodos (anterior o siguiente
	 a n) no existen.
	*/
	static void borraElem(Nodo *n) {
		assert(n != NULL);
		Nodo *ant = n->_ant;
		Nodo *sig = n->_sig;
		if (ant != NULL)
			ant->_sig = sig;
		if (sig != NULL)
			sig->_ant = ant;
		delete n;
	}

	/**
	 Elimina todos los nodos de la lista enlazada cuyo
	 primer nodo se pasa como par�metro.
	 Se admite que el nodo sea NULL (no habr� nada que
	 liberar). En caso de pasarse un nodo v�lido,
	 su puntero al nodo anterior debe ser NULL (si no,
	 no ser�a el primero de la lista!).
	 */
	static void libera(Nodo *prim) {
		assert(!prim || !prim->_ant);

		while (prim != NULL) {
			Nodo *aux = prim;
			prim = prim->_sig;
			delete aux;
		}
	}

	// Puntero al primer y �ltimo elemento
	Nodo *_prim, *_ult;

	// N�mero de elementos (n�mero de nodos entre _prim y _ult)
	unsigned int _numElems;
};

/**
  @file Cola.h

  Implementaci�n del TAD Cola utilizando una
  lista enlazada de nodos.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/


/**
 Implementaci�n del TAD Cola utilizando una lista enlazada.

 Las operaciones son:

 - ColaVacia: -> Cola. Generadora implementada en el
   constructor sin par�metros.
 - PonDetras: Cola, Elem -> Cola. Generadora
 - quitaPrim: Cola - -> Cola. Modificadora parcial.
 - primero: Cola - -> Elem. Observadora parcial.
 - esVacia: Cola -> Bool. Observadora.
 - numElems: Cola -> Entero. Observadora.

 @author Marco Antonio G�mez Mart�n
 */
template <class T>
class Cola {
public:

	/** Constructor; operacion ColaVacia */
	Cola() : _prim(NULL), _ult(NULL), _numElems(0) {
	}

	/** Destructor; elimina la lista enlazada. */
	~Cola() {
		libera();
		_prim = _ult = NULL;
	}

	/**
	 A�ade un elemento en la parte trasera de la cola.
	 Operaci�n generadora.

	 @param elem Elemento a a�adir.
	*/
	void ponDetras(const T &elem) {
		Nodo *nuevo = new Nodo(elem, NULL);

		if (_ult != NULL)
			_ult->_sig = nuevo;
		_ult = nuevo;
		// Si la cola estaba vac�a, el primer elemento
		// es el que acabamos de a�adir
		if (_prim == NULL)
			_prim = nuevo;
		_numElems++;
	}

	/**
	 Elimina el primer elemento de la cola.
	 Operaci�n modificadora parcial, que falla si 
	 la cola est� vac�a.

	 quitaPrim(PonDetras(elem, ColaVacia)) = ColaVacia
	 quitaPrim(PonDetras(elem, xs)) = PonDetras(elem, quitaPrim(xs)) si !esVacia(xs)
	 error: quitaPrim(ColaVacia)
	*/
	void quitaPrim() {
		if (esVacia())
			throw EColaVacia();
		Nodo *aBorrar = _prim;
		_prim = _prim->_sig;
		delete aBorrar;
		--_numElems;
		// Si la cola se qued� vac�a, no hay
		// �ltimo
		if (_prim == NULL)
			_ult = NULL;
	}

	/**
	 Devuelve el primer elemento de la cola. Operaci�n
	 observadora parcial, que falla si la cola est� vac�a.

	 primero(PonDetras(elem, ColaVacia)) = elem
	 primero(PonDetras(elem, xs)) = primero(xs) si !esVacia(xs)
	 error: primero(ColaVacia)

	 @return El primer elemento de la cola.
	 */
	const T &primero() const {
		if (esVacia())
			throw EColaVacia();
		return _prim->_elem;
	}

	/**
	 Devuelve true si la cola no tiene ning�n elemento.

	 esVacia(Cola) = true
	 esVacia(PonDetras(elem, p)) = false

	 @return true si la cola no tiene ning�n elemento.
	 */
	bool esVacia() const {
		return _prim == NULL;
	}

	/**
	 Devuelve el n�mero de elementos que hay en la
	 cola.
	 numElems(ColaVacia) = 0
	 numElems(PonDetras(elem, p)) = 1 + numElems(p)

	 @return N�mero de elementos.
	 */
	int numElems() const {
		return _numElems;
	}

	// //
	// M�TODOS DE "FONTANER�A" DE C++ QUE HACEN VERS�TIL
	// A LA CLASE
	// //

	/** Constructor copia */
	Cola(const Cola<T> &other) : _prim(NULL), _ult(NULL) {
		copia(other);
	}

	/** Operador de asignaci�n */
	Cola<T> &operator=(const Cola<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

	/** Operador de comparaci�n. */
	bool operator==(const Cola<T> &rhs) const {
		if (_numElems != rhs._numElems)
			return false;
		Nodo *p1 = _prim;
		Nodo *p2 = rhs._prim;
		while ((p1 != NULL) && (p2 != NULL)) {
			if (p1->_elem != p2->_elem)
				return false;
			p1 = p1->_sig;
			p2 = p2->_sig;
		}

		return (p1 == NULL) && (p2 == NULL);
	}

	bool operator!=(const Cola<T> &rhs) const {
		return !(*this == rhs);
	}

protected:

	void libera() {
		libera(_prim);
	}

	void copia(const Cola &other) {

		if (other.esVacia()) {
			_prim = _ult = NULL;
			_numElems = 0;
		} else {
			Nodo *act = other._prim;
			Nodo *ant;
			_prim = new Nodo(act->_elem);
			ant = _prim;
			while (act->_sig != NULL) {
				act = act->_sig;
				ant->_sig = new Nodo(act->_elem);
				ant = ant->_sig;
			}
			_ult = ant;
			_numElems = other._numElems;
		}
	}

private:

	/**
	 Clase nodo que almacena internamente el elemento (de tipo T),
	 y un puntero al nodo siguiente, que podr�a ser NULL si
	 el nodo es el �ltimo de la lista enlazada.
	 */
	class Nodo {
	public:
		Nodo() : _sig(NULL) {}
		Nodo(const T &elem) : _elem(elem), _sig(NULL) {}
		Nodo(const T &elem, Nodo *sig) : 
		    _elem(elem), _sig(sig) {}

		T _elem;
		Nodo *_sig;
	};

	/**
	 Elimina todos los nodos de la lista enlazada cuyo
	 primer nodo se pasa como par�metro.
	 Se admite que el nodo sea NULL (no habr� nada que
	 liberar).
	 */
	static void libera(Nodo *prim) {
		while (prim != NULL) {
			Nodo *aux = prim;
			prim = prim->_sig;
			delete aux;
		}
	}

	/** Puntero al primer elemento. */
	Nodo *_prim;

	/** Puntero al �ltimo elemento. */
	Nodo *_ult;

	/** N�mero de elementos */
	int _numElems;
};

/**
 Implementaci�n din�mica del TAD Arbin utilizando 
 nodos con un puntero al hijo izquierdo y otro al
 hijo derecho. La implementaci�n permite compartici�n
 de estructura, manteniendola bajo control mediante
 conteo de referencias. La implementaci�n, sin embargo,
 es bastante artesanal, pues para no complicar el c�digo
 excesivamente no se ha hecho uso de punteros inteligentes
 que incrementen y decrementen autom�ticamente esas
 referencias.

 Las operaciones son:

 - ArbolVacio: -> Arbin. Generadora implementada en el
   constructor sin par�metros.
 - Cons: Arbin, Elem, Arbin -> Arbin. Generadora implementada
   en un constructor con tres par�metros.
 - hijoIz, hijoDr: Arbin - -> Arbin. Observadoras que
   devuelven el hijo izquiero o derecho de un �rbol.
 - esVacio: Arbin -> Bool. Observadora que devuelve si
   un �rbol binario es vac�o.

 @author Marco Antonio G�mez Mart�n
 */
template <class T>
class Arbin {
public:

	/** Constructor; operacion ArbolVacio */
	Arbin() : _ra(NULL) {
	}

	/** Constructor; operacion Cons */
	Arbin(const Arbin &iz, const T &elem, const Arbin &dr) :
		_ra(new Nodo(iz._ra, elem, dr._ra)) {
		_ra->addRef();
	}

	/** Destructor; elimina la estructura jer�rquica de nodos. */
	~Arbin() {
		libera();
		_ra = NULL;
	}

	/**
	 Devuelve el elemento almacenado en la raiz

	 raiz(Cons(iz, elem, dr)) = elem
	 error raiz(ArbolVacio)
	 @return Elemento en la ra�z.
	 */
	const T &raiz() const {
		if (esVacio())
			throw EArbolVacio();
		return _ra->_elem;
	}

	/**
	 Devuelve un �rbol copia del �rbol izquierdo.
	 Es una operaci�n parcial (falla con el �rbol vac�o).

	 hijoIz(Cons(iz, elem, dr)) = iz
	 error hijoIz(ArbolVacio)
	*/
	Arbin hijoIz() const {
		if (esVacio())
			throw EArbolVacio();

		return Arbin(_ra->_iz);
	}

	/**
	 Devuelve un �rbol copia del �rbol derecho.
	 Es una operaci�n parcial (falla con el �rbol vac�o).

	 hijoDr(Cons(iz, elem, dr)) = dr
	 error hijoDr(ArbolVacio)
	*/
	Arbin hijoDr() const {
		if (esVacio())
			throw EArbolVacio();

		return Arbin(_ra->_dr);
	}

	/**
	 Operaci�n observadora que devuelve si el �rbol
	 es vac�o (no contiene elementos) o no.

	 esVacio(ArbolVacio) = true
	 esVacio(Cons(iz, elem, dr)) = false
	 */
	bool esVacio() const {
		return _ra == NULL;
	}

	// //
	// RECORRIDOS SOBRE EL �RBOL
	// //

	Lista<T> preorden() const {
		Lista<T> ret;
		preordenAcu(_ra, ret);
		return ret;
	}

	Lista<T> inorden() const {
		Lista<T> ret;
		inordenAcu(_ra, ret);
		return ret;
	}

	Lista<T> postorden() const {
		Lista<T> ret;
		postordenAcu(_ra, ret);
		return ret;
	}

	Lista<T> niveles() const {

		if (esVacio())
			return Lista<T>();

		Lista<T> ret;
		Cola<Nodo*> porProcesar;
		porProcesar.ponDetras(_ra);

		while (!porProcesar.esVacia()) {
			Nodo *visita = porProcesar.primero();
			porProcesar.quitaPrim();
			ret.ponDr(visita->_elem);
			if (visita->_iz)
				porProcesar.ponDetras(visita->_iz);
			if (visita->_dr)
				porProcesar.ponDetras(visita->_dr);
		}

		return ret;
	}

	// //
	// OTRAS OPERACIONES OBSERVADORAS
	// //

	/**
	 Devuelve el n�mero de nodos de un �rbol.
	 */
	unsigned int numNodos() const {
		return numNodosAux(_ra);
	}

	/**
	 Devuelve la talla del �rbol.
	 */
	unsigned int talla() const {
		return tallaAux(_ra);
	}

	/**
	 Devuelve el n�mero de hojas de un �rbol.
	 */
	unsigned int numHojas() const {
		return numHojasAux(_ra);
	}

	// //
	// M�TODOS DE "FONTANER�A" DE C++ QUE HACEN VERS�TIL
	// A LA CLASE
	// //

	/** Constructor copia */
	Arbin(const Arbin<T> &other) : _ra(NULL) {
		copia(other);
	}

	/** Operador de asignaci�n */
	Arbin<T> &operator=(const Arbin<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

	/** Operador de comparaci�n. */
	bool operator==(const Arbin<T> &rhs) const {
		return comparaAux(_ra, rhs._ra);
	}

	bool operator!=(const Arbin<T> &rhs) const {
		return !(*this == rhs);
	}

protected:

	/**
	 Clase nodo que almacena internamente el elemento (de tipo T),
	 y los punteros al hijo izquierdo y al hijo derecho, as�
	 como el n�mero de referencias que hay.
	 */
	class Nodo {
	public:
		Nodo() : _iz(NULL), _dr(NULL), _numRefs(0) {}
		Nodo(Nodo *iz, const T &elem, Nodo *dr) : 
			_elem(elem), _iz(iz), _dr(dr), _numRefs(0) {
			if (_iz != NULL)
				_iz->addRef();
			if (_dr != NULL)
				_dr->addRef();
		}

		void addRef() { assert(_numRefs >= 0); _numRefs++; }
		void remRef() { assert(_numRefs > 0); _numRefs--; }

		T _elem;
		Nodo *_iz;
		Nodo *_dr;

		int _numRefs;
	};

	/**
	 Constructor protegido que crea un �rbol
	 a partir de una estructura jer�rquica existente.
	 Esa estructura jer�rquica SE COMPARTE, por lo que
	 se a�ade la referencia.
	 Se utiliza en hijoIz e hijoDr.
	 */
	Arbin(Nodo *raiz) : _ra(raiz) {
		if (_ra != NULL)
			_ra->addRef();
	}

	void libera() {
		libera(_ra);
	}

	void copia(const Arbin &other) {
		assert(this != &other);
		_ra = other._ra;
		if (_ra != NULL)
			_ra->addRef();
	}

	// //
	// M�TODOS AUXILIARES PARA LOS RECORRIDOS
	// //
	
	static void preordenAcu(Nodo *ra, Lista<T> &acu) {
		if (ra == NULL)
			return;

		acu.ponDr(ra->_elem);
		preordenAcu(ra->_iz, acu);
		preordenAcu(ra->_dr, acu);
	}

	static void inordenAcu(Nodo *ra, Lista<T> &acu) {
		if (ra == NULL)
			return;

		inordenAcu(ra->_iz, acu);
		acu.ponDr(ra->_elem);
		inordenAcu(ra->_dr, acu);
	}

	static void postordenAcu(Nodo *ra, Lista<T> &acu) {
		if (ra == NULL)
			return;

		postordenAcu(ra->_iz, acu);
		postordenAcu(ra->_dr, acu);
		acu.ponDr(ra->_elem);
	}

	// //
	// M�TODOS AUXILIARES (RECURSIVOS) DE OTRAS OPERACIONES
	// OBSERVADORAS
	// //

	static unsigned int numNodosAux(Nodo *ra) {
		if (ra == NULL)
			return 0;
		return 1 + numNodosAux(ra->_iz) + numNodosAux(ra->_dr);
	}

	static unsigned int tallaAux(Nodo *ra) {
		if (ra == NULL)
			return 0;

		int tallaiz = tallaAux(ra->_iz);
		int talladr = tallaAux(ra->_dr);
		if (tallaiz > talladr)
			return 1 + tallaiz;
		else
			return 1 + talladr;
	}

	static unsigned int numHojasAux(Nodo *ra) {
		if (ra == NULL)
			return 0;

		if ((ra->_iz == NULL) && (ra->_dr == NULL))
			return 1;

		return numHojasAux(ra->_iz) + numHojasAux(ra->_dr);
	}

private:

	/**
	 Elimina todos los nodos de una estructura arb�rea
	 que comienza con el puntero ra.
	 Se admite que el nodo sea NULL (no habr� nada que
	 liberar).
	 */
	static void libera(Nodo *ra) {
		if (ra != NULL) {
			ra->remRef();
			if (ra->_numRefs == 0) {
				libera(ra->_iz);
				libera(ra->_dr);
				delete ra;
			}
		}
	}

	/**
	 Compara dos estructuras jer�rquicas de nodos,
	 dadas sus raices (que pueden ser NULL).
	 */
	static bool comparaAux(Nodo *r1, Nodo *r2) {
		if (r1 == r2)
			return true;
		else if ((r1 == NULL) || (r2 == NULL))
			// En el if anterior nos aseguramos de
			// que r1 != r2. Si uno es NULL, el
			// otro entonces no lo ser�, luego
			// son distintos.
			return false;
		else {
			return (r1->_elem == r2->_elem) &&
				comparaAux(r1->_iz, r2->_iz) &&
				comparaAux(r1->_dr, r2->_dr);
		}
	}

protected:
	/** 
	 Puntero a la ra�z de la estructura jer�rquica
	 de nodos.
	 */
	Nodo *_ra;
};

#endif // __ARBOLES_H

/*
 * ignora espacios en un 'stream' (por ejemplo, cin)
 */
void ignoraEspacios(istream &in) {
  int c;
  bool ok = true;
  // in.peek() mira, *sin extraerlo*, el siguiente caracter de in
  while (ok)
  {
	  c = in.peek();
	  ok = (c == ' ' || c == '\t' || c == '\r'|| c == '\n');

	// es un espacio: extrae el caracter
	if(ok) in.get(); 
  }

  c = c;
}

/*
 * Lee un arbol binario de 'cin',
 * 	asumiendo un formato similar a la salida de
 * 	muestraEnPreorden (posiblemente con mas espacios)
 */
Arbin<int> leeArbolEnPreorden() {
  // ejemplo de entrada:
  // 5 7 -1 6 -1 3 -1 -1

  // ignora posibles espacios y lee un entero
  ignoraEspacios(cin);
  int valor;
  cin >> valor;
  if (valor == -1) {
	// devuelve vacio
	return Arbin<int>();
  } else {
	// leemos subarboles y devolvemos leido
	const Arbin<int> &iz = leeArbolEnPreorden();
	const Arbin<int> &dr = leeArbolEnPreorden();
	return Arbin<int>(iz, valor, dr);
  }
}

/*
 * Lee un arbol binario de 'cin',
 * 	asumiendo un formato similar a la salida de
 * 	muestraConParentesis (posiblemente con mas espacios)
 */
Arbin<int> leeArbolConParentesis() {
	int c1,c2,c3,c4,c5;
  // ejemplo de entrada:
  // (2 (1 () () ) (3 () (4 (5 () () ) (6 () () ) ) ) )

  // ignora parentesis de apertura y posibles espacios
  ignoraEspacios(cin);

  c1 = cin.get();
  c2 = cin.peek();

  assert(c1 == '(');
  ignoraEspacios(cin);
  // fin o recursion
  if (cin.peek() == ')') {
	// ignora parentesis de cierre y devuelve vacio
	cin.get();
	return Arbin<int>();
  } else {
	// leemos valor y subarboles
	int valor;
	cin >> valor;
	const Arbin<int> &iz = leeArbolConParentesis();
	const Arbin<int> &dr = leeArbolConParentesis();
	// ignora espacios + cierre y devuelve leido
	ignoraEspacios(cin);
 	assert(cin.get() == ')');
	return Arbin<int>(iz, valor, dr);
  }
}

/*
 * muestra un arbol de forma recursiva, indentado
 * entra:
 *    a - un arbol
 *    nivel - debe ser 0 para la raiz
 *            (solo se usa para generar indentacion)
 * sale:
 *    la representacion del arbol por cout
 */
void muestraIndentado(const Arbin<int> &a, int nivel) {
	// condicion de terminacion
	if (a.esVacio()) return;
  
	// string(5, 'v') crea una cadena "vvvvv" ...
    string espacios = string(nivel*2, ' '); 
	// recorrido en postorden, indentando cada nivel
	cout << espacios << a.raiz() << endl;
	muestraIndentado(a.hijoIz(), nivel+1);
	muestraIndentado(a.hijoDr(), nivel+1);
}

/*
 * muestra un arbol de forma recursiva, con parentesis
 * entra:
 *    a - un arbol
 *    nivel - debe ser 0 para la raiz
 *            (solo se usa para generar un salto de linea)
 * sale:
 *    la representacion del arbol por cout
 */
void muestraConParentesis(const Arbin<int> &a, int nivel) {
	// condicion de terminacion
	if (a.esVacio()) {
	  cout << " ()";
	} else {
	  // recorrido en postorden
	  cout << " (" << a.raiz();
	  muestraConParentesis(a.hijoIz(), nivel+1);
	  muestraConParentesis(a.hijoDr(), nivel+1);
	  cout << " )";
	}
	if (nivel == 0) {
	  cout << endl;
	}
}

/*
 * muestra un arbol de forma recursiva, en preorden,
 * 	usando -1 para indicar 'subarbol vacio'
 * entra:
 *    a - un arbol
 *    nivel - debe ser 0 para la raiz
 *            (solo se usa para generar un salto de linea)
 * sale:
 *    la representacion del arbol por cout
 */
void muestraEnPreorden(const Arbin<int> &a, int nivel) {
	// condicion de terminacion
	if (a.esVacio()) {
	  cout << " -1";
	} else {
	  // recorrido en postorden
	  cout << " " << a.raiz();
	  muestraEnPreorden(a.hijoIz(), nivel+1);
	  muestraEnPreorden(a.hijoDr(), nivel+1);
	}
	if (nivel == 0) {
	  cout << endl;
	}
}

// ejemplo de creacion estatica de un arbol sencillo
Arbin<int> arbolSencillo() {
	return Arbin<int>(
		Arbin<int>(Arbin<int>(), 1, Arbin<int>()), 2, 
		Arbin<int>(Arbin<int>(), 3, Arbin<int>(
			Arbin<int>(Arbin<int>(), 5, Arbin<int>()), 4, 
			Arbin<int>(Arbin<int>(), 6, Arbin<int>()))));
}


bool checksum(const Arbin<int> &tree, int sum,const int goal);

#define ISDIGIT(x) (x) >= '0' && (x) <= '9'

int main (void) {
	
	int goal;
	Arbin<int> arbol;
	Cola<bool> cola;

	while (cin >> goal) {
		arbol = leeArbolConParentesis();
		cola.ponDetras (checksum(arbol, 0, goal));

		cin.get();
	}

	while (!cola.esVacia ()) {
		if (cola.primero ())
			cout << "yes" << endl;
		else
			cout << "no" << endl;
		
		cola.quitaPrim ();
	}

	return 0;
}

bool checksum(const Arbin<int> &tree, int sum,const int goal)
{
	if(tree.esVacio())
		return false;
	else
	{
		if(!tree.hijoIz().esVacio() && !tree.hijoDr().esVacio())

			return checksum(tree.hijoIz(),sum + tree.raiz(),goal) || checksum(tree.hijoDr(),sum + tree.raiz(),goal);
		else if(!tree.hijoIz().esVacio())
			return checksum(tree.hijoIz(),sum + tree.raiz(),goal);
		else if(!tree.hijoDr().esVacio())
			return checksum(tree.hijoDr(),sum + tree.raiz(),goal);
		else 
			return sum + tree.raiz() == goal;
	}
}