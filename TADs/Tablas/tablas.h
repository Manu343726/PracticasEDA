/**
 @file tablas.h
 
 Implementación del TAD Tabla usando una tabla hash abierta.
 Resultado de concatenar Excepciones.h, Hash.h y Tabla.h
 
 */
#ifndef __TABLAS_H
#define __TABLAS_H

#include <cassert>
#include <string>
#include <iosfwd>

/**
 @file Hash.h
  Definición de algunas excepciones de las distintas
  implementaciones de los TADs.

  Estructura de Datos y Algoritmos
  Facultad de Informática
  Universidad Complutense de Madrid

 (c) Marco Antonio Gómez Martín, 2012
*/

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


// Macro para declarar las clases de tipo excepción
// que heredan de ExcepcionConMensaje, para ahorrar
// escribir muchas veces lo mismo...
#define DECLARA_EXCEPCION(Excepcion) \
class Excepcion : public ExcepcionTAD { \
public: \
Excepcion() {}; \
Excepcion(const std::string &msg) : ExcepcionTAD(msg) {} \
};

/**
 Excepción generada por algunas operaciones de las pilas.
 */
DECLARA_EXCEPCION(EPilaVacia);

/**
 Excepción generada por algunas de las operaciones de las colas.
 */
DECLARA_EXCEPCION(EColaVacia);

/**
 Excepción generada por algunas operaciones de las colas dobles.
 */
DECLARA_EXCEPCION(EDColaVacia);

/**
 Excepción generada por algunas operaciones de las listas.
 */
DECLARA_EXCEPCION(EListaVacia);

/**
 Excepción generada por accesos incorrectos a las listas
 (tanto a un número de elemento incorrecto como por
 mal manejo de los iteradores).
 */
DECLARA_EXCEPCION(EAccesoInvalido);

/**
 Excepción generada por algunas operaciones de los
 árboles binarios.
 */
DECLARA_EXCEPCION(EArbolVacio);

/**
 Excepción generada al intentar acceder a un contenedor 
 asociativo con una clave incorrecta. 
 */
DECLARA_EXCEPCION(EClaveErronea);


/**
 @file Hash.h
 
 Declaración e implementación de funciones de localización para
 tipos básicos y función genérica que confía en la existencia
 del método método hash de las clases.
 
 Estructura de Datos y Algoritmos
 Facultad de Informática
 Universidad Complutense de Madrid
 
 (c) Antonio Sánchez Ruiz-Granados, 2012
 */

// ----------------------------------------------------
//
// Funciones hash para distintos tipos de datos básicos
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

// Nota: Esta función de hash para cadenas no es muy buena.
inline unsigned int hash(std::string clave) {
	
	// Suma los valores ASCII de todos sus caracters.
	unsigned int valor = 0;
	for (unsigned int i=0; i<clave.length(); ++i) {
		valor += clave[i];
	}
	return valor;
}


/**
 * Función hash genérica para clases que implementen un
 * método publico hash.
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
 Implementación del TAD Tabla usando una tabla hash abierta.
 
 Las operaciones públicas son:
 
 - TablaVacia: -> Tabla. Generadora (constructor).
 - inserta: Tabla, Clave, Valor -> Tabla. Generadora.
 - borra: Tabla, Clave -> Tabla. Modificadora.
 - esta: Tabla, Clave -> Bool. Observadora.
 - consulta: Tabla, Clave - -> Valor. Observadora parcial. 
 - esVacia: Tabla -> Bool. Observadora.
 
 @author Antonio Sánchez Ruiz-Granados
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
		
		/* Atributos públicos. */
		C _clave;    
		V _valor;   
		Nodo *_sig;  // Puntero al siguiente nodo.
	};
	
