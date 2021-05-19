#include <iostream>
#include <cassert>

//ブロックの数（縦、横）
constexpr int f_height = 22;
constexpr int f_width = 19;

constexpr int size = 32; //1blockの大きさ
//ピクセル
constexpr int height = (f_height - 1) * size + 1;
constexpr int width = (f_width - 1) * size + 1;

constexpr int pac_pos_y = 16, pac_pos_x = 9;
constexpr int red_pos_y = 10, red_pos_x = 10;
constexpr int blue_pos_y = 10, blue_pos_x = 8;
constexpr int oran_pos_y = 10, oran_pos_x = 9;
constexpr int pink_pos_y = 9, pink_pos_x = 9;

enum{
  none, wall, //黒、壁
  pac, //Pac-Man
  red, blue, orange, pink //enemies
};
//フィールドの初期状態
int field[f_height][f_width] = {
  {wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall},
  {wall,none,none,none,none,none,none,none,none,wall,none,none,none,none,none,none,none,none,wall},
  {wall,none,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,none,wall},
  {wall,none,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,none,wall},
  {wall,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,wall},
  {wall,none,wall,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,wall,none,wall},
  {wall,none,none,none,none,wall,none,none,none,wall,none,none,none,wall,none,none,none,none,wall},
  {wall,wall,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,none,none,none,none,none,none,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,wall,wall,pink,wall,wall,none,wall,none,wall,wall,wall,wall},
  {none,none,none,none,none,none,none,wall,blue,orange,blue,wall,none,none,none,none,none,none,none},
  {wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,none,none,none,none,none,none,wall,none,wall,wall,wall,wall},
  {wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall},
  {wall,none,none,none,none,none,none,none,none,wall,none,none,none,none,none,none,none,none,wall},
  {wall,none,wall,wall,none,wall,wall,wall,none,wall,none,wall,wall,wall,none,wall,wall,none,wall},
  {wall,none,none,wall,none,none,none,none,none,pac ,none,none,none,none,none,wall,none,none,wall},
  {wall,wall,none,wall,none,wall,none,wall,wall,wall,wall,wall,none,wall,none,wall,none,wall,wall},
  {wall,none,none,none,none,wall,none,none,none,wall,none,none,none,wall,none,none,none,none,wall},
  {wall,none,wall,wall,wall,wall,wall,wall,none,wall,none,wall,wall,wall,wall,wall,wall,none,wall},
  {wall,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,none,wall},
  {wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall}
};

//direction: 0,1,2,3 = up,left,down,right
//方向は小さいほうから優先度高め
struct position {
  position(int y, int x, int r = 0): y(y), x(x), rot(r){}
  int get_y(){ return y; }
  int get_x(){ return x; }
  int get_r(){ return rot; }
  void move(int dy, int dx){
    y += dy, x += dx;
    assert(0 <= y && y < height);
    assert(0 <= x && x < width);
  }
  //方向をrにセット
  void rotate(int r){
    assert(0 <= r && r < 4);
    rot = r;
  }
  private:
  int y,x,rot;
};
position pacman(pac_pos_y*size, pac_pos_x*size, 3); //初期状態は右を向いている
position red_enemy(red_pos_y*size, red_pos_x*size, 0);
position blue_enemy(blue_pos_y*size, blue_pos_x*size, 0);
position oran_enemy(oran_pos_y*size, oran_pos_x*size, 0);
position pink_enemy(pink_pos_y*size, pink_pos_x*size, 0);





//Pythonから毎フレーム呼び出される
  void update(){
    pacman.move(0, 1);
  }


//前回の状態からどれだけ動くのかを出力する
//それを受けてpythonで移動の処理をする
namespace python {
  //fieldの値を取得
  int get_field_val(int y, int x){
    assert(0 <= y && y < f_height);
    assert(0 <= x && x < f_width) ;
    return field[y][x];
  }
  //pacman, red,blue,orange,pink = 0,1,2,3,4
  //現在の位置を出力する
  int get_posy(int i){
    assert(0 <= i && i < 5);
    switch(i){
      case 0: return pacman.get_y();
      case 1: return red_enemy.get_y();
      case 2: return blue_enemy.get_y();
      case 3: return oran_enemy.get_y();
      case 4: return pink_enemy.get_y();
      default: assert(0); return -1;
    }
  }
  int get_posx(int i){
    assert(0 <= i && i < 5);
    switch(i){
      case 0: return pacman.get_x();
      case 1: return red_enemy.get_x();
      case 2: return blue_enemy.get_x();
      case 3: return oran_enemy.get_x();
      case 4: return pink_enemy.get_x();
      default: assert(0); return -1;
    }
  }
  int get_rot(int i){
    assert(0 <= i && i < 5);
    switch(i){
      case 0: return pacman.get_r();
      case 1: return red_enemy.get_r();
      case 2: return blue_enemy.get_r();
      case 3: return oran_enemy.get_r();
      case 4: return pink_enemy.get_r();
      default: assert(0); return -1;
    }
  }

  //パックマンの方向移動
  void turn(int r){
    pacman.rotate(r);
  }
};
