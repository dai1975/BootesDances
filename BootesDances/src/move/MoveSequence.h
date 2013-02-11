#ifndef BOOTESDANCES_MOVE_MOVESEQUENCE_H
#define BOOTESDANCES_MOVE_MOVESEQUENCE_H

#include "../include.h"
#include <bootes/dances/IMove.h>
#include <vector>

/*
MoveSequence は、時系列に沿った一連の Move の順序集合を提供する。
個々の Move は [開始時刻,終了時刻) を持つ。
MoveSequence は、Move が時間的に重ならないようにしている。 
*/
class GuideFactory;
class MotionFactory;
class MoveSequence
{
   struct Entry;
public:
   struct iterator {
      Entry* e;
      inline iterator(Entry* e = NULL);
      inline iterator(const iterator&);
      inline iterator& operator=(const iterator&);
      inline friend bool operator==(const iterator& l, const iterator& r);
      inline friend bool operator!=(const iterator& l, const iterator& r);
      inline IMove* operator*();
      inline bool is_group() const;
      inline iterator&  operator++();
      inline iterator   operator++(int);
      inline iterator&  operator--();
      inline iterator   operator--(int);
      inline iterator   group_begin();
      inline iterator   group_end();
   };
   struct const_iterator {
      const Entry* e;
      inline const_iterator(const Entry* e = NULL);
      inline const_iterator(const const_iterator&);
      inline const_iterator& operator=(const const_iterator&);
      inline friend bool operator==(const const_iterator& l, const const_iterator& r);
      inline friend bool operator!=(const const_iterator& l, const const_iterator& r);
      inline const IMove* operator*() const;
      inline bool is_group() const;
      inline const_iterator&  operator++();
      inline const_iterator   operator++(int);
      inline const_iterator&  operator--();
      inline const_iterator   operator--(int);
      inline const_iterator   group_begin() const;
      inline const_iterator   group_end() const;
   };

public:
   MoveSequence();
   virtual ~MoveSequence();

   inline void setGuideFactory(const GuideFactory* f) { _guide_factory = f; }
   inline void setMotionFactory(const MotionFactory* f) { _motion_factory = f; }
   inline const GuideFactory*  getGuideFactory() const { return _guide_factory; }
   inline const MotionFactory* getMotionFactory() const { return _motion_factory; }

   size_t size() const;
   inline iterator begin();
   inline iterator end();
   inline const_iterator begin() const;
   inline const_iterator end() const;

   //! returns Move which include time 't'
   //  if given 'group' is true, returns head Move of group which include time 't'
   iterator searchAt(__int64 t, bool group);
   const_iterator searchAt(__int64 t, bool group) const;

   //! returns Move which include time 't' or the first Move after 't'
   //  if given 'group' is true, returns head Move of group which include time 't' or first one.
   iterator searchGE(__int64 t, bool group);
   const_iterator searchGE(__int64 t, bool group) const;

   //! 期間内に開始点のある Move リストの [先頭,最後] を返す。
   std::pair<iterator,iterator> search(__int64 t0, __int64 t1);
   std::pair<const_iterator,const_iterator> search(__int64 t0, __int64 t1) const;

   iterator search(const IMove*);
   const_iterator search(const IMove*) const;

   bool isChainPrev(const iterator& i) const;
   bool isChainNext(const iterator& i) const;
   bool isChainPrev(const const_iterator& i) const;
   bool isChainNext(const const_iterator& i) const;

   //次のMoveと連結または切断する
   bool chainNext(const iterator& i, bool chain);
   //前のMoveと連結または切断する
   bool chainPrev(const iterator& i, bool chain);
   // 削除する。削除された MoveModel を返す。
   IMove* remove(iterator& i);
   // old を取り除き、rep を追加する。削除された MoveModel を返す。
   IMove* replace(iterator& old, IMove* rep);

   // 新たな MoveModel を追加する。時間が重なると失敗。
   iterator add(IMove*);
   // MoveModel 情報を消去する。登録してあった MoveModel を返す。
   size_t clear(std::vector<IMove*>* = NULL);

private:
   struct Entry {
      IMove* pMove;
      Entry *next, *prev;
      bool  group; //次の要素とグループになっているかどうか
      inline Entry(): pMove(NULL), next(NULL), prev(NULL), group(false) { }
      inline Entry(const Entry& r) { operator=(r); }
      inline Entry& operator=(const Entry& r) {
         pMove = r.pMove;
         next  = r.next;
         prev  = r.prev;
         group = r.group;
         return *this;
      }
   };
   Entry _head, _tail;
   const GuideFactory* _guide_factory;
   const MotionFactory* _motion_factory;
};

