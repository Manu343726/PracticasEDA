#include <iostream>
using namespace std;

#include <string>
#include <iosfwd>
#include <cassert>
#include <sstream>
#include <vector>
#include <cmath>

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

#define DECLARA_EXCEPCION(Excepcion) \
class Excepcion : public ExcepcionTAD { \
public: \
	Excepcion() {}; \
	Excepcion(const std::string &msg) : ExcepcionTAD(msg) {} \
};
DECLARA_EXCEPCION(EPilaVacia);

DECLARA_EXCEPCION(EColaVacia);

DECLARA_EXCEPCION(EDColaVacia);

DECLARA_EXCEPCION(EListaVacia);

DECLARA_EXCEPCION(EAccesoInvalido);

DECLARA_EXCEPCION(EArbolVacio);

DECLARA_EXCEPCION(EClaveErronea);

template <class T>
class Pila {
public:

	enum { TAM_INICIAL = 10 };

	Pila() {
		inicia();
	}
	~Pila() {
		libera();
	}
	void apila(const T &elem) {
		if (_numElems == _tam)
			amplia();
		_v[_numElems] = elem;
		_numElems++;
	}

	void desapila() {
		if (esVacia())
			throw EPilaVacia();
		--_numElems;
	}

	const T &cima() const {
		if (esVacia())
			throw EPilaVacia();
		return _v[_numElems - 1];
	}
	bool esVacia() const {
		return _numElems == 0;
	}
	int numElems() const {
		return _numElems;
	}
	Pila(const Pila<T> &other) {
		copia(other);
	}
	Pila<T> &operator=(const Pila<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}
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

	T *_v;

	unsigned int _tam;

	unsigned int _numElems;
};

template <class T>
class Lista {
private:

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

	Lista() : _prim(NULL), _ult(NULL), _numElems(0) {}

	~Lista() {
		libera();
	}

	void Cons(const T &elem) {
		_numElems++;
		_prim = insertaElem(elem, NULL, _prim);
		if (_ult == NULL)
			_ult = _prim;
	}

	void ponDr(const T &elem) {
		_numElems++;
		_ult = insertaElem(elem, _ult, NULL);
		if (_prim == NULL)
			_prim = _ult;
	}
	const T &primero() const {
		if (esVacia())
			throw EListaVacia();
		return _prim->_elem;
	}

	const T &ultimo() const {
		if (esVacia())
			throw EListaVacia();

		return _ult->_elem;
	}
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
	bool esVacia() const {
		return _prim == NULL;
	}

	unsigned int numElems() const {
		return _numElems;
	}
	const T &elem(unsigned int idx) const {
		if (idx >= _numElems)
			throw EAccesoInvalido();

		Nodo *aux = _prim;
		for (int i = 0; i < idx; ++i)
			aux = aux->_sig;

		return aux->_elem;
	}

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

		friend class Lista;

		Iterador() : _act(NULL) {}
		Iterador(Nodo *act) : _act(act) {}

		// Puntero al nodo actual del recorrido
		Nodo *_act;
	};

	Iterador principio() {
		return Iterador(_prim);
	}

	Iterador final() const {
		return Iterador(NULL);
	}

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

	void insertar(const T &elem, const Iterador &it) {

		// Caso especial: ¿añadir al principio?
		if (_prim == it._act) {
			Cons(elem);
		} else
			// Caso especial: ¿añadir al final?
			if (it._act == NULL) {
				ponDr(elem);
			}
			// Caso normal
			else {
				insertaElem(elem, it._act->_ant, it._act);
			}
	}
	Lista(const Lista<T> &other) : _prim(NULL), _ult(NULL) {
		copia(other);
	}
	Lista<T> &operator=(const Lista<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}
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

		_prim = 0;
		_numElems = 0;

		Nodo *act = other._prim;
		while (act != NULL) {
			ponDr(act->_elem);
			act = act->_sig;
		}
	}

