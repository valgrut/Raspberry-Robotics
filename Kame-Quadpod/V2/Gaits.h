#include "Legs.h"


// ---------------------------------------------------------------------------------------
// ---------------------------------- Definitions ----------------------------------------
// ---------------------------------------------------------------------------------------

struct GaitStep
{
    int z[4];   // height (Z) values for 4 legs
    int xy[4];  // horizontal values (e.g. forward-back step phase)
};


// ---------------------------------------------------------------------------------------
// ------------------------------- Predefined Gaits --------------------------------------
// ---------------------------------------------------------------------------------------

// Walking forward (Step table)
//     {              Z               }            {       XY       }
const GaitStep basicWalkGate[] = {
    {{FRZ.mid, lup(FLZ, 35), lup(BRZ,40), BLZ.mid}, {190, 110, 18, 140}},  // step 0
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {190, 110, 18, 140}},  // step 1

    {{lup(FRZ,30), FLZ.mid, BRZ.mid, lup(BLZ,30)}, {145, 60, 64, 190}},  // step 2
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {145, 60, 64, 190}},  // step 2
};

// Walking backward (Step table)
const GaitStep backwardWalkGate[] = {
    {{FRZ.mid, lup(FLZ, 35), lup(BRZ,40), BLZ.mid}, {145, 60, 64, 190}},  // step 0
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {145, 60, 64, 190}},  // step 1

    {{lup(FRZ,30), FLZ.mid, BRZ.mid, lup(BLZ,30)}, {190, 110, 18, 140}},  // step 2
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {190, 110, 18, 140}},  // step 2
};

// Turn right on place
const GaitStep turnRightGait[] = {
    {{lup(FRZ,30), ldown(FLZ,20), ldown(BRZ,20), lup(BLZ,30)}, {150, 110, 60, 140}},  // step 0
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {190, 60, 20, 190}}, // step 1

    {{ldown(FRZ,20), lup(FLZ,30), lup(BRZ,35), ldown(BLZ,20)}, {190, 60, 20, 190}}, // step 2
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {150, 110, 60, 140}}, // step 3
};

// Turn left on place
const GaitStep turnLeftGait[] = {
    {{ldown(FRZ,20), lup(FLZ,30), lup(BRZ,35), ldown(BLZ,20)}, {150, 110, 60, 140}},  // step 0
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {190, 60, 20, 190}}, // step 1

    {{lup(FRZ,30), ldown(FLZ,20), ldown(BRZ,20), lup(BLZ,30)}, {190, 60, 20, 190}}, // step 2
    {{FRZ.mid, FLZ.mid, BRZ.mid, BLZ.mid}, {150, 110, 60, 140}}, // step 3
};



// ---------------------------------------------------------------------------------------
// ------------------------------- Predefined Moves --------------------------------------
// ---------------------------------------------------------------------------------------

void lay_flat()
{
  FLZ.target_value  = FLZ.floor_height;
  FRZ.target_value = FRZ.floor_height;
  BRZ.target_value  = BRZ.floor_height;
  BLZ.target_value   = BLZ.floor_height;

  FLXY.target_value  = FLXY.def;
  FRXY.target_value = FRXY.def;
  BRXY.target_value  = BRXY.def;
  BLXY.target_value   = BLXY.def;
}

void stand_up()
{
  FLZ.target_value  = FLZ.MAX;
  FRZ.target_value = FRZ.MAX;
  BRZ.target_value  = BRZ.MAX;
  BLZ.target_value   = BLZ.MAX;

  FLXY.target_value  = FLXY.def;
  FRXY.target_value = FRXY.def;
  BRXY.target_value  = BRXY.def;
  BLXY.target_value   = BLXY.def;
}

void setRestPose()
{
  FLZ.target_value  = FLZ.mid;
  FRZ.target_value = FRZ.mid;
  BRZ.target_value  = BRZ.mid;
  BLZ.target_value   = BLZ.mid;

  FLXY.target_value  = FLXY.def;
  FRXY.target_value = FRXY.def;
  BRXY.target_value  = BRXY.def;
  BLXY.target_value   = BLXY.def;
}