#include "MoveSequence.h"

MoveSequence::MoveSequence()
{
   _head.next = &_tail;
   _tail.next = NULL;
}

MoveSequence::~MoveSequence()
{
}

size_t MoveSequence::size() const
{
   size_t n = 0;
   for (Entry* e = _head.next; e != &_tail; e = e->next) {
      ++n;
   }
   return n;
}

MoveSequence::iterator MoveSequence::search(const IMove* m)
{
   const MoveSequence* c = this;
   const_iterator i = c->search(m);
   return iterator( const_cast< Entry* >(i.e) );
}
MoveSequence::const_iterator MoveSequence::search(const IMove* m) const
{
   const_iterator i = begin();
   for (; i != end(); ++i) {
      if (*i == m) { return i; }
   }
   return i;
}

MoveSequence::iterator MoveSequence::searchAt(__int64 t, bool group)
{
   const MoveSequence* c = this;
   const_iterator i = c->searchAt(t, group);
   return iterator( const_cast< Entry* >(i.e) );
}
MoveSequence::const_iterator MoveSequence::searchAt(__int64 t, bool group) const
{
   __int64 t0,t1;
   const_iterator i = begin();
   for (; i != end(); ++i) {
      (*i)->getTime(&t0,&t1);
      if (t < t0) {
         return end();
      }
      const_iterator i0 = i;
      if (group) {
         while (i.e->group) { ++i; }
      }
      (*i)->getTime(&t0,&t1);
      if (t < t1) { return i0; }
   }
   return i;
}

MoveSequence::iterator MoveSequence::searchGE(__int64 t, bool group)
{
   const MoveSequence* c = this;
   const_iterator i = c->searchGE(t, group);
   return iterator( const_cast< Entry* >(i.e) );
}
MoveSequence::const_iterator MoveSequence::searchGE(__int64 t, bool group) const
{
   __int64 t0,t1;
   const_iterator i;
   for (i = begin(); i != end(); ++i) {
      (*i)->getTime(&t0,&t1);
      if (t < t1) { break; }
   }
   if (i == end() || !group) { return i; }
   return i.group_begin();
}

std::pair<MoveSequence::iterator, MoveSequence::iterator > MoveSequence::search(__int64 t0, __int64 t1)
{
   const MoveSequence* c = this;
   std::pair< const_iterator, const_iterator > p = c->search(t0, t1);
   std::pair< iterator, iterator > ret;

   ret.first  = iterator(const_cast< Entry* >(p.first.e));
   ret.second = iterator(const_cast< Entry* >(p.second.e));
   return ret;
}

std::pair<MoveSequence::const_iterator, MoveSequence::const_iterator > MoveSequence::search(__int64 t0, __int64 t1) const
{
   std::pair< const_iterator, const_iterator > ret(end(), end());
   __int64 s0,s1;

   {
      const_iterator b = begin();
      const_iterator e = end();

      // Move が空なら、結果は空
      if (b == e) { return ret; }

      // 最初の Move が指定範囲の後にあるなら、結果は空
      (*b)->getTime(&s0, &s1);
      if (t1 < s0) {
         return ret;
      }
      // 最後の Move が指定時刻の前にあるなら、結果は空
      const_iterator e2 = e;
      (*--e2)->getTime(&s0, &s1);
      if (s1 <= t0) {
         return ret;
      }
   }

   // 指定範囲と重なる最初の Move を探す
   const_iterator i;
   for (i = begin(); i != end(); ++i) {
      if ((*i)->timeOverlay(t0, t1)) { break; }
   }
   if (i == end()) { return ret; }
   ret.first = i;

   // 指定範囲と重ならない最初の Move を探す
   for (++i; i != end(); ++i) {
      if (! (*i)->timeOverlay(t0, t1)) { break; }
   }
   ret.second = --i;

   return ret;
}

bool MoveSequence::isChainPrev(const iterator& i) const
{
   const_iterator ci(i.e);
   return isChainPrev(ci);
}
bool MoveSequence::isChainNext(const iterator& i) const
{
   const_iterator ci(i.e);
   return isChainNext(ci);
}
bool MoveSequence::isChainPrev(const const_iterator& i) const
{
   const Entry* e = i.e;
   if (i == begin() || e == NULL) { return false; }
   return (e->prev->group);
}
bool MoveSequence::isChainNext(const const_iterator& i) const
{
   const Entry* e = i.e;
   if (i == end() || e == NULL) { return false; }

   const_iterator j = i;
   ++j;
   if (j == end()) { return false; }

   return (e->group);
}

bool MoveSequence::chainPrev(const iterator& i, bool chain)
{
   Entry* e = const_cast< Entry* >(i.e);
   if (i == begin() || e == NULL) { return false; }

   e->prev->group = chain;
   return true;
}
bool MoveSequence::chainNext(const iterator& i, bool chain)
{
   Entry* e = const_cast< Entry* >(i.e);
   if (i == end() || e == NULL) { return false; }

   iterator j = i;
   ++j;
   if (j == end()) { return false; }

   e->group = chain;
   return true;
}

