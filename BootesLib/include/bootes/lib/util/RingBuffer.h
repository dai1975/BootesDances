#ifndef BOOTES_LIB_UTIL_RINGBUFFER_H
#define BOOTES_LIB_UTIL_RINGBUFFER_H

#include "macros.h"
#include "../win.h"
#include <vector>

namespace bootes { namespace lib { namespace util {

template <typename T> class RingBuffer
{
public:
   RingBuffer(size_t s);
   virtual ~RingBuffer();

   inline size_t capacity() const { return _buffer.size(); }
   inline size_t size() const;
   inline bool empty() const;
   inline bool full() const;

   void clear();
   void swap(RingBuffer< T >& r);
   bool push_back(const T& v);
   bool pop_front(T& v);

private:
   std::vector< T > _buffer;
   size_t _wi, _ri;
   bool _bWrap;
};

template <typename T> RingBuffer< T >::RingBuffer(size_t s)
   : _buffer(s)
   , _wi(0), _ri(0), _bWrap(false)
{ }

template <typename T> RingBuffer< T >::~RingBuffer()
{ }

template <typename T> void RingBuffer< T >::clear()
{
   _wi = _ri = 0;
   _bWrap = false;
}

template <typename T> void RingBuffer< T >::swap(RingBuffer< T >& r)
{
   size_t wi = _wi;
   size_t ri = _ri;
   bool   bWrap = _bWrap;
   _wi = r._wi;
   _ri = r._ri;
   _bWrap = r._bWrap;
   r._wi = wi;
   r._ri = ri;
   r._bWrap = bWrap;
   _buffer.swap(r._buffer);
}

template <typename T> size_t RingBuffer< T >::size() const
{
   if (_bWrap) {
      return _buffer.size() - (_ri - _wi);
   } else {
      return _wi - _ri;
   }
}

template <typename T> bool RingBuffer< T >::empty() const
{
   return (!_bWrap && (_wi == _ri));
}

template <typename T> bool RingBuffer< T >::full() const
{
   return (_bWrap && (_wi == _ri));
}

template <typename T> bool RingBuffer< T >::push_back(const T& v)
{
   if (full()) { return false; }
   _buffer[_wi++] = v;
   if (_wi == _buffer.size()) {
      _wi = 0;
      _bWrap = true;
   }
   return true;
}

template <typename T> bool RingBuffer< T >::pop_front(T& v)
{
   if (empty()) { return false; }
   v = _buffer[_ri++];
   if (_ri == _buffer.size()) {
      _ri = 0;
      _bWrap = false;
   }
   return true;
}

} } }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
