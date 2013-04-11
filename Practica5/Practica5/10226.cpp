/**
  @file Arbus.h

  Implementaci�n din�mica del TAD Arbol de B�squeda.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/

/*
  Definici�n de algunas excepciones de las distintas
  implementaciones de los TADs.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/
#ifndef __EXCEPCIONES_H
#define __EXCEPCIONES_H

#include <string>
#include <iosfwd>
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
 Excepci�n generada por algunas operaciones de las pilas.
 */
DECLARA_EXCEPCION(EPilaLlena);

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
 �rboles de b�squeda.
 */
DECLARA_EXCEPCION(EClaveErronea);

#endif // __EXCEPCIONES_H

/**
  @file Lista.h

  Implementaci�n del TAD lista, utilizando una 
  lista doblemente enlazada.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/
#ifndef __LISTA_H
#define __LISTA_H

#include <cassert>

/**
 Implementaci�n del TAD Lista utilizando una lista doblemente enlazada.

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

#endif // __LISTA_ENLAZADA_H

/**
  @file Pila.h

  Implementaci�n del TAD Pila utilizando un
  vector din�mico cuyo tama�o va creciendo si
  es necesario.

  Estructura de Datos y Algoritmos
  Facultad de Inform�tica
  Universidad Complutense de Madrid

 (c) Marco Antonio G�mez Mart�n, 2012
*/
#ifndef __PILA_H
#define __PILA_H

/**
 Implementaci�n del TAD Pila utilizando vectores din�micos.

 Las operaciones son:

 - PilaVacia: -> Pila. Generadora implementada en el
   constructor sin par�metros.
 - apila: Pila, Elem -> Pila. Generadora
 - desapila: Pila - -> Pila. Modificadora parcial.
 - cima: Pila - -> Elem. Observadora parcial.
 - esVacia: Pila -> Bool. Observadora.
 - numElems: Pila -> Entero. Observadora.

 @author Marco Antonio G�mez Mart�n
 */
template <class T>
class Pila {
public:

	/** Tama�o inicial del vector din�mico. */
	enum { TAM_INICIAL = 10 };

	/** Constructor; operaci�n PilaVacia */
	Pila() {
		inicia();
	}

	/** Destructor; elimina el vector. */
	~Pila() {
		libera();
	}

	/**
	 Apila un elemento. Operaci�n generadora.

	 @param elem Elemento a apilar.
	*/
	void apila(const T &elem) {
		if (_numElems == _tam)
			amplia();
		_v[_numElems] = elem;
		_numElems++;
	}
	
	/**
	 Desapila un elemento. Operaci�n modificadora parcial,
	 que falla si la pila est� vac�a.

	 desapila(Apila(elem, p)) = p
	 error: desapila(PilaVacia)
	*/
	void desapila() {
		if (esVacia())
			throw EPilaVacia();
		--_numElems;
	}

	/**
	 Devuelve el elemento en la cima de la pila. Operaci�n
	 observadora parcial, que falla si la pila est� vac�a.

	 cima(Apila(elem, p) = elem
	 error: cima(PilaVacia)

	 @return Elemento en la cima de la pila.
	 */
	const T &cima() const {
		if (esVacia())
			throw EPilaVacia();
		return _v[_numElems - 1];
	}

	/**
	 Devuelve true si la pila no tiene ning�n elemento.

	 esVacia(PilaVacia) = true
	 esVacia(Apila(elem, p)) = false

	 @return true si la pila no tiene ning�n elemento.
	 */
	bool esVacia() const {
		return _numElems == 0;
	}

