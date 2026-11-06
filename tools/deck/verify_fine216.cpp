#include "../../native/deck/fine216.h"
#include <cassert>
#include <vector>
#include <iostream>
using namespace deck174;
int main(){unsigned cases=0;auto check=[&](bool ok){assert(ok);++cases;};deck216::FinePad pad;Frame f;f.buttons=bit(Button::LeftPadTouch);f.left_pad={1,1};check(!pad.sample(f,100).active);
 for(Vec2 d:std::vector<Vec2>{{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}}){f.left_pad=d;f.buttons=bit(Button::LeftPadClick);auto first=pad.sample(f,1000);check(first.began&&first.active&&!first.released&&std::fabs(std::hypot(first.move.x,first.move.y)-1)<.001f);
  bool repeat=false;for(unsigned n=0;n<1000;++n){auto held=pad.sample(f,2000+n*8000);repeat|=held.began||held.released||held.turn!=0;}check(!repeat);
  f.buttons=0;auto released=pad.sample(f,9000000);check(released.released&&!released.active&&released.move.x*d.x+released.move.y*d.y>.9f);check(!pad.sample(f,9008000).released);
 }
 f.left_pad={0,1};f.buttons=bit(Button::LeftPadClick);pad.sample(f,100);
 f.left_pad={.05f,.98f};check(!pad.sample(f,200).turning);
 f.left_pad={.4f,0};auto slipped=pad.sample(f,300);
 check(!slipped.turning&&slipped.turn==0&&slipped.move.y==1.f);
 check(pad.sample(f,400).move.y==1.f);f.buttons=0;check(pad.sample(f,500).released);

 f.left_pad={.75f,0};f.buttons=bit(Button::LeftPadClick);auto center=pad.sample(f,600);
 check(center.began&&center.turning&&!center.move.x&&!center.move.y);
 f.left_pad={.79f,.6f};check(pad.sample(f,650).turn==0);
 f.left_pad={.99f,0};auto drag=pad.sample(f,700);
 check(drag.turning&&drag.turn>25.f&&!drag.released);
 f.buttons=0;f.left_pad={};auto rotated=pad.sample(f,800);
 check(rotated.released&&rotated.turn==drag.turn&&!rotated.move.x&&!rotated.move.y);

 f.left_pad={.76f,0};f.buttons=bit(Button::LeftPadClick);check(!pad.sample(f,900).turning);
 f.left_pad={2,0};check(pad.sample(f,1000).cancel);
 f.buttons=0;f.left_pad={};check(!pad.sample(f,1100).released);
 f.buttons=bit(Button::LeftPadClick);pad.sample(f,1200);f.left_pad={-1,0};
 check(pad.sample(f,1300).turn<0);
 pad.reset();
 constexpr float diagonal=.70710678f;
 const Vec2 directions[]={{1,0},{diagonal,diagonal},{0,1},{-diagonal,diagonal},
                          {-1,0},{-diagonal,-diagonal},{0,-1},{diagonal,-diagonal}};
 for(int sector=0;sector<8;++sector) {
  for(int offset:{-23,-22,22,23}) {
   const float angle=(sector*45+offset)*.01745329252f;
   const int adjacent=offset<-22?-1:offset>22?1:0;
   const auto expected=directions[(sector+adjacent+8)%8];
   f.buttons=bit(Button::LeftPadClick);
   f.left_pad={.9f*std::cos(angle),.9f*std::sin(angle)};
   const auto pressed=pad.sample(f,2000);
   f.left_pad={-f.left_pad.x,-f.left_pad.y};
   const auto held=pad.sample(f,2100);
   f.buttons=0;
   const auto released=pad.sample(f,2200);
   check(pressed.began&&pressed.active&&!pressed.turning&&
         pressed.move.x==expected.x&&pressed.move.y==expected.y&&
         !held.began&&!held.released&&held.move.x==expected.x&&held.move.y==expected.y&&
         released.released&&released.move.x==expected.x&&released.move.y==expected.y);
  }
 }
 std::cout<<"PASS fine216: "<<cases<<" eight-sector, release-only, center-boundary and drag cases; held samples never reissue an order\n";
}
