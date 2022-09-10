/*抽象クラス*/
class Motor {
  public:
    virtual void Run() = 0;
    virtual void Stop() = 0;
    virtual void Brake() = 0;
    virtual int Speed(int) = 0;
    virtual bool Reverse(bool) = 0;
};

/*モータードライバMD30C用クラス*/
class MD30C : public Motor {
  private:
    int pwm_pin;
    int dir_pin;
    int pwm_val;
    bool dir_flag;
    bool run_flag;
  public:
    MD30C(int, int);
    MD30C(int, int, bool);
    ~MD30C();
  public:
    void Run();
    void Stop();
    void Brake();
    int Speed(int);
    bool Reverse(bool);
};

/*コンストラクタ*/
MD30C::MD30C(int dir, int pwm) {
  pwm_pin = pwm;
  dir_pin = dir;
  pinMode(dir_pin, OUTPUT);
  Stop();
}
MD30C::MD30C(int dir, int pwm, bool flag) {
  pwm_pin = pwm;
  dir_pin = dir;
  pinMode(dir_pin, OUTPUT);
  Stop();
  Reverse(flag);
}

/*デストラクタ*/
//Arduinoの入出力ピンは何も設定しなければ入力が標準
MD30C::~MD30C() {
  pinMode(dir_pin, INPUT);
}

/*回転開始のメンバ関数*/
void MD30C::Run() {
  if (pwm_val > 0) {
    digitalWrite(dir_pin, dir_flag);
  } else if (pwm_val < 0) {
    digitalWrite(dir_pin, !dir_flag);
  }
  analogWrite(pwm_pin, abs(pwm_val));
  run_flag = true;
}

/*回転停止のメンバ関数*/
void MD30C::Stop() {
  analogWrite(pwm_pin, 0);
  run_flag = false;
}

/*ブレーキのメンバ関数*/
//このモータードライバにはブレーキモードはついてないのでStopで実装
void MD30C::Brake() {
  Stop();
}

/*出力変更のメンバ関数*/
//-255～255の範囲で設定
int MD30C::Speed(int val) {
  if (val > 255) val = 255;
  if (val < -255) val = -255;
  int old = pwm_val;
  pwm_val = val;
  if (run_flag) Run();
  return old;
}

/*回転方向変更のメンバ関数*/
bool MD30C::Reverse(bool flag) {
  bool old = dir_flag;
  dir_flag = flag;
  if (run_flag) Run();
  return old;
}

/*X配置のオムニクラス*/
class OmuniX {
  private:
    Motor* front_right;
    Motor* front_left;
    Motor* back_right;
    Motor* back_left;
  public:
    OmuniX(Motor*, Motor*, Motor*, Motor*);
    ~OmuniX();
  public:
    void Run();
    void Stop();
    void Brake();
    void Speed(int, int, int);
};

/*コンストラクタ*/
OmuniX::OmuniX(Motor* fr, Motor* fl, Motor* br, Motor* bl) {
  front_right = fr;
  front_left = fl;
  back_right = br;
  back_left = bl;
}

/*デストラクタ*/
OmuniX::~OmuniX() {
  delete front_right;
  delete front_left;
  delete back_right;
  delete back_left;
}

/*モータ起動*/
void OmuniX::Run() {
  front_right->Run();
  front_left->Run();
  back_right->Run();
  back_left->Run();
}

/*モータ停止*/
void OmuniX::Stop() {
  front_right->Stop();
  front_left->Stop();
  back_right->Stop();
  back_left->Stop();
}

/*ブレーキ*/
void OmuniX::Brake() {
  front_right->Brake();
  front_left->Brake();
  back_right->Brake();
  back_left->Brake();
}

/*オムニホイールの速度*/
//モーターの正転方向が次のようになっているものとする
//　↙　　↖
//　　○
//　↘　　↗

//x 左右
//y 前後
//w 回転

void OmuniX::Speed(int x, int y, int w) {
  front_right->Speed(int(-2.8 * (float)x + 2.8 * (float)y + (float)w));
  front_left->Speed(int(-2.8 * (float)x - 2.8 * (float)y + (float)w));
  back_right->Speed(int(2.8 * (float)x + 2.8 * (float)y + (float)w));
  back_left->Speed(int(2.8 * (float)x - 2.8 * (float)y + (float)w));
}

/*使い方*/
//MD30C(【DIRピン】, 【PWMピン】);
//MD30C(【DIRピン】, 【PWMピン】,【逆回転フラグ】);
OmuniX robot(new MD30C(2, 3), new MD30C(4, 5), new MD30C(7, 6), new MD30C(8, 9));

void setup() {
  //停止
  robot.Stop();
}

void loop() {
  //起動
  robot.Run();
  //反時計回り
  robot.Speed(0, 0, 100);
  delay(1000);
  //時計回り
  robot.Speed(0, 0, -100);
  delay(1000);
  //停止
  robot.Stop();
  delay(1000);
}