	/**
	 Devuelve el n�mero de elementos que hay en la
	 pila.
	 numElems(PilaVacia) = 0
	 numElems(Apila(elem, p)) = 1 + numElems(p)

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
	Pila(const Pila<T> &other) {
		copia(other);
	}

	/** Operador de asignaci�n */
	Pila<T> &operator=(const Pila<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

	/** Operador de comparaci�n. */
	bool operator==(const Pila<T> &rhs) const {
		if (_numElems != rhs._numElems)
			return false;
		for (unsigned int i = 0; i < _numElems; ++i)
			if (_v[i] != rhs._v[i])
				return false;
		return true;
	}

	bool operator!=(const Pila<T> &rhs) const {
		return !(*this == rhs);
	}

protected:

	void inicia() {
		_v = new T[TAM_INICIAL];
		_tam = TAM_INICIAL;
		_numElems = 0;
	}

	void libera() {
		delete []_v;
		_v = NULL;
	}

	void copia(const Pila &other) {
		_tam = other._numElems + TAM_INICIAL;
		_numElems = other._numElems;
		_v = new T[_tam];
		for (unsigned int i = 0; i < _numElems; ++i)
			_v[i] = other._v[i];
	}

	void amplia() {
		T *viejo = _v;
		_tam *= 2;
		_v = new T[_tam];

		for (unsigned int i = 0; i < _numElems; ++i)
			_v[i] = viejo[i];

		delete []viejo;
	}

private:

	/** Puntero al array que contiene los datos. */
	T *_v;

	/** Tama�o del vector _v. */
	unsigned int _tam;

	/** N�mero de elementos reales guardados. */
	unsigned int _numElems;
};

#endif // __PILA_H

/**
 @file Hash.h
 
 Declaraci�n e implementaci�n de funciones de localizaci�n para
 tipos b�sicos y funci�n gen�rica que conf�a en la existencia
 del m�todo m�todo hash de las clases.
 
 Estructura de Datos y Algoritmos
 Facultad de Inform�tica
 Universidad Complutense de Madrid
 
 (c) Antonio S�nchez Ruiz-Granados, 2012
 */

// ----------------------------------------------------
//
// Funciones hash para distintos tipos de datos b�sicos
//
// ----------------------------------------------------

inline unsigned int hash(unsigned int clave) {
	return clave;
}

inline unsigned int hash(int clave) {
	return (unsigned int) clave;
}

inline unsigned int hash(char clave) {
	return clave;
}

// Nota: Esta funci�n de hash para cadenas no es muy buena.
inline unsigned int hash(std::string clave) {
	
	// Suma los valores ASCII de todos sus caracters.
	unsigned int valor = 0;
	for (unsigned int i=0; i<clave.length(); ++i) {
		valor += clave[i];
	}
	return valor;
}


/**
 * Funci�n hash gen�rica para clases que implementen un
 * m�todo publico hash.
 */
template<class C>
unsigned int hash(const C &clave) {
	return clave.hash();
}


// ---------------------------------------------
//
// TAD Tabla 
//
// ---------------------------------------------

#define Tarbusto Tabla


/**
 Implementaci�n del TAD Tabla usando una tabla hash abierta.
 
 Las operaciones p�blicas son:
 
 - TablaVacia: -> Tabla. Generadora (constructor).
 - inserta: Tabla, Clave, Valor -> Tabla. Generadora.
 - borra: Tabla, Clave -> Tabla. Modificadora.
 - esta: Tabla, Clave -> Bool. Observadora.
 - consulta: Tabla, Clave - -> Valor. Observadora parcial. 
 - esVacia: Tabla -> Bool. Observadora.
 
 @author Antonio S�nchez Ruiz-Granados
 */
template <class C, class V>
class Tabla {
private:
	
	/**
	 * La tabla contiene un array de punteros a nodos. Cada nodo contiene una 
	 * clave, un valor y un puntero al siguiente nodo.
	 */
	class Nodo {
	public:
		/* Constructores. */
		Nodo(const C &clave, const V &valor) : 
				_clave(clave), _valor(valor), _sig(NULL) {};
		
		Nodo(const C &clave, const V &valor, Nodo *sig) : 
				_clave(clave), _valor(valor), _sig(sig) {};
		
		/* Atributos p�blicos. */
		C _clave;    
		V _valor;   
		Nodo *_sig;  // Puntero al siguiente nodo.
	};
	
public:
	
	/**
	 * Tama�o inicial de la tabla.
	 */
	static const int TAM_INICIAL = 10;
	
	/**
	 * Constructor por defecto. Crea una tabla con TAM_INICIAL
	 * posiciones.
	 */
	Tabla() : _v(new Nodo*[TAM_INICIAL]), _tam(TAM_INICIAL), _numElems(0) {
		for (unsigned int i=0; i<_tam; ++i) {
			_v[i] = NULL;
		}
	}
	
	/**
	 * Destructor.
	 */
	~Tabla() {
		libera();
	}
	
	/**
	 * Inserta un nuevo par (clave, valor) en la tabla. Si ya exist�a un 
	 * elemento con esa clave, se actualiza su valor.
	 *
	 * @param clave clave del nuevo elemento.
	 * @param valor valor del nuevo elemento.
	 */
	void inserta(const C &clave, const V &valor) {
		
		// Si la ocupaci�n es muy alta ampliamos la tabla
		float ocupacion = 100 * ((float) _numElems) / _tam; 
		if (ocupacion > MAX_OCUPACION)
			amplia();
		
		// Obtenemos el �ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Si la clave ya exist�a, actualizamos su valor
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		if (nodo != NULL) {
			nodo->_valor = valor;
		} else {
			
			// Si la clave no exist�a, creamos un nuevo nodo y lo insertamos
			// al principio
			_v[ind] = new Nodo(clave, valor, _v[ind]);
			_numElems++;
		}
	}
	
