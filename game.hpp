#include <iostream>
#include <cassert>
//ブロックの数（縦、横）
constexpr int f_height = 22;
constexpr int f_width = 19;

constexpr int size = 32; //1blockの大きさ
//ピクセル
constexpr int height = (f_height - 1) * size + 1;
constexpr int width = (f_width - 1) * size + 1;
//位置はピクセル数とする

enum{
  none, wall, //黒、壁
  pac, //Pac-Man
  red, blue, orange, pink //enemies
};
struct position {
  position(int y, int x): y(y), x(x){}
  public:
  //enemyを(dy,dx)平行移動する
  void move(int dy, int dx){
    y += dy, x += dx;
    assert(0 <= y && y < height);
    assert(0 <= x && x < width);
  }
  int get_y(){ return y; }
  int get_x(){ return x; }

  private:
  int y, x;
};

//red 
//blue
//orange
//pink
//赤色の敵

struct red {
  public:
  //int get_y(){ return pos.get_y(); }
  //int get_x(){ return pos.get_x(); }
  private:
  //position pos(10*size, 10*size);
} red_e;

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
  {none,none,none,none,none,none,none,wall,blue,orange,red,wall,none,none,none,none,none,none,none},
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

//前回の状態からどれだけ動くのかを出力する
//それを受けてpythonで移動の処理をする



//Python//
//fieldの値を取得
int get_field_val(int y, int x){
  assert(0 <= y && y < f_height);
  assert(0 <= x && x < f_width);
  return field[y][x];
}