inline MoveSequence::iterator::iterator(Entry* e_): e(e_)
{ }
inline MoveSequence::iterator::iterator(const iterator& r)
{ operator=(r); }
inline MoveSequence::iterator& MoveSequence::iterator::operator=(const iterator& r)
{ e = r.e; return *this; }
inline bool operator==(const MoveSequence::iterator& l, const MoveSequence::iterator& r)
{ return l.e == r.e; }
inline bool operator!=(const MoveSequence::iterator& l, const MoveSequence::iterator& r)
{ return l.e != r.e; }
inline IMove* MoveSequence::iterator::operator*()
{ return e->pMove; }
inline bool MoveSequence::iterator::is_group() const
{ return e->group; }
inline MoveSequence::iterator&  MoveSequence::iterator::operator++()
{
   e = e->next;
   return *this;
}
inline MoveSequence::iterator   MoveSequence::iterator::operator++(int)
{
   iterator r(e);
   e = e->next;
   return r;
}
inline MoveSequence::iterator&  MoveSequence::iterator::operator--()
{
   e = e->prev;
   return *this;
}
inline MoveSequence::iterator   MoveSequence::iterator::operator--(int)
{
   iterator r(e);
   e = e->prev;
   return r;
}

inline MoveSequence::iterator   MoveSequence::iterator::group_begin()
{
   const_iterator ci(e);
   const_iterator cbi = ci.group_begin();
   return iterator(const_cast< Entry* >(cbi.e));
}
inline MoveSequence::iterator   MoveSequence::iterator::group_end()
{
   const_iterator ci(e);
   const_iterator cbi = ci.group_end();
   return iterator(const_cast< Entry* >(cbi.e));
}

inline MoveSequence::const_iterator::const_iterator(const Entry* e_): e(e_)
{ }
inline MoveSequence::const_iterator::const_iterator(const const_iterator& r)
{ operator=(r); }
inline MoveSequence::const_iterator& MoveSequence::const_iterator::operator=(const const_iterator& r)
{ e = r.e; return *this; }
inline bool operator==(const MoveSequence::const_iterator& l, const MoveSequence::const_iterator& r)
{ return l.e == r.e; }
inline bool operator!=(const MoveSequence::const_iterator& l, const MoveSequence::const_iterator& r)
{ return l.e != r.e; }
inline const IMove* MoveSequence::const_iterator::operator*() const
{ return e->pMove; }
inline bool MoveSequence::const_iterator::is_group() const
{ return e->group; }
inline MoveSequence::const_iterator&  MoveSequence::const_iterator::operator++()
{
   e = e->next;
   return *this;
}
inline MoveSequence::const_iterator   MoveSequence::const_iterator::operator++(int)
{
   const_iterator r(e);
   e = e->next;
   return r;
}
inline MoveSequence::const_iterator&  MoveSequence::const_iterator::operator--()
{
   e = e->prev;
   return *this;
}
inline MoveSequence::const_iterator   MoveSequence::const_iterator::operator--(int)
{
   const_iterator r(e);
   e = e->prev;
   return r;
}
inline MoveSequence::const_iterator   MoveSequence::const_iterator::group_begin() const
{
   const Entry *e0 = e;
   while (e0->prev != NULL && e0->prev->group) {
      e0 = e0->prev;
   }
   return const_iterator(e0);
}
inline MoveSequence::const_iterator   MoveSequence::const_iterator::group_end() const
{
   const Entry *e0 = e;
   while (e0->next != NULL && e0->group) {
      e0 = e0->next;
   }
   return const_iterator(e0->next);
}


inline MoveSequence::iterator MoveSequence::begin()
{
   return iterator(_head.next);
}
inline MoveSequence::iterator MoveSequence::end()
{
   return iterator(&_tail);
}
inline MoveSequence::const_iterator MoveSequence::begin() const
{
   return const_iterator(_head.next);
}
inline MoveSequence::const_iterator MoveSequence::end() const
{
   return const_iterator(&_tail);
}

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