private:

	static Nodo *insertaElem(const T &e, Nodo *nodo1, Nodo *nodo2) {
		Nodo *nuevo = new Nodo(nodo1, e, nodo2);
		if (nodo1 != NULL)
			nodo1->_sig = nuevo;
		if (nodo2 != NULL)
			nodo2->_ant = nuevo;
		return nuevo;
	}

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
	static void libera(Nodo *prim) {
		assert(!prim || !prim->_ant);

		while (prim != NULL) {
			Nodo *aux = prim;
			prim = prim->_sig;
			delete aux;
		}
	}

	Nodo *_prim, *_ult;

	unsigned int _numElems;
};

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
	Añade un elemento en la parte trasera de la cola.
	Operación generadora.

	@param elem Elemento a añadir.
	*/
	void ponDetras(const T &elem) {
		Nodo *nuevo = new Nodo(elem, NULL);

		if (_ult != NULL)
			_ult->_sig = nuevo;
		_ult = nuevo;
		// Si la cola estaba vacía, el primer elemento
		// es el que acabamos de añadir
		if (_prim == NULL)
			_prim = nuevo;
		_numElems++;
	}

	/**
	Elimina el primer elemento de la cola.
	Operación modificadora parcial, que falla si 
	la cola está vacía.

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
		// Si la cola se quedó vacía, no hay
		// último
		if (_prim == NULL)
			_ult = NULL;
	}

	/**
	Devuelve el primer elemento de la cola. Operación
	observadora parcial, que falla si la cola está vacía.

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
	Devuelve true si la cola no tiene ningún elemento.

	esVacia(Cola) = true
	esVacia(PonDetras(elem, p)) = false

	@return true si la cola no tiene ningún elemento.
	*/
	bool esVacia() const {
		return _prim == NULL;
	}

	/**
	Devuelve el número de elementos que hay en la
	cola.
	numElems(ColaVacia) = 0
	numElems(PonDetras(elem, p)) = 1 + numElems(p)

	@return Número de elementos.
	*/
	int numElems() const {
		return _numElems;
	}

	// //
	// MÉTODOS DE "FONTANERÍA" DE C++ QUE HACEN VERSÁTIL
	// A LA CLASE
	// //

	/** Constructor copia */
	Cola(const Cola<T> &other) : _prim(NULL), _ult(NULL) {
		copia(other);
	}

	/** Operador de asignación */
	Cola<T> &operator=(const Cola<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

	/** Operador de comparación. */
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
	y un puntero al nodo siguiente, que podría ser NULL si
	el nodo es el último de la lista enlazada.
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
	primer nodo se pasa como parámetro.
	Se admite que el nodo sea NULL (no habrá nada que
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

	/** Puntero al último elemento. */
	Nodo *_ult;

	/** Número de elementos */
	int _numElems;
};

/**
Implementación dinámica del TAD Arbin utilizando 
nodos con un puntero al hijo izquierdo y otro al
hijo derecho. La implementación permite compartición
de estructura, manteniendola bajo control mediante
conteo de referencias. La implementación, sin embargo,
es bastante artesanal, pues para no complicar el código
excesivamente no se ha hecho uso de punteros inteligentes
que incrementen y decrementen automáticamente esas
referencias.

Las operaciones son:

- ArbolVacio: -> Arbin. Generadora implementada en el
constructor sin parámetros.
- Cons: Arbin, Elem, Arbin -> Arbin. Generadora implementada
en un constructor con tres parámetros.
- hijoIz, hijoDr: Arbin - -> Arbin. Observadoras que
devuelven el hijo izquiero o derecho de un árbol.
- esVacio: Arbin -> Bool. Observadora que devuelve si
un árbol binario es vacío.

@author Marco Antonio Gómez Martín
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

	/** Destructor; elimina la estructura jerárquica de nodos. */
	~Arbin() {
		libera();
		_ra = NULL;
	}

	/**
	Devuelve el elemento almacenado en la raiz

	raiz(Cons(iz, elem, dr)) = elem
	error raiz(ArbolVacio)
	@return Elemento en la raíz.
	*/
	const T &raiz() const {
		if (esVacio())
			throw EArbolVacio();
		return _ra->_elem;
	}

	/**
	Devuelve un árbol copia del árbol izquierdo.
	Es una operación parcial (falla con el árbol vacío).

	hijoIz(Cons(iz, elem, dr)) = iz
	error hijoIz(ArbolVacio)
	*/
	Arbin hijoIz() const {
		if (esVacio())
			throw EArbolVacio();

		return Arbin(_ra->_iz);
	}

	/**
	Devuelve un árbol copia del árbol derecho.
	Es una operación parcial (falla con el árbol vacío).

	hijoDr(Cons(iz, elem, dr)) = dr
	error hijoDr(ArbolVacio)
	*/
	Arbin hijoDr() const {
		if (esVacio())
			throw EArbolVacio();

		return Arbin(_ra->_dr);
	}

	/**
	Operación observadora que devuelve si el árbol
	es vacío (no contiene elementos) o no.

	esVacio(ArbolVacio) = true
	esVacio(Cons(iz, elem, dr)) = false
	*/
	bool esVacio() const {
		return _ra == NULL;
	}

	// //
	// RECORRIDOS SOBRE EL ÁRBOL
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
	Devuelve el número de nodos de un árbol.
	*/
	unsigned int numNodos() const {
		return numNodosAux(_ra);
	}

	/**
	Devuelve la talla del árbol.
	*/
	unsigned int talla() const {
		return tallaAux(_ra);
	}

	/**
	Devuelve el número de hojas de un árbol.
	*/
	unsigned int numHojas() const {
		return numHojasAux(_ra);
	}

	// //
	// MÉTODOS DE "FONTANERÍA" DE C++ QUE HACEN VERSÁTIL
	// A LA CLASE
	// //

	/** Constructor copia */
	Arbin(const Arbin<T> &other) : _ra(NULL) {
		copia(other);
	}

	/** Operador de asignación */
	Arbin<T> &operator=(const Arbin<T> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

	/** Operador de comparación. */
	bool operator==(const Arbin<T> &rhs) const {
		return comparaAux(_ra, rhs._ra);
	}

	bool operator!=(const Arbin<T> &rhs) const {
		return !(*this == rhs);
	}

protected:

	/**
	Clase nodo que almacena internamente el elemento (de tipo T),
	y los punteros al hijo izquierdo y al hijo derecho, así
	como el número de referencias que hay.
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
	Constructor protegido que crea un árbol
	a partir de una estructura jerárquica existente.
	Esa estructura jerárquica SE COMPARTE, por lo que
	se añade la referencia.
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
	// MÉTODOS AUXILIARES PARA LOS RECORRIDOS
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
	// MÉTODOS AUXILIARES (RECURSIVOS) DE OTRAS OPERACIONES
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
	Elimina todos los nodos de una estructura arbórea
	que comienza con el puntero ra.
	Se admite que el nodo sea NULL (no habrá nada que
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
	Compara dos estructuras jerárquicas de nodos,
	dadas sus raices (que pueden ser NULL).
	*/
	static bool comparaAux(Nodo *r1, Nodo *r2) {
		if (r1 == r2)
			return true;
		else if ((r1 == NULL) || (r2 == NULL))
			// En el if anterior nos aseguramos de
			// que r1 != r2. Si uno es NULL, el
			// otro entonces no lo será, luego
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
	Puntero a la raíz de la estructura jerárquica
	de nodos.
	*/
	Nodo *_ra;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

/*
* ignora espacios en un 'stream' (por ejemplo, cin)
*/
void ignoraEspacios(istream &in) {
	int c;
	// in.peek() mira, *sin extraerlo*, el siguiente caracter de in
	while ((c = in.peek()) && 
		(c == ' ' || c == '\t' || 
		c == '\r'|| c == '\n')) {
			// es un espacio: extrae el caracter
			in.get(); 
	}
}

/*
* Lee un arbol binario de 'cin',
* 	asumiendo un formato similar a la salida de
* 	muestraEnPreorden (posiblemente con mas espacios)
*/
Arbin<int> leeArbolEnPreorden(istream& is = cin) {
	// ejemplo de entrada:
	// 5 7 -1 6 -1 3 -1 -1

	// ignora posibles espacios y lee un entero
	ignoraEspacios(is);
	int valor;
	is >> valor;

	if (valor == -1) {
		// devuelve vacio
		return Arbin<int>();
	} else {
		// leemos subarboles y devolvemos leido
		const Arbin<int> &iz = leeArbolEnPreorden(is);
		const Arbin<int> &dr = leeArbolEnPreorden(is);
		return Arbin<int>(iz, valor, dr);
	}
}

/*
* Lee un arbol binario de 'cin',
* 	asumiendo un formato similar a la salida de
* 	muestraConParentesis (posiblemente con mas espacios)
*/
Arbin<int> leeArbolConParentesis() {
	// ejemplo de entrada:
	// (2 (1 () () ) (3 () (4 (5 () () ) (6 () () ) ) ) )

	// ignora parentesis de apertura y posibles espacios
	ignoraEspacios(cin);
	assert(cin.get() == '(');
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

/**
 @file Hash.h
 
 DeclaraciÃ³n e implementaciÃ³n de funciones de localizaciÃ³n para
 tipos bÃ¡sicos y funciÃ³n genÃ©rica que confÃ­a en la existencia
 del mÃ©todo mÃ©todo hash de las clases.
 
 Estructura de Datos y Algoritmos
 Facultad de InformÃ¡tica
 Universidad Complutense de Madrid
 
 (c) Antonio SÃ¡nchez Ruiz-Granados, 2012
 */

// ----------------------------------------------------
//
// Funciones hash para distintos tipos de datos bÃ¡sicos
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

// Nota: Esta funciÃ³n de hash para cadenas no es muy buena.
inline unsigned int hash(std::string clave) {
	
	// Suma los valores ASCII de todos sus caracters.
	unsigned int valor = 0;
	for (unsigned int i=0; i<clave.length(); ++i) {
		valor += clave[i];
	}
	return valor;
}


/**
 * FunciÃ³n hash genÃ©rica para clases que implementen un
 * mÃ©todo publico hash.
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




/**
 ImplementaciÃ³n del TAD Tabla usando una tabla hash abierta.
 
 Las operaciones pÃºblicas son:
 
 - TablaVacia: -> Tabla. Generadora (constructor).
 - inserta: Tabla, Clave, Valor -> Tabla. Generadora.
 - borra: Tabla, Clave -> Tabla. Modificadora.
 - esta: Tabla, Clave -> Bool. Observadora.
 - consulta: Tabla, Clave - -> Valor. Observadora parcial. 
 - esVacia: Tabla -> Bool. Observadora.
 
 @author Antonio SÃ¡nchez Ruiz-Granados
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
		
		/* Atributos pÃºblicos. */
		C _clave;    
		V _valor;   
		Nodo *_sig;  // Puntero al siguiente nodo.
	};
	
public:
	
	/**
	 * TamaÃ±o inicial de la tabla.
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
	 * Inserta un nuevo par (clave, valor) en la tabla. Si ya existÃ­a un 
	 * elemento con esa clave, se actualiza su valor.
	 *
	 * @param clave clave del nuevo elemento.
	 * @param valor valor del nuevo elemento.
	 */
	void inserta(const C &clave, const V &valor) {
		
		// Si la ocupaciÃ³n es muy alta ampliamos la tabla
		float ocupacion = 100 * ((float) _numElems) / _tam; 
		if (ocupacion > MAX_OCUPACION)
			amplia();
		
		// Obtenemos el Ã­ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Si la clave ya existÃ­a, actualizamos su valor
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		if (nodo != NULL) {
			nodo->_valor = valor;
		} else {
			
			// Si la clave no existÃ­a, creamos un nuevo nodo y lo insertamos
			// al principio
			_v[ind] = new Nodo(clave, valor, _v[ind]);
			_numElems++;
		}
	}
	
	/**
	 * Elimina el elemento de la tabla con la clave dada. Si no existÃ­a ningÃºn
	 * elemento con dicha clave, la tabla no se modifica.
	 *
	 * @param clave clave del elemento a eliminar.
	 */
	void borra(const C &clave) {
		
		// Obtenemos el Ã­ndice asociado a la clave.
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
			
			// Borramos el nodo extraÃ­do.
			delete act;
			_numElems--;
		}
	}
	
	/**
	 * Comprueba si la tabla contiene algÃºn elemento con la clave dada.
	 *
	 * @param clave clave a buscar.
	 * @return si existe algÃºn elemento con esa clave.
	 */
	bool esta(const C &clave) {
		// Obtenemos el Ã­ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos un nodo que contenga esa clave.
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		return nodo != NULL;
	}
	
	/**
	 * Devuelve el valor asociado a la clave dada. Si la tabla no contiene 
	 * esa clave lanza una excepciÃ³n.
	 *
	 * @param clave clave del elemento a buscar.
	 * @return valor asociado a dicha clave.
	 * @throw EClaveInexistente si la clave no existe en la tabla.
	 */
	V& consulta(const C &clave) {
		
		// Obtenemos el Ã­ndice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos un nodo que contenga esa clave.
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		if (nodo == NULL)
			throw EClaveErronea();
		
		return nodo->_valor;
	}

	/**
	 * Indica si la tabla estÃ¡ vacÃ­a, es decir, si no contiene ningÃºn elemento.
	 *
	 * @return si la tabla estÃ¡ vacÃ­a.
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
		unsigned int _ind;		///< Ãndice actual en el vector _v
		const Tabla *_tabla;	///< Tabla que se estÃ¡ recorriendo
		
	};
	
	/**
	 * Devuelve un iterador al primer par (clave, valor) de la tabla. 
	 * El iterador devuelto coincidirÃ¡ con final() si la tabla estÃ¡ vacÃ­a.
	 * @return iterador al primer par (clave, valor) de la tabla.
	 */
	 Iterador principio() const {
		
		unsigned int ind = 0;
		Nodo* act = _v[ind];
		
		while ((act == NULL) && (ind < _tam - 1)) {
			++ind;
			act = _v[ind];
		}
		
		return Iterador(this, act, ind);
	}
	
	/**
	 * Devuelve un iterador al final del recorrido (apunta mÃ¡s allÃ¡ del Ãºltimo
	 * elemento de la tabla).
	 * @return iterador al final del recorrido.
	 */
	Iterador final() const {
		return Iterador(this, NULL, _tam);
	}
	
	
	// 
	// MÃ‰TODOS DE "FONTANERÃA" DE C++ QUE HACEN VERSÃTIL A LA CLASE
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
	 * Operador de asignaciÃ³n.
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
	 * Libera toda la memoria dinÃ¡mica reservada para la tabla.
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
	 * Hace una copia de la tabla que recibe como parÃ¡metro. Antes de llamar
	 * a este mÃ©todo se debe invocar al mÃ©todo "liberar".
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
	 * Este mÃ©todo duplica la capacidad del array de punteros actual.
	 */
	void amplia() {
		// Creamos un puntero al array actual y anotamos su tamaÃ±o.
		Nodo **vAnt = _v;
		unsigned int tamAnt = _tam;

		// Duplicamos el array en otra posiciÃ³n de memoria.
		_tam *= 2; 
		_v = new Nodo*[_tam];
		for (unsigned int i=0; i<_tam; ++i)
			_v[i] = NULL;
		
		// Recorremos el array original moviendo cada nodo a la nueva 
		// posiciÃ³n que le corresponde en el nuevo array.
		for (unsigned int i=0; i<tamAnt; ++i) {
			
			// IMPORTANTE: Al modificar el tamaÃ±o tambiÃ©n se modifica el Ã­ndice
			// asociado a cada nodo. Es decir, los nodos se mueven a posiciones
			// distintas en el nuevo array.
			
			// NOTA: por eficiencia movemos los nodos del array antiguo al 
			// nuevo, no creamos nuevos nodos. 
			
			// Recorremos la lista de nodos
			Nodo *nodo = vAnt[i];
			while (nodo != NULL) {
				Nodo *aux = nodo;
				nodo = nodo->_sig;
				
				// Calculamos el nuevo Ã­ndice del nodo, lo desenganchamos del 
				// array antiguo y lo enganchamos al nuevo.
				unsigned int ind = ::hash(aux->_clave) % _tam;
				aux->_sig = _v[ind];
				_v[ind] = aux;
			}
		}
		
		// Borramos el array antiguo (ya no contiene ningÃºn nodo).
		delete[] vAnt;
	}
	
	/**
	 * Busca un nodo a partir del nodo "act" que contenga la clave dada. Si lo 
	 * encuentra, "act" quedarÃ¡ apuntando a dicho nodo y "ant" al nodo anterior.
	 * Si no lo encuentra "act" quedarÃ¡ apuntando a NULL.
	 *
	 * @param clave clave del nodo que se busca.
	 * @param act [in/out] inicialmente indica el primer nodo de la bÃºsqueda y 
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
	 * diferencia del otro mÃ©todo "buscaNodo", este no devuelve un puntero al
	 * nodo anterior.
	 *
	 * @param clave clave del nodo que se busca.
	 * @param prim nodo a partir del cual realizar la bÃºsqueda. 
	 * @return nodo encontrado o NULL.
	 */
	static Nodo* buscaNodo(const C &clave, Nodo* prim) {
		Nodo *act = prim;
		Nodo *ant = NULL;
		buscaNodo(clave, act, ant);
		return act;
	}
		
	/**
	 * OcupaciÃ³n mÃ¡xima permitida antes de ampliar la tabla en tanto por cientos.
	 */
	static const unsigned int MAX_OCUPACION = 80;
	
	
	Nodo **_v;               ///< Array de punteros a Nodo.
	unsigned int _tam;       ///< TamaÃ±o del array _v.
	unsigned int _numElems;  ///< NÃºmero de elementos en la tabla.
	

};

/**
 Implementación dinámica del TAD Arbus utilizando 
 nodos con un puntero al hijo izquierdo y otro al
 hijo derecho.

 Las operaciones son:

 - ArbusVacio: operación generadora que construye
 un árbol de búsqueda vacío.

 - Inserta(clave, valor): generadora que añade una 
 nueva pareja (clave, valor) al árbol. Si la
 clave ya estaba se sustituye el valor.

 - borra(clave): operación modificadora. Elimina la
 clave del árbol de búsqueda.  Si la clave no está,
 la operación no tiene efecto.

 - consulta(clave): operación observadora que devuelve
 el valor asociado a una clave. Es un error preguntar
 por una clave que no existe.

 - esta(clave): operación observadora. Sirve para
 averiguar si se ha introducido una clave en el
 árbol.

 - esVacio(): operacion observadora que indica si
 el árbol de búsqueda tiene alguna clave introducida.

 @author Marco Antonio Gómez Martín
 */
template <class Clave, class Valor>
class Arbus {
private:
	/**
	 Clase nodo que almacena internamente la pareja (clave, valor)
	 y los punteros al hijo izquierdo y al hijo derecho.
	 */
	class Nodo {
	public:
		Nodo() : _iz(NULL), _dr(NULL) {}
		Nodo(const Clave &clave, const Valor &valor) 
			: _clave(clave), _valor(valor), _iz(NULL), _dr(NULL) {}
		Nodo(Nodo *iz, const Clave &clave, const Valor &valor, Nodo *dr)
			: _clave(clave), _valor(valor), _iz(iz), _dr(dr) {}

		Clave _clave;
		Valor _valor;
		Nodo *_iz;
		Nodo *_dr;
	};

public:

	/** Constructor; operacion ArbolVacio */
	Arbus() : _ra(NULL) {
	}

	/** Destructor; elimina la estructura jerárquica de nodos. */
	~Arbus() {
		libera();
		_ra = NULL;
	}

	//Elimina todos los nodos (Reinicializa el arbol)
	void clear() {libera();}

	/**
	 Operación generadora que añade una nueva clave/valor
	 a un árbol de búsqueda.
	 @param clave Clave nueva.
	 @param valor Valor asociado a esa clave. Si la clave
	 ya se había insertado previamente, sustituimos el valor
	 viejo por el nuevo.
	 */
	void inserta(const Clave &clave, const Valor &valor) {
		_ra = insertaAux(clave, valor, _ra);
	}

	/**
	 Operación modificadora que elimina una clave del árbol.
	 Si la clave no existía la operación no tiene efecto.

	   borra(elem, ArbusVacio) = ArbusVacio
	   borra(e, inserta(c, v, arbol)) = 
	                     inserta(c, v, borra(e, arbol)) si c != e
	   borra(e, inserta(c, v, arbol)) = borra(e, arbol) si c == e

	 @param clave Clave a eliminar.
	 */
	void borra(const Clave &clave) {
		_ra = borraAux(_ra, clave);
	}

	/**
	 Operación observadora que devuelve el valor asociado
	 a una clave dada.

	 consulta(e, inserta(c, v, arbol)) = v si e == c
	 consulta(e, inserta(c, v, arbol)) = consulta(e, arbol) si e != c
	 error consulta(ArbusVacio)

	 @param clave Clave por la que se pregunta.
	 */
	 Valor &consulta(const Clave &clave) {
		Nodo *p = buscaAux(_ra, clave);
		if (p == NULL)
			throw EClaveErronea();

		return p->_valor;
	}

	/**
	 Operación observadora que permite averiguar si una clave
	 determinada está o no en el árbol de búsqueda.

	 esta(e, ArbusVacio) = false
	 esta(e, inserta(c, v, arbol)) = true si e == c
	 esta(e, inserta(c, v, arbol)) = esta(e, arbol) si e != c

	 @param clave Clave por la que se pregunta.
	 */
	bool esta(const Clave &clave) {
		return buscaAux(_ra, clave) != NULL;
	}

	/**
	 Operación observadora que devuelve si el árbol
	 es vacío (no contiene elementos) o no.

	 esVacio(ArbusVacio) = true
	 esVacio(inserta(c, v, arbol)) = false
	 */
	bool esVacio() const {
		return _ra == NULL;
	}

	// //
	// OPERACIONES RELACIONADAS CON LOS ITERADORES
	// //

	/**
	 Clase interna que implementa un iterador sobre
	 la lista que permite recorrer la lista e incluso
	 alterar el valor de sus elementos.
	 */
	class Iterador {
	public:
		void avanza() {
			if (_act == NULL) throw EAccesoInvalido();

			// Si hay hijo derecho, saltamos al primero
			// en inorden del hijo derecho
			if (_act->_dr)
				_act = primeroInOrden(_act->_dr);
			else {
				// Si no, vamos al primer ascendiente
				// no visitado. Para eso consultamos
				// la pila; si ya está vacía, no quedan
				// ascendientes por visitar
				if (_ascendientes.esVacia())
					_act = NULL;
				else {
					_act = _ascendientes.cima();
					_ascendientes.desapila();
				}
			}
		}

		const Clave &clave() const {
			if (_act == NULL) throw EAccesoInvalido();
			return _act->_clave;
		}

		const Valor &valor() const {
			if (_act == NULL) throw EAccesoInvalido();
			return _act->_valor;
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
		friend class Arbus;

		Iterador() : _act(NULL) {}
		Iterador(Nodo *act) {
			_act = primeroInOrden(act);
		}

		/**
		 Busca el primer elemento en inorden de
		 la estructura jerárquica de nodos pasada
		 como parámetro; va apilando sus ascendientes
		 para poder "ir hacia atrás" cuando sea necesario.
		 @param p Puntero a la raíz de la subestructura.
		 */
		Nodo *primeroInOrden(Nodo *p) {
			if (p == NULL)
				return NULL;

			while (p->_iz != NULL) {
				_ascendientes.apila(p);
				p = p->_iz;
			}
			return p;
		}

		// Puntero al nodo actual del recorrido
		// NULL si hemos llegado al final.
		Nodo *_act;

		// Ascendientes del nodo actual
		// aún por visitar
		Pila<Nodo*> _ascendientes;
	};
	
	/**
	 Devuelve el iterador al principio de la lista.
	 @return iterador al principio de la lista;
	 coincidirá con final() si la lista está vacía.
	 */
	Iterador principio() {
		return Iterador(_ra);
	}

	/**
	 @return Devuelve un iterador al final del recorrido
	 (fuera de éste).
	 */
	Iterador final() const {
		return Iterador(NULL);
	}


	// //
	// MÉTODOS DE "FONTANERÍA" DE C++ QUE HACEN VERSÁTIL
	// A LA CLASE
	// //

	/** Constructor copia */
	Arbus(const Arbus<Clave, Valor> &other) : _ra(NULL) {
		copia(other);
	}

	/** Operador de asignación */
	Arbus<Clave, Valor> &operator=(const Arbus<Clave, Valor> &other) {
		if (this != &other) {
			libera();
			copia(other);
		}
		return *this;
	}

protected:

	/**
	 Constructor protegido que crea un árbol
	 a partir de una estructura jerárquica de nodos
	 previamente creada.
	 Se utiliza en hijoIz e hijoDr.
	 */
	Arbus(Nodo *raiz) : _ra(raiz) {
	}

	void libera() {
		libera(_ra);
		_ra=NULL;
	}

	void copia(const Arbus &other) {
		_ra = copiaAux(other._ra);
	}

private:

	/**
	 Elimina todos los nodos de una estructura arbórea
	 que comienza con el puntero ra.
	 Se admite que el nodo sea NULL (no habrá nada que
	 liberar).
	 */
	static void libera(Nodo *ra) {
		if (ra != NULL) {
			libera(ra->_iz);
			libera(ra->_dr);

			delete ra;
		}
	}

	/**
	 Copia la estructura jerárquica de nodos pasada
	 como parámetro (puntero a su raiz) y devuelve un
	 puntero a una nueva estructura jerárquica, copia
	 de anterior (y que, por tanto, habrá que liberar).
	 */
	static Nodo *copiaAux(Nodo *ra) {
		if (ra == NULL)
			return NULL;

		return new Nodo(copiaAux(ra->_iz),
						ra->_clave, ra->_valor,
						copiaAux(ra->_dr));
	}

	/**
	 Inserta una pareja (clave, valor) en la estructura
	 jerárquica que comienza en el puntero pasado como parámetro.
	 Ese puntero se admite que sea NULL, por lo que se creará
	 un nuevo nodo que pasará a ser la nueva raíz de esa
	 estructura jerárquica. El método devuelve un puntero a la
	 raíz de la estructura modificada. En condiciones normales
	 coincidirá con el parámetro recibido; sólo cambiará si
	 la estructura era vacía.
	 @param clave Clave a insertar. Si ya aparecía en la
	 estructura de nodos, se sobreescribe el valor.
	 @param valor Valor a insertar.
	 @param p Puntero al nodo raíz donde insertar la pareja.
	 @return Nueva raíz (o p si no cambia).
	 */
	static Nodo *insertaAux(const Clave &clave, const Valor &valor, Nodo *p) {

		if (p == NULL) {
			return new Nodo(clave, valor);
		} else if (p->_clave == clave) {
			p->_valor = valor;
			return p;
		} else if (clave < p->_clave) {
			p->_iz = insertaAux(clave, valor, p->_iz);
			return p;
		} else { // (clave > p->_clave)
			p->_dr = insertaAux(clave, valor, p->_dr);
			return p;
		}
	}

	/**
	 Busca una clave en la estructura jerárquica de
	 nodos cuya raíz se pasa como parámetro, y devuelve
	 el nodo en la que se encuentra (o NULL si no está).
	 @param p Puntero a la raíz de la estructura de nodos
	 @param clave Clave a buscar
	 */
	static Nodo *buscaAux(Nodo *p, const Clave &clave) {
		if (p == NULL)
			return NULL;

		if (p->_clave == clave)
			return p;

		if (clave < p->_clave)
			return buscaAux(p->_iz, clave);
		else
			return buscaAux(p->_dr, clave);
	}

	/**
	 Elimina (si existe) la clave/valor de la estructura jerárquica
	 de nodos apuntada por p. Si la clave aparecía en la propia raíz,
	 ésta cambiará, por lo que se devuelve la nueva raíz. Si no cambia
	 se devuelve p.

	 @param p Raíz de la estructura jerárquica donde borrar la clave.
	 @param clave Clave a borrar.
	 @return Nueva raíz de la estructura, tras el borrado. Si la raíz
	 no cambia, se devuelve el propio p.
	*/
	static Nodo *borraAux(Nodo *p, const Clave &clave) {

		if (p == NULL)
			return NULL;

		if (clave == p->_clave) {
			return borraRaiz(p);
		} else if (clave < p->_clave) {
			p->_iz = borraAux(p->_iz, clave);
			return p;
		} else { // clave > p->_clave
			p->_dr = borraAux(p->_dr, clave);
			return p;
		}
	}

	/**
	 Borra la raíz de la estructura jerárquica de nodos
	 y devuelve el puntero a la nueva raíz que garantiza
	 que la estructura sigue siendo válida para un árbol de
	 búsqueda (claves ordenadas).
	 */
	static Nodo *borraRaiz(Nodo *p) {

		Nodo *aux;

		// Si no hay hijo izquierdo, la raíz pasa a ser
		// el hijo derecho
		if (p->_iz == NULL) {
			aux = p->_dr;
			delete p;
			return aux;
		} else
		// Si no hay hijo derecho, la raíz pasa a ser
		// el hijo izquierdo
		if (p->_dr == NULL) {
			aux = p->_iz;
			delete p;
			return aux;
		} else {
		// Convertimos el elemento más pequeño del hijo derecho
		// en la raíz.
			return mueveMinYBorra(p);
		}
	}

	/**
	 Método auxiliar para el borrado; recibe un puntero a la
	 raíz a borrar. Busca el elemento más pequeño del hijo derecho
	 que se convertirá en la raíz (que devolverá), borra la antigua
	 raíz (p) y "cose" todos los punteros, de forma que ahora:

	   - El mínimo pasa a ser la raíz, cuyo hijo izquierdo y
	     derecho eran los hijos izquierdo y derecho de la raíz
	     antigua.
	   - El hijo izquierdo del padre del elemento más pequeño
	     pasa a ser el antiguo hijo derecho de ese mínimo.
	 */
	static Nodo *mueveMinYBorra(Nodo *p) {

		// Vamos bajando hasta que encontramos el elemento
		// más pequeño (aquel que no tiene hijo izquierdo).
		// Vamos guardando también el padre (que será null
		// si el hijo derecho es directamente el elemento
		// más pequeño).
		Nodo *padre = NULL;
		Nodo *aux = p->_dr;
		while (aux->_iz != NULL) {
			padre = aux;
			aux = aux->_iz;
		}

		// aux apunta al elemento más pequeño.
		// padre apunta a su padre (si el nodo es hijo izquierdo)

		// Dos casos dependiendo de si el padre del nodo con 
		// el mínimo es o no la raíz a eliminar
		// (=> padre != NULL)
		if (padre != NULL) {
			padre->_iz = aux->_dr;
			aux->_iz = p->_iz;
			aux->_dr = p->_dr;
		} else {
			aux->_iz = p->_iz;
		}

		delete p;
		return aux;
	}

	/** 
	 Puntero a la raíz de la estructura jerárquica
	 de nodos.
	 */
	Nodo *_ra;
};

////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned int uint;
typedef Arbin<int> Arbol;

typedef Lista<Tabla<int,int> >::Iterador ResultIterator;

const char* DEBUG_INPUT = "5 7 -1 6 -1 -1 3 -1 -1\n"
						  "8 2 9 -1 -1 6 5 -1 -1 12 -1 -1 3 7 -1 -1 -1\n"
						  "-1\n";

const bool DEBUGGING = false;

#define Arbusto Arbus

void Authum(Arbusto<int,int>& leaves, int pos , Arbol& arbol) {
	if (!arbol.esVacio()) {
		if(leaves.esta(pos))
		{
			leaves.consulta(pos) += arbol.raiz();
		}
		else
		{
			leaves.inserta(pos,arbol.raiz());
		}

		Authum(leaves,pos-1,arbol.hijoIz());
		Authum(leaves,pos+3,arbol.hijoDr());
	}
}

void printLeaves(Arbusto<int,int>& leaves) {
	stringstream output("",ios_base::out | ios_base::in);

	Arbusto<int,int>::Iterador it = leaves.principio();
	
	for(int i = it.clave() ; it != leaves.final() ; ++i)
		if(leaves.esta(i))
		{
			output << leaves.consulta(i) << " ";
			it.avanza();
		}
		else
			output << "0 ";
		

	output.get();//elimina el ultimo espacio

	cout << output.str() << endl << endl;
}

void printResults(Lista<Arbusto<int,int> >& results)
{
	for(uint i = 0; i < results.numElems() ; ++i)
	{
		cout << "Case " << i+1 << ":" << endl;
		printLeaves((Arbusto<int,int>&)results.elem(i));//mmm...
		cout << endl;
	}
}

void _699(istream& is = cin)
{
	Lista <Arbus<int,int> > results;

	while (!is.eof()) {
		Arbin<int> a;
		Arbusto<int,int> leaves;
		int offset;

		a = leeArbolEnPreorden(is);
		if (!a.esVacio()) {
			Authum(leaves,0, a);

			results.ponDr(leaves);
		}

		ignoraEspacios(is);
	}

	printResults (results);
}

int main (void) {
	if(DEBUGGING)
	{
		stringstream input((string)DEBUG_INPUT,ios::in);

		_699(input);

		cin.sync();
		cin.get();
	}
	else
		_699();

	return 0;
}

