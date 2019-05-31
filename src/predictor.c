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
#include "tage.h"
#include "percptron.h"
//
// TODO:Student Information
//
const char *studentName = "Jiazhou Gao, Rui Yang";
const char *studentID   = "A53278947, ";
const char *email       = "j7gao@eng.ucsd.edu, r3yang@eng.ucsd.edu ";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

uint32_t gBHSR;
int *gshareBHT;
int *localHT;    // pcIndexBits
int *lshareBHT;  // lhistoryBits
int *choicePredictor; // taken means global, not taken means local

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

uint8_t make_tourna_prediction(uint32_t pc){
	uint32_t index_global = gBHSR & ((1<<ghistoryBits)-1);
	uint8_t choice = choicePredictor[index_global];
	if (choice == SN || choice == WN){
		// not taken, choose local
		uint32_t local_history = localHT[pc & ((1<<pcIndexBits)-1)];
		uint32_t index_local = local_history & ((1<<lhistoryBits)-1);
		uint8_t lPrediction = lshareBHT[index_local];
		if (lPrediction == SN || lPrediction == WN) {
			return NOTTAKEN;
		}
		return TAKEN;

		uint8_t gPrediction = gshareBHT[index_global];
		if (gPrediction == SN || gPrediction == WN) {
			return NOTTAKEN;
		}
		return TAKEN;
	}else{
		// taken, choose global
		uint8_t gPrediction = gshareBHT[index_global];
		if (gPrediction == SN || gPrediction == WN) {
			return NOTTAKEN;
		}
		return TAKEN;
	}
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
    	gBHSR = 0;
    	size_t gBHTsize = (1<<ghistoryBits)*sizeof(int);
    	gshareBHT = (int *)malloc(gBHTsize);
    	choicePredictor = (int *)malloc(gBHTsize);
		memset(gshareBHT, WN, gBHTsize);
		memset(choicePredictor, WT, gBHTsize);


		size_t localhis_size = (1<<pcIndexBits)*sizeof(int);
		localHT = (int *)malloc(localhis_size);
		memset(localHT,0,localhis_size);

		size_t lBHTsize = (1<<lhistoryBits)*sizeof(int);
		lshareBHT = (int *)malloc(lBHTsize);
		memset(lshareBHT,WN,lBHTsize);
		break;
    case CUSTOM:
		init_perceptron();
		break;
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
    	return make_tourna_prediction(pc);
    case CUSTOM:
		return make_perceptron_prediction(pc);
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
    	;
    	
		int local_history = localHT[pc & ((1<<pcIndexBits)-1)];
		int index_local = local_history & ((1<<lhistoryBits)-1);

    	// update choice predictor, BHT(global or local)
    	
    	int index_global = gBHSR & ((1<<ghistoryBits)-1);

    	uint8_t choice = choicePredictor[index_global];
    	uint8_t gprediction = gshareBHT[index_global];
    	uint8_t lprediction = lshareBHT[index_local];
    	uint8_t prediction;
    	if (choice == SN || choice == WN){
    		prediction = lprediction;
    	}else{
    		prediction = gprediction;
    	}


    	if(outcome==NOTTAKEN){
			if(lprediction != SN){
				lshareBHT[index_local]--;
			}
			if(gprediction != SN){
				gshareBHT[index_global]--;

			}
			if((gprediction == SN || gprediction == WN) && (lprediction == ST || lprediction == WT)){
				if(choice!=ST){
					choicePredictor[index_global]++;
				}
			}else if((lprediction == SN || lprediction == WN) && (gprediction == ST || gprediction == WT)){
				if(choice != SN){
					choicePredictor[index_global]--;
				}
			}
		}
		else{
			if(lprediction != ST){
				lshareBHT[index_local]++;
			}
			if(gprediction != ST){
				gshareBHT[index_global]++;
			}
			if((lprediction == SN || lprediction == WN) && (gprediction == ST || gprediction == WT)){
				if(choice != ST){
					choicePredictor[index_global]++;
				}			
			}else if((gprediction == SN || gprediction == WN) && (lprediction == ST || lprediction == WT)){
				if(choice != SN){
					choicePredictor[index_global]--;
				}			
			}
		}

		//update global history
    	gBHSR <<= 1;
		gBHSR |= outcome;
    	
    	// update local history
    	localHT[pc & ((1<<pcIndexBits)-1)] <<= 1;
    	localHT[pc & ((1<<pcIndexBits)-1)] |= outcome;

    case CUSTOM:
		;

		train_perceptron(pc, outcome);
    default:
      break;
  }
  //
}
