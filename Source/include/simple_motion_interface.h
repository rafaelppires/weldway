#include <simplemotion.h>
#include <string>

class SimpleMotion {
public:
  SimpleMotion( std::string name );
  void startHoming();
private:
  std::string name_;
};
