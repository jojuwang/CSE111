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
   node* curr = anchor()->next;
   while(curr != anchor() && less ((curr)->value.first, pair.first)){
      curr = curr->next;
   }
   node* n = new node(curr, curr->prev, pair);
   curr->prev->next = n;
   curr->prev = n;
   iterator i = n;
   cout << "insert called \n";
   return i;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   node* curr = anchor()->next;
   int key_found = 0;
   node* n ();
   while(curr != anchor() && curr->value.first != that){
      curr = curr->next;
      if (curr->value.first == that){
         key_found = 1;
      }
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
//it = find
//iterator next = ++it
   iterator next = ++position;
   position->prev->next = position->next;
   position->next->prev = position->prev;
   delete position;
   return next;
}