	/**
	 * Elimina el elemento de la tabla con la clave dada. Si no exist�a ning�n
	 * elemento con dicha clave, la tabla no se modifica.
	 *
	 * @param clave clave del elemento a eliminar.
	 */
	void borra(const C &clave) {
		
		// Obtenemos el �ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos el nodo que contiene esa clave y el nodo anterior.
		Nodo *act = _v[ind];
		Nodo *ant = NULL;
		buscaNodo(clave, act, ant);
		
		if (act != NULL) {
			
			// Sacamos el nodo de la secuencia de nodos.
			if (ant != NULL) {
				ant->_sig = act->_sig;
			} else {
				_v[ind] = act->_sig;
			}
			
			// Borramos el nodo extra�do.
			delete act;
			_numElems--;
		}
	}
	
	/**
	 * Comprueba si la tabla contiene alg�n elemento con la clave dada.
	 *
	 * @param clave clave a buscar.
	 * @return si existe alg�n elemento con esa clave.
	 */
	bool esta(const C &clave) {
		// Obtenemos el �ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos un nodo que contenga esa clave.
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		return nodo != NULL;
	}
	
	/**
	 * Devuelve el valor asociado a la clave dada. Si la tabla no contiene 
	 * esa clave lanza una excepci�n.
	 *
	 * @param clave clave del elemento a buscar.
	 * @return valor asociado a dicha clave.
	 * @throw EClaveInexistente si la clave no existe en la tabla.
	 */
	V consulta(const C &clave) {
		
		// Obtenemos el �ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos un nodo que contenga esa clave.
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		if (nodo == NULL)
			throw EClaveErronea();
		
		return nodo->_valor;
	}

	/**
	 * Indica si la tabla est� vac�a, es decir, si no contiene ning�n elemento.
	 *
	 * @return si la tabla est� vac�a.
	 */
	bool esVacia() {
		return _numElems == 0;
	}
	
	/**
	 * Clase interna que implementa un iterador sobre el conjunto de pares
	 * (clave, valor). Es importante tener en cuenta que el iterador puede
	 * devolver el conunto de pares en cualquier orden.
	 */
	class Iterador {
	public:
		void avanza() {
			if (_act == NULL) throw EAccesoInvalido();
			
			// Buscamos el siguiente nodo de la lista de nodos.
			_act = _act->_sig;
			
			// Si hemos llegado al final de la lista de nodos, seguimos
			// buscando por el vector _v.
			while ((_act == NULL) && (_ind < _tabla->_tam - 1)) {
				++_ind;
				_act = _tabla->_v[_ind];
			}
		}
		
		const C& clave() const {
			if (_act == NULL) throw EAccesoInvalido();
			return _act->_clave;
		}
		
		const V& valor() const {
			if (_act == NULL) throw EAccesoInvalido();
			return _act->_valor;
		}
		
		bool operator==(const Iterador &other) const {
			return _act == other._act;
		}
		
		bool operator!=(const Iterador &other) const {
			return !(this->operator==(other));
		}
		
	private:
		// Para que pueda construir objetos del tipo iterador
		friend class Tabla;
		
		Iterador(const Tabla* tabla, Nodo* act, unsigned int ind) 
			: _tabla(tabla), _act(act), _ind(ind) { }

		
		Nodo* _act;				///< Puntero al nodo actual del recorrido
		unsigned int _ind;		///< �ndice actual en el vector _v
		const Tabla *_tabla;	///< Tabla que se est� recorriendo
		
	};
	
	/**
	 * Devuelve un iterador al primer par (clave, valor) de la tabla. 
	 * El iterador devuelto coincidir� con final() si la tabla est� vac�a.
	 * @return iterador al primer par (clave, valor) de la tabla.
	 */
	Iterador principio() {
		
		unsigned int ind = 0;
		Nodo* act = _v[ind];
		
		while ((act == NULL) && (ind < _tam - 1)) {
			++ind;
			act = _v[ind];
		}
		
		return Iterador(this, act, ind);
	}
	
