#include "renderer.h"
#include <sstream>
#include <iostream>
#include <stdexcept>
void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(){
 try {
  gb::Video v;
  check(gb::render(v)[0]==0,"blank");
  v.ram[0]=0x80; v.ram[1]=0x40;
  auto f=gb::render(v); check(f[0]==1&&f[1]==2&&f[2]==0,"bitplanes");
  v.palette={3,2,1,0}; check(gb::render(v)[0]==2,"palette");
  v.palette={0,1,2,3}; v.scrollX=1; check(gb::render(v)[0]==2,"scroll");
  v.scrollX=-1; v.ram[0x1800+31]=1; v.ram[16]=1; check(gb::render(v)[0]==1,"wrap");
  v=gb::Video{}; v.tilemap=1; v.ram[0x1c00]=2; v.ram[32]=128; check(gb::render(v)[0]==1,"alternate map");
  v=gb::Video{}; v.tileset=0; v.ram[0x1800]=255; v.ram[0xff0]=128; check(gb::render(v)[0]==1,"signed tile");
  std::ostringstream out; for(int i=0;i<8192;++i)out<<0<<' '; out<<"1 0 0 0 0 1 2 3";
  std::istringstream in(out.str()); check(gb::render(gb::load(in))[0]==0,"snapshot");
  std::istringstream bad("256"); bool rejected=false; try{gb::load(bad);}catch(const std::runtime_error&){rejected=true;} check(rejected,"invalid snapshot");
  std::cout<<"All renderer checks passed\n"; return 0;
 } catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