// Entry を削除する。削除された IMove を返す。
IMove* MoveSequence::remove(iterator& i)
{
   Entry* e = const_cast< Entry* >(i.e);
   if (i == end() || e == NULL) { return false; }

   i.e->prev->next = i.e->next;
   i.e->next->prev = i.e->prev;

   IMove* r = i.e->pMove;
   delete i.e;
   i.e = NULL;
   return r;
}

// old を取り除き、rep を追加する。削除された IMove を返す。
IMove* MoveSequence::replace(iterator& i, IMove* rep)
{
   if (i == end() || i.e == NULL) { return false; }

   __int64 t0,t1,s0,s1;
   rep->getTime(&t0, &t1);
   rep->setUuid(i.e->pMove->getUuid());

   if (isChainPrev(i) || isChainNext(i)) {
      // 連結されている場合、前後のモデルをまたいでの移動は不可。
      if (i != begin()) {
         i.e->prev->pMove->getTime(&s0, &s1);
         if (t0 < s1) { return NULL; }
      }
      if (i.e->next->pMove != NULL) {
         i.e->next->pMove->getTime(&s0, &s1);
         if (s0 < t1) { return NULL; }
      }
      // 置き換え Model のリスト順序は変わらないので、Entry を再利用する
      IMove* old = i.e->pMove;
      i.e->pMove = rep;
      return old;

   } else {
      // 連結されていない場合、モデルまたぎを許す。add で代用

      // リスト上の位置が変わるか確認
      bool consist = true;
      {
         if (i != begin()) {
            i.e->prev->pMove->getTime(&s0, &s1);
            if (t0 < s1) { consist = false; }
         }
         if (consist && i.e->next->pMove != NULL) {
            i.e->next->pMove->getTime(&s0, &s1);
            if (s0 < t1) { consist = false; }
         }
      }
      if (consist) { // 置き換え. Model のリスト順序は変わらない
         IMove* old = i.e->pMove;
         i.e->pMove = rep;
         return old;

      } else { // 位置が変わる。既存のエントリを外して新規に追加する。
         // 置き換え元の Entry は副作用が起きないように外しておく
         i.e->prev->next = i.e->next;
         i.e->next->prev = i.e->prev;

         if (add(rep) != end()) {
            IMove* ret = i.e->pMove;
            delete i.e;
            i.e = NULL;
            return ret;
         } else {
            i.e->prev->next = i.e;
            i.e->next->prev = i.e;
            return NULL;
         }
      }      
   }
}

// IMove 情報を消去する。登録してあった IMove を返す。
size_t MoveSequence::clear(std::vector< IMove* >* ret)
{
   size_t n = 0;
   for (iterator i=begin(); i!=end(); ++i) { ++n; }

   if (ret) {
      ret->resize(n);
      size_t a = 0;
      for (iterator i=begin(); i!=end(); ++i) {
         ret->at(a++) = *i;
      }
   }

   for (Entry* e = _head.next; e != &_tail; ) {
      Entry* e0 = e;
      e = e->next;
      delete e0;
   }
   _head.next = &_tail;
   _tail.prev = &_head;
   return n;
}

// 新たな IMove を追加する。重なる場合は失敗し、偽を返す。
MoveSequence::iterator MoveSequence::add(IMove* m)
{
   __int64 t0,t1;
   m->getTime(&t0, &t1);

   // search Move just after the new Move.
   Entry* e;
   for (e = _head.next; e != &_tail; e = e->next) {
      __int64 s0,s1;
      e->pMove->getTime(&s0, &s1);
      if (t0 < s0) { break; }
   }

   Entry* prev = e->prev;
   Entry* next = e;

   // check the previous Move and the next Move is not chained
   if (prev->pMove != NULL) {
      if (prev->group) { return end(); }
   }
   // check the new Move is not over the previous Move
   if (prev->pMove != NULL) {
      if (prev->pMove->timeOverlay(t0, t1)) { return end(); }
   }
   // check the new Move is not over the next Move
   if (next->pMove != NULL) {
      if (next->pMove->timeOverlay(t0, t1)) { return end(); }
   }
   /* 追加する側でセットアップすべき
   {
      MoveModel* impl = static_cast< MoveModel* >(m);
      if (impl->getMotion(Motion::WIIMOTE) == NULL) {
         impl->setMotion(new WiimoteMotionSimple());
      }
   }
   */

   e = new Entry();
   e->pMove = m;
   e->group = prev->group;

   e->prev = prev;
   e->next = next;
   next->prev = e;
   prev->next = e;

   return MoveSequence::iterator(e);
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
