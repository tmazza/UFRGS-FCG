class Camera {
public:
  Camera();
  bool frentePressed,trasPressed,upPressed,downtPressed;
  float x,y,z;
  void update();
  float rotx,roty;
};