	/**
	 * Devuelve un iterador al final del recorrido (apunta m�s all� del �ltimo
	 * elemento de la tabla).
	 * @return iterador al final del recorrido.
	 */
	Iterador final() const {
		return Iterador(this, NULL, _tam);
	}
	
	
	// 
	// M�TODOS DE "FONTANER�A" DE C++ QUE HACEN VERS�TIL A LA CLASE
	// 
	
	/**
	 * Constructor por copia.
	 *
	 * @param other tabla que se quiere copiar.
	 */
	Tabla(const Tabla<C,V> &other) {
		copia(other);
	}
	
	/**
	 * Operador de asignaci�n.
	 *
	 * @param other tabla que se quiere copiar.
	 * @return referencia a este mismo objeto (*this).
	 */
	Tabla<C,V> &operator=(const Tabla<C,V> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}
	
	
private:
	
	// Para que el iterador pueda acceder a la parte privada
	friend class Iterador;
	
	/**
	 * Libera toda la memoria din�mica reservada para la tabla.
	 */
	void libera() {
		
		// Liberamos las listas de nodos.
		for (unsigned int i=0; i<_tam; i++) {
			liberaNodos(_v[i]);
		}
		
		// Liberamos el array de punteros a nodos.
		if (_v != NULL) {
			delete[] _v;
			_v = NULL;
		}
	}
	
	/**
	 * Libera un nodo y todos los siguientes.
	 *
	 * @param prim puntero al primer nodo de la lista a liberar.
	 */
	static void liberaNodos(Nodo *prim) {
		
		while (prim != NULL) {
			Nodo *aux = prim;
			prim = prim->_sig;
			delete aux;
		}		
	}	

	/**
	 * Hace una copia de la tabla que recibe como par�metro. Antes de llamar
	 * a este m�todo se debe invocar al m�todo "liberar".
	 *
	 * @param other tabla que se quiere copiar.
	 */
	void copia(const Tabla<C,V> &other) {
		_tam = other._tam;
		_numElems = other._numElems;
	
		// Reservar memoria para el array de punteros a nodos.
		_v = new Nodo*[_tam];
		for (unsigned int i=0; i<_tam; ++i) { 
			_v[i] = NULL;
			
			// Copiar la lista de nodos de other._v[i] a _v[i].
			// La lista de nodos queda invertida con respecto a la original.
			Nodo *act = other._v[i];
			while (act != NULL) {
				_v[i] = new Nodo(act->_clave, act->_valor, _v[i]); 
				act = act->_sig;
			}
		}
	}
	
	/**
	 * Este m�todo duplica la capacidad del array de punteros actual.
	 */
	void amplia() {
		// Creamos un puntero al array actual y anotamos su tama�o.
		Nodo **vAnt = _v;
		unsigned int tamAnt = _tam;

		// Duplicamos el array en otra posici�n de memoria.
		_tam *= 2; 
		_v = new Nodo*[_tam];
		for (unsigned int i=0; i<_tam; ++i)
			_v[i] = NULL;
		
		// Recorremos el array original moviendo cada nodo a la nueva 
		// posici�n que le corresponde en el nuevo array.
		for (unsigned int i=0; i<tamAnt; ++i) {
			
			// IMPORTANTE: Al modificar el tama�o tambi�n se modifica el �ndice
			// asociado a cada nodo. Es decir, los nodos se mueven a posiciones
			// distintas en el nuevo array.
			
			// NOTA: por eficiencia movemos los nodos del array antiguo al 
			// nuevo, no creamos nuevos nodos. 
			
			// Recorremos la lista de nodos
			Nodo *nodo = vAnt[i];
			while (nodo != NULL) {
				Nodo *aux = nodo;
				nodo = nodo->_sig;
				
				// Calculamos el nuevo �ndice del nodo, lo desenganchamos del 
				// array antiguo y lo enganchamos al nuevo.
				unsigned int ind = ::hash(aux->_clave) % _tam;
				aux->_sig = _v[ind];
				_v[ind] = aux;
			}
		}
		
		// Borramos el array antiguo (ya no contiene ning�n nodo).
		delete[] vAnt;
	}
	
	/**
	 * Busca un nodo a partir del nodo "act" que contenga la clave dada. Si lo 
	 * encuentra, "act" quedar� apuntando a dicho nodo y "ant" al nodo anterior.
	 * Si no lo encuentra "act" quedar� apuntando a NULL.
	 *
	 * @param clave clave del nodo que se busca.
	 * @param act [in/out] inicialmente indica el primer nodo de la b�squeda y 
	 *            al finalizar indica el nodo encontrado o NULL.
	 * @param ant [out] puntero al nodo anterior a "act" o NULL.
	 */
	static void buscaNodo(const C &clave, Nodo* &act, Nodo* &ant) {
		ant = NULL;
		bool encontrado = false;
		while ((act != NULL) && !encontrado) {
			
			// Comprobar si el nodo actual contiene la clave buscada
			if (act->_clave == clave) {
				encontrado = true;
			} else {
				ant = act;
				act = act->_sig;
			}
		}
	}
	
