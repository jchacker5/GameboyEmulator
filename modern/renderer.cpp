#include "renderer.h"
#include <stdexcept>
namespace gb {
Frame render(const Video& v) {
 if ((v.tileset!=0 && v.tileset!=1)||(v.tilemap!=0 && v.tilemap!=1)) throw std::invalid_argument("invalid tile mode");
 for(int p:v.palette) if(p<0||p>3) throw std::invalid_argument("invalid palette");
 Frame frame{};
 for(int y=0;y<height;++y) for(int x=0;x<width;++x) {
  const unsigned sx=(static_cast<unsigned>(v.scrollX)+x)&255u, sy=(static_cast<unsigned>(v.scrollY)+y)&255u;
  const int tile=v.ram[(v.tilemap?0x1c00:0x1800)+(sy/8)*32+sx/8];
  const int base=v.tileset?tile*16:0x1000+(tile<128?tile:tile-256)*16;
  const int row=base+2*(sy%8), bit=7-(sx%8);
  const int index=((v.ram[row]>>bit)&1)|(((v.ram[row+1]>>bit)&1)<<1);
  frame[y*width+x]=static_cast<std::uint8_t>(v.palette[index]);
 }
 return frame;
}
Video load(std::istream& in) {
 Video v;
 for(auto& byte:v.ram) { int n; if(!(in>>n)||n<0||n>255) throw std::runtime_error("invalid video RAM"); byte=static_cast<std::uint8_t>(n); }
 if(!(in>>v.tileset>>v.tilemap>>v.scrollX>>v.scrollY)) throw std::runtime_error("missing video registers");
 for(auto& p:v.palette) if(!(in>>p)) throw std::runtime_error("missing palette");
 render(v);
 return v;
}
Video demo() {
 Video v;
 for(int row=0;row<8;++row) {v.ram[row*2]=0xaa; v.ram[row*2+1]=row%2?0xff:0;}
 for(int row=0;row<8;++row) {v.ram[16+row*2]=0x55; v.ram[17+row*2]=row%2?0:0xff;}
 for(int y=0;y<32;++y) for(int x=0;x<32;++x) v.ram[0x1800+y*32+x]=(x+y)%2;
 return v;
}
}
