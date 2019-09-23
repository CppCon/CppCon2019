/*
Copyright Rene Rivera 2019
Distributed under the Boost Software License, Version 1.0.
(See accompanying file BOOST_SOFTWARE_LICENSE.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/
#include "std.hpp"
int main() {
  vector<int> cards(24); iota(cards.begin(),cards.end(),0);
  mt19937 r(random_device{}()); shuffle(cards.begin(),cards.end(),r);
  int U; cin>>U; cout<<"cards["<<U<<"]: "; U = cards[U]; cout<<U<<"\n";
  shuffle(cards.begin(),cards.end(),r);
  for (int i: {0,1,2}) {
    vector<int> decks[4];
    accumulate(cards.begin(),cards.end(),0,[&](int d, int c) {
      decks[d%4].insert(decks[d%4].begin(),c); return (d+1)%4; });
    int d = distance(decks, find_if(decks,decks+4,
      [&](auto &x) { return count(x.begin(),x.end(),U) > 0; }));
    cards.clear(); for (int i: {d,d+1,d+2,d+3}) {
      move(decks[i%4].begin(),decks[i%4].end(),back_inserter(cards)); }
  }
  cout<<cards[4]<<" == "<<U<<"\n"; assert(cards[4] == U);
}