	/**
	 * Busca un nodo a partir de "prim" que contenga la clave dada. A 
	 * diferencia del otro m�todo "buscaNodo", este no devuelve un puntero al
	 * nodo anterior.
	 *
	 * @param clave clave del nodo que se busca.
	 * @param prim nodo a partir del cual realizar la b�squeda. 
	 * @return nodo encontrado o NULL.
	 */
	static Nodo* buscaNodo(const C &clave, Nodo* prim) {
		Nodo *act = prim;
		Nodo *ant = NULL;
		buscaNodo(clave, act, ant);
		return act;
	}
		
	/**
	 * Ocupaci�n m�xima permitida antes de ampliar la tabla en tanto por cientos.
	 */
	static const unsigned int MAX_OCUPACION = 80;
	
	
	Nodo **_v;               ///< Array de punteros a Nodo.
	unsigned int _tam;       ///< Tama�o del array _v.
	unsigned int _numElems;  ///< N�mero de elementos en la tabla.
	

};

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

#define DEBUGGING 1
const char* DEBUG_INPUT_EXAMPLE = "1\n"
								  "Red Alder\n"
								  "Ash\n"
								  "Aspen\n"
								  "Basswood\n"
								  "Ash\n"
								  "Beech\n"
								  "Yellow Birch\n"
								  "Ash\n"
								  "Cherry\n"
								  "Cottonwood\n"
								  "Ash\n"
								  "Cypress\n"
								  "Red Elm\n"
								  "Gum\n"
								  "Hackberry\n"
								  "White Oak\n"
								  "Hickory\n"
								  "Pecan\n"
								  "Hard Maple\n"
								  "White Oak\n"
								  "Soft Maple\n"
								  "Red Oak\n"
								  "Red Oak\n"
								  "White Oak\n"
								  "Poplan\n"
								  "Sassafras\n"
								  "Sycamore\n"
								  "Black Walnut\n"
								  "Willow\n"
								  "\n";

#ifndef uint
typedef unsigned int uint;
#endif

#if DEBUGGING
#define DEBUG_TRACE(x) cout << x << endl; 
#else
#define DEBUG_TRACE(x)
#endif

#define TRUE(x) ((x) == 1)
#define FALSE(x) (!TRUE(x))

const char   ENDLINE_CHAR   = '\n';
const string ENDLINE_STRING = "\n";

void _10226(istream& input)
{
	Tarbusto<string , uint> countData;
	uint testsCount;
	uint treesCount;
	Lista<string> results;

	input >> testsCount;
	input.get();//Linea en blanco

	for(uint i = 0 ; i < testsCount ; ++i)
	{
		treesCount = 0;

		while(input.peek() != ENDLINE_CHAR)
		{
			string tree; std::getline(input,tree);
			uint treeCount;

			if( countData.esta(tree) ) 
			{
				treeCount = countData.consulta(tree);
				countData.inserta(tree , treeCount+1);
			}
			else
				countData.inserta(tree , 1);

			treesCount++;
		}

		vector<string> unorderedResults;

		for(Tarbusto<string , uint>::Iterador it = countData.principio() ; it != countData.final() ; it.avanza())
		{
			stringstream stream;

			stream << it.clave() << " " << std::fixed << std::setprecision(4) << (float)((it.valor() / (float)treesCount) * 100) << endl;

			unorderedResults.push_back( stream.str() );
		}

		std::sort(unorderedResults.begin() , unorderedResults.end());

		for(vector<string>::iterator it = unorderedResults.begin() ; it != unorderedResults.end() ; ++it)
			results.insertar(*it , results.final());

		input.get();//Linea vac�a entre tests.
		results.insertar( ENDLINE_STRING , results.final() );

		countData = Tarbusto<string , uint>();//Nueva tabla...Si tuviera clear() no har�a esta burrada 
	}

	for(Lista<string>::Iterador it = results.principio() ; it != results.final() ; it.avanza() )
		cout << it.elem();
}

int main()
{
	if(TRUE(DEBUGGING))
	{
		stringstream input(DEBUG_INPUT_EXAMPLE);

		_10226(input);

		cin.sync();
		cin.get();
	}
	else
		_10226(cin);

	return 0;
}
