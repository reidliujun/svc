#ifndef _TPERFECT_H_
#define _TPERFECT_H_

struct TxvidStats;
class Tperfect
{
private:
 void init(void);
 unsigned int *framesCount;unsigned int framesCountCount;
public:
 Tperfect(void);
 void analyze(const TxvidStats *stats);
 bool ready;
 double avg;
 double avgmin,avgmax;
 uint64_t Psize;
 unsigned int Pnum;
 unsigned int getFramesCount(unsigned int size);
};

#endif