public:
	
	/**
	 * Tamaño inicial de la tabla.
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
	 * Inserta un nuevo par (clave, valor) en la tabla. Si ya existía un 
	 * elemento con esa clave, se actualiza su valor.
	 *
	 * @param clave clave del nuevo elemento.
	 * @param valor valor del nuevo elemento.
	 */
	void inserta(const C &clave, const V &valor) {
		
		// Si la ocupación es muy alta ampliamos la tabla
		float ocupacion = 100 * ((float) _numElems) / _tam; 
		if (ocupacion > MAX_OCUPACION)
			amplia();
		
		// Obtenemos el índice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Si la clave ya existía, actualizamos su valor
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		if (nodo != NULL) {
			nodo->_valor = valor;
		} else {
			
			// Si la clave no existía, creamos un nuevo nodo y lo insertamos
			// al principio
			_v[ind] = new Nodo(clave, valor, _v[ind]);
			_numElems++;
		}
	}
	
	/**
	 * Elimina el elemento de la tabla con la clave dada. Si no existía ningún
	 * elemento con dicha clave, la tabla no se modifica.
	 *
	 * @param clave clave del elemento a eliminar.
	 */
	void borra(const C &clave) {
		
		// Obtenemos el índice asociado a la clave.
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
			
			// Borramos el nodo extraído.
			delete act;
			_numElems--;
		}
	}
	
	/**
	 * Comprueba si la tabla contiene algún elemento con la clave dada.
	 *
	 * @param clave clave a buscar.
	 * @return si existe algún elemento con esa clave.
	 */
	bool esta(const C &clave) {
		// Obtenemos el índice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos un nodo que contenga esa clave.
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		return nodo != NULL;
	}
	
	/**
	 * Devuelve el valor asociado a la clave dada. Si la tabla no contiene 
	 * esa clave lanza una excepción.
	 *
	 * @param clave clave del elemento a buscar.
	 * @return valor asociado a dicha clave.
	 * @throw EClaveInexistente si la clave no existe en la tabla.
	 */
	V consulta(const C &clave) {
		
		// Obtenemos el índice asociado a la clave.
		unsigned int ind = ::hash(clave) % _tam;
		
		// Buscamos un nodo que contenga esa clave.
		Nodo *nodo = buscaNodo(clave, _v[ind]);
		if (nodo == NULL)
			throw EClaveErronea();
		
		return nodo->_valor;
	}

	/**
	 * Indica si la tabla está vacía, es decir, si no contiene ningún elemento.
	 *
	 * @return si la tabla está vacía.
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
		unsigned int _ind;		///< Índice actual en el vector _v
		const Tabla *_tabla;	///< Tabla que se está recorriendo
		
	};
	
	/**
	 * Devuelve un iterador al primer par (clave, valor) de la tabla. 
	 * El iterador devuelto coincidirá con final() si la tabla está vacía.
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
	 * Devuelve un iterador al final del recorrido (apunta más allá del último
	 * elemento de la tabla).
	 * @return iterador al final del recorrido.
	 */
	Iterador final() const {
		return Iterador(this, NULL, _tam);
	}
	
	
	// 
	// MÉTODOS DE "FONTANERÍA" DE C++ QUE HACEN VERSÁTIL A LA CLASE
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
	 * Operador de asignación.
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
	 * Libera toda la memoria dinámica reservada para la tabla.
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
	 * Hace una copia de la tabla que recibe como parámetro. Antes de llamar
	 * a este método se debe invocar al método "liberar".
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
	 * Este método duplica la capacidad del array de punteros actual.
	 */
	void amplia() {
		// Creamos un puntero al array actual y anotamos su tamaño.
		Nodo **vAnt = _v;
		unsigned int tamAnt = _tam;

		// Duplicamos el array en otra posición de memoria.
		_tam *= 2; 
		_v = new Nodo*[_tam];
		for (unsigned int i=0; i<_tam; ++i)
			_v[i] = NULL;
		
		// Recorremos el array original moviendo cada nodo a la nueva 
		// posición que le corresponde en el nuevo array.
		for (unsigned int i=0; i<tamAnt; ++i) {
			
			// IMPORTANTE: Al modificar el tamaño también se modifica el índice
			// asociado a cada nodo. Es decir, los nodos se mueven a posiciones
			// distintas en el nuevo array.
			
			// NOTA: por eficiencia movemos los nodos del array antiguo al 
			// nuevo, no creamos nuevos nodos. 
			
			// Recorremos la lista de nodos
			Nodo *nodo = vAnt[i];
			while (nodo != NULL) {
				Nodo *aux = nodo;
				nodo = nodo->_sig;
				
				// Calculamos el nuevo índice del nodo, lo desenganchamos del 
				// array antiguo y lo enganchamos al nuevo.
				unsigned int ind = ::hash(aux->_clave) % _tam;
				aux->_sig = _v[ind];
				_v[ind] = aux;
			}
		}
		
		// Borramos el array antiguo (ya no contiene ningún nodo).
		delete[] vAnt;
	}
	
	/**
	 * Busca un nodo a partir del nodo "act" que contenga la clave dada. Si lo 
	 * encuentra, "act" quedará apuntando a dicho nodo y "ant" al nodo anterior.
	 * Si no lo encuentra "act" quedará apuntando a NULL.
	 *
	 * @param clave clave del nodo que se busca.
	 * @param act [in/out] inicialmente indica el primer nodo de la búsqueda y 
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
	 * diferencia del otro método "buscaNodo", este no devuelve un puntero al
	 * nodo anterior.
	 *
	 * @param clave clave del nodo que se busca.
	 * @param prim nodo a partir del cual realizar la búsqueda. 
	 * @return nodo encontrado o NULL.
	 */
	static Nodo* buscaNodo(const C &clave, Nodo* prim) {
		Nodo *act = prim;
		Nodo *ant = NULL;
		buscaNodo(clave, act, ant);
		return act;
	}
		
	/**
	 * Ocupación máxima permitida antes de ampliar la tabla en tanto por cientos.
	 */
	static const unsigned int MAX_OCUPACION = 80;
	
	
	Nodo **_v;               ///< Array de punteros a Nodo.
	unsigned int _tam;       ///< Tamaño del array _v.
	unsigned int _numElems;  ///< Número de elementos en la tabla.
	

};

#endif // __TABLAS_H
