#include <iostream>
#include <vector>

#include "CommonAlgo.h"

// rasterCircle
// calc the pixels per row of a circle with center {x0,y0} and {radius}
class COMMONALGO_API rasterCircle {
   public:
    rasterCircle(int x0 = 0, int y0 = 0, int radius = 0);
    ~rasterCircle() {}
    std::vector<int>& XMin() { return *m_xmin; }
    std::vector<int>& XMax() { return *m_xmax; }

   private:
    std::vector<int>* m_xmin;
    std::vector<int>* m_xmax;
};
