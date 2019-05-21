//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <string.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Jiazhou Gao, Rui Yang";
const char *studentID   = "A53278947, ";
const char *email       = "j7gao@eng.ucsd.edu, ";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

uint32_t gBHSR;
int *gshareBHT;

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//


//------------------------------------//
//        Predictor Functions         //
uint8_t make_gshare_prediction(uint32_t pc){
	uint32_t index = (gBHSR ^ pc) & ((1<<ghistoryBits)-1) ;
	uint8_t gPrediction = gshareBHT[index];
	if (gPrediction == SN || gPrediction == WN) {
		return NOTTAKEN;
	}
	return TAKEN;
}
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  switch (bpType) {
    case STATIC:
      return;
    case GSHARE:
		//initialize gBHSR
		gBHSR = 0;
		// initialize gshareBHT
		size_t BHTsize = (1<<ghistoryBits)*sizeof(int);
		gshareBHT = (int *)malloc(BHTsize);
		memset(gshareBHT, WN, BHTsize);
		//printf ("%s%d\n", "after",gshareBHT[1]);
		break;

    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
  //
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
		return make_gshare_prediction(pc);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  switch (bpType) {
    case STATIC:
      return ;
    case GSHARE:
		// update gshareBHT (shift register)
		;
		int index;
		index = (gBHSR ^ pc) & ((1<<ghistoryBits)-1) ;
		uint8_t gPrediction = gshareBHT[(gBHSR ^ pc) & ((1<<ghistoryBits)-1)];
		
		if(outcome==NOTTAKEN){
			if(gPrediction != SN){
				gshareBHT[index]--;
			}
		}
		else{
			if(gPrediction != ST){
				gshareBHT[index]++;
			}
		}
		
		//update gBHSR
		gBHSR <<= 1;
		gBHSR |= outcome;

		break;

    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
  //
}
