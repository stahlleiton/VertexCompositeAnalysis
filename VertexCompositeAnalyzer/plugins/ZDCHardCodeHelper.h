#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalDigi/interface/QIE10DataFrame.h"
#include <vector>

class ZDCHardCodeHelper {
 public:
  ZDCHardCodeHelper() {
    setLowEdges(nbins_, binMin2);
  };
  virtual ~ZDCHardCodeHelper() {};
  double charge(unsigned fAdc, unsigned fCapId) const {
    unsigned range = range_(fAdc);
    unsigned index = 4*fCapId + range;
    return (center(fAdc) - exact_offsets[index]) / exact_slopes[index];
  }

 private:

  std::vector<double> mValues;
  const unsigned int nbins_ = 64;

  const double exact_offsets[16] = {-0.50000, -0.66002, 18.71833, -270.46150, -0.50000, -0.66002, 18.71833, -270.46150,
-0.50000, -0.66002, 18.71833, -270.46150, -0.50000, -0.66002, 18.71833, -270.46150}; 
  const double exact_slopes[16] = { 0.30683, 0.31046, 0.31191, 0.31766, 0.30683, 0.31046, 0.31191, 0.31766,
0.30683, 0.31046, 0.31191, 0.31766, 0.30683, 0.31046, 0.31191, 0.31766};

  const double binMin2[64] = {-0.5,  0.5,   1.5,   2.5,   3.5,   4.5,   5.5,   6.5,   7.5,   8.5,   9.5,
                     10.5,  11.5,  12.5,  13.5,  14.5,  // 16 bins with width 1x
                     15.5,  17.5,  19.5,  21.5,  23.5,  25.5,  27.5,  29.5,  31.5,  33.5,  35.5,
                     37.5,  39.5,  41.5,  43.5,  45.5,  47.5,  49.5,  51.5,  53.5,  // 20 bins with width 2x
                     55.5,  59.5,  63.5,  67.5,  71.5,  75.5,  79.5,  83.5,  87.5,  91.5,  95.5,
                     99.5,  103.5, 107.5, 111.5, 115.5, 119.5, 123.5, 127.5, 131.5, 135.5,  // 21 bins with width 4x
                     139.5, 147.5, 155.5, 163.5, 171.5, 179.5, 187.5};                      // 7 bins with width 8x
                                        
  double center(unsigned fAdc) const {
    if (fAdc < 4 * nbins_) {
      if (fAdc % nbins_ == nbins_ - 1)
        return 0.5 * (3 * mValues[fAdc] - mValues[fAdc - 1]);  // extrapolate
      else
        return 0.5 * (mValues[fAdc] + mValues[fAdc + 1]);  // interpolate
    }
    return 0.;
  };

  unsigned range_(unsigned fAdc) const {
    //6 bit mantissa in QIE10, 5 in QIE8
    return (nbins_ == 32) ? (fAdc >> 5) & 0x3 : (fAdc >> 6) & 0x3;
  };

  void expand() {
    int scale = 1;
    for (unsigned range = 1; range < 4; range++) {
      int factor = nbins_ == 32 ? 5 : 8;  // QIE8/QIE10 -> 5/8
      scale *= factor;
      unsigned index = range * nbins_;
      unsigned overlap = (nbins_ == 32) ? 2 : 3;  // QIE10 -> 3 bin overlap
      mValues[index] = mValues[index - overlap];  // link to previous range
      for (unsigned i = 1; i < nbins_; i++) {
        mValues[index + i] = mValues[index + i - 1] + scale * (mValues[i] - mValues[i - 1]);
      }
    }
    mValues[nbins_ * 4] = 2 * mValues[nbins_ * 4 - 1] - mValues[nbins_ * 4 - 2];  // extrapolate
  };

  bool setLowEdge(double fValue, unsigned fAdc) {
    if (fAdc >= nbins_)
      return false;
    mValues[fAdc] = fValue;
    return true;
  };

  bool setLowEdges(unsigned int nbins_, const double *fValue) {
    mValues.clear();
    mValues.resize(4 * nbins_ + 1);
    bool result = true;
    for (unsigned int adc = 0; adc < nbins_; adc++)
      result = result && setLowEdge(fValue[adc], adc);
    expand();
    return result;
  };
};
