#ifndef _PARALLEL_COMM_H_
#define _PARALLEL_COMM_H_

#include <stdint.h>

// BASE (Data)
#define PPIN02 0x01 // OUT
#define PPIN03 0x02 // OUT
#define PPIN04 0x04 // OUT
#define PPIN05 0x08 // OUT
#define PPIN06 0x10 // OUT
#define PPIN07 0x20 // OUT
#define PPIN08 0x40 // OUT
#define PPIN09 0x80 // OUT - INVERTED

// BASE + 1 (Status)
#define PPIN10 0x40 // IN
#define PPIN11 0x80 // IN - INVERTED
#define PPIN12 0x20 // IN
#define PPIN13 0x10 // IN
#define PPIN15 0x08 // IN

// BASE + 2 (Control)
#define PPIN01 0x01 // IN/OUT - INVERTED
#define PPIN14 0x02 // IN/OUT
#define PPIN16 0x04 // IN/OUT
#define PPIN17 0x08 // IN/OUT - INVERTED

class ParallelPort {
  typedef std::vector< std::pair< std::string, uint32_t > > ParallelList;
public:

  ParallelPort();

  void list(); // Checks with windows the available parallel ports and their addresses
  void select( int32_t ); //
private:
  ParallelList devlist_;
  int32_t index_;
};

#endif
