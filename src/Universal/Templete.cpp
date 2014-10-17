using namespace std;
#include <sstream>

template <typename T> string to_str(const T& t) {
   ostringstream os;
   os<<t;
   return os.str();
}
