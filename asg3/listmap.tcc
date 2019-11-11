// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   // can I just call erase while anchor->next != anchor?
   node* curr = anchor()->next;
   while(curr != anchor()){
      node* temp = curr->next;
      delete curr;
      curr = temp;
   }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   node* curr = begin().where;
   //cout << "curr assigned" << endl;
   while(curr != end().where && less ((curr)->value.first, pair.first)){
      curr = curr->next;
      //cout << "curr moved" << endl;
   }
   //cout << "while loop passed" << endl;
   if (curr == end().where){
      node* start = new node(curr, curr->prev, pair);
      curr->prev->next = start;
      curr->prev = start;
      iterator result = start;
      return result;
   } else if (curr->value.first == pair.first){
      //cout << "if cond entered" << endl;
      curr->value.second = pair.second;
      return curr;
   } else {
      //cout << "else cond entered" << endl;
      node* n = new node(curr, curr->prev, pair);
      //cout << "new node created" << endl;
      curr->prev->next = n;
      curr->prev = n;
      iterator i = n;
      return i;
   }
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   node* curr = anchor()->next;
   node* n ();
   while(curr != anchor() && curr->value.first != that){
      curr = curr->next;
   }
   return curr;
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   iterator next = position.where->next;
   position.where->prev->next = position.where->next;
   position.where->next->prev = position.where->prev;
   delete position.where;
   return next;
}


