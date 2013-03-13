/**
  @file Pila.h

  Implementación del TAD Pila utilizando un
  array estático.

  Estructura de Datos y Algoritmos
  Facultad de Informática
  Universidad Complutense de Madrid

 (c) Antonio Sánchez Ruiz-Granados, 2013
*/
#ifndef __PILAE_H
#define __PILAE_H

#include "Excepciones.h"

/**
 Implementación del TAD Pila utilizando un array estático.

 Las operaciones son:

 - PilaVacia: -> Pila. Generadora implementada en el
   constructor sin parámetros.
 - apila: Pila, Elem -> Pila. Generadora parcial.
 - desapila: Pila - -> Pila. Modificadora parcial.
 - cima: Pila - -> Elem. Observadora parcial.
 - esVacia: Pila -> Bool. Observadora.
 - numElems: Pila -> Entero. Observadora.

 @author Antonio Sánchez Ruiz-Granados
 */
template <class T>
class PilaE {
public:
	/** Número máximo de elementos. */
	static const int TAM_MAX = 100;

	/** Constructor; operación PilaVacia */
	PilaE() : _numElems(0) {	}

	/** Destructor. */
	~PilaE() { }

	/**
	 Apila un elemento. Operación generadora parcial.

	 @param elem Elemento a apilar.
	 error: apila(p) cuando numElems(p) == TAM_MAX
	*/
	void apila(const T &elem) {
		if (_numElems == TAM_MAX)
			throw EPilaLlena();
		
		_v[_numElems] = elem;
		_numElems++;
	}
	
	/**
	 Desapila un elemento. Operación modificadora parcial,
	 que falla si la pila está vacía.

	 desapila(Apila(elem, p)) = p
	 error: desapila(PilaVacia)
	*/
	void desapila() {
		if (esVacia())
			throw EPilaVacia();
		_numElems--;
	}

	/**
	 Devuelve el elemento en la cima de la pila. Operación
	 observadora parcial, que falla si la pila está vacía.

	 cima(Apila(elem, p) = elem
	 error: cima(PilaVacia)

	 @return Elemento en la cima de la pila.
	 */
	const T &cima() const {
		if (esVacia())
			throw EPilaVacia();
		return _v[_numElems-1];
	}

	/**
	 Devuelve true si la pila no tiene ningún elemento.

	 esVacia(PilaVacia) = true
	 esVacia(Apila(elem, p)) = false

	 @return true si la pila no tiene ningún elemento.
	 */
	bool esVacia() const {
		return _numElems == 0;
	}

	/**
	 Devuelve el número de elementos que hay en la
	 pila.
	 numElems(PilaVacia) = 0
	 numElems(Apila(elem, p)) = 1 + numElems(p)

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
	PilaE(const PilaE<T> &other) {
		copia(other);
	}

	/** Operador de asignación */
	PilaE<T> &operator=(const PilaE<T> &other) {
		if (this != &other) 
			copia(other);
		
		return *this;
	}

	/** Operador de comparación. */
	bool operator==(const PilaE<T> &rhs) const {
		if (_numElems != rhs._numElems)
			return false;
		
		for (unsigned int i = 0; i < _numElems; ++i)
			if (_v[i] != rhs._v[i])
				return false;
		
		return true;
	}

	bool operator!=(const PilaE<T> &rhs) const {
		return !(*this == rhs);
	}

protected:

	void copia(const PilaE<T> &other) {
		_numElems = other._numElems;
		for (unsigned int i = 0; i < _numElems; ++i)
			_v[i] = other._v[i];
	}

private:

	/** Array de elementos. */
	T _v[TAM_MAX];

	/** Número de elementos almacenados en la pila. */
	unsigned int _numElems;
};

#endif // __PILAE_H
