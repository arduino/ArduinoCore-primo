/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "RingBufferT.h"
#include <string.h>

template<class T, int size> 
RingBufferT<T, size>::RingBufferT(){
    clear();
    _bufferSize = size;
}

template<class T, int size> 
bool RingBufferT<T, size>::put(T *value){
    if(isFull()) return false;
    _buffer[_head] = *value;
    _head = nextIndex(_head);
    return true;
}

template<class T, int size> 
bool RingBufferT<T, size>::get(T *value){
    if(isEmpty()) return false;
    *value = _buffer[_tail];
    _tail = nextIndex(_tail);
    return true;
}

template<class T, int size> 
bool RingBufferT<T, size>::peek(T *value){
    if(isEmpty()) return false;
    *value = _buffer[_tail];
    return true;
}

template<class T, int size> 
void RingBufferT<T, size>::clear(){
	_head = 0;
	_tail = 0;
}

template<class T, int size> 
int RingBufferT<T, size>::available(){
    if(_tail > _head) return _bufferSize + _head - _tail;
    else return _head - _tail;
}

template<class T, int size> 
int RingBufferT<T, size>::nextIndex(int index){
	return (uint32_t)(index + 1) % _bufferSize;
}

template<class T, int size> 
bool RingBufferT<T, size>::isFull(){
	return nextIndex(_head) == _tail;
}

template<class T, int size> 
bool RingBufferT<T, size>::isEmpty(){
    return _head == _tail;
